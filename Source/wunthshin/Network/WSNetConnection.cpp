#include "WSNetConnection.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Net/Common/Packets/PacketTraits.h"
#include "message.h"
#include "wunthshin/Network/Channel/WSChannelBase.h"

UWSNetConnection::UWSNetConnection()
{
	DisableAddressResolution();
}

void UWSNetConnection::LowLevelSend(void* Data, int32 CountBits, FOutPacketTraits& Traits)
{
	if (CountBits == 0 || Data == nullptr)
	{
		UE_LOG(LogNet, Warning, TEXT("%hs: CountBits == 0 || Data == nullptr"), __FUNCTION__);
		check(false);
		return;
	}

	int32 SentBytes = 0;
	if (!Socket->Send((const uint8*)Data, CountBits, SentBytes))
	{
		UE_LOG(LogNet, Warning, TEXT("%hs: LowLevelSend: Server로 Data를 보낼 수 없습니다. %d"), __FUNCTION__, CountBits);
		check(false);
	}

	if (SentBytes != CountBits)
	{
		UE_LOG(LogNet, Warning, TEXT("%hs: LowLevelSend: SentBytes(%d) != CountBits(%d)"), __FUNCTION__, SentBytes, CountBits);
		check(false);
		return;
	}

	if (SentBytes == -1)
	{
		UE_LOG(LogNet, Warning, TEXT("%hs: LowLevelSend: SentBytes == -1"), __FUNCTION__);
		check(false);
		return;
	}
}

void UWSNetConnection::OnConnect(bool bInNetDriverRecvThread)
{
	SetConnectionState(EConnectionState::USOCK_Open);

	bNetDriverRecvThread = bInNetDriverRecvThread;
	RecvBuffer.PacketBytes.SetNumUninitialized(std::numeric_limits<uint16_t>::max(), false);
}

MessageBase* UWSNetConnection::GetPacket()
{
	MessageBase* Bunch = (MessageBase*)RecvBuffer.PacketBytes.GetData();
	return Bunch;
}

void UWSNetConnection::TickDispatch(float DeltaTime)
{
	if (bNetDriverRecvThread)
	{
		FRawReceivedPacket IncomingPacket;
		
		while (bNetDriverRecvThread)
		{
			if ( ReceiveQueue.Dequeue(IncomingPacket) )
			{
				ProcessPacket(IncomingPacket);
			}
		}
	}

	if (GetConnectionState() == EConnectionState::USOCK_Closed)
	{
		RequestEngineExit(TEXT("Server Connection lost"));
	}
}

bool UWSNetConnection::ReadPacketSome(const uint32 InReadSize)
{
	if (InReadSize == 0)
	{
		return false;
	}

	const int32 MaxRecvSize = RecvPacketSize + InReadSize;
	if (MaxRecvSize > RecvBuffer.PacketBytes.Num())
	{
		UE_LOG(LogNet, Error, TEXT("MaxRecvSize > RecvBuffer.PacketBytes.Num(): %d > %d"), MaxRecvSize, RecvBuffer.PacketBytes.Num());
		return false;
	}

	int32 BytesRead = 0;
	if (Socket->Recv(&RecvBuffer.PacketBytes[RecvPacketSize], InReadSize, BytesRead))
	{
		RecvPacketSize += BytesRead;

		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		ESocketErrors Error = SocketSubsystem->GetLastErrorCode();

		if (BytesRead == 0)
		{
			if (Error == SE_EWOULDBLOCK)
			{
				return false;
			}
			else
			{
				check(false);
			}
		}

		return true;
	}
	else if (BytesRead == 0)
	{
		SetConnectionState(EConnectionState::USOCK_Closed);
		return false;
	}

	check(false);
	return false;
}

bool UWSNetConnection::ReadPacket()
{
	MessageBase* Bunch = reinterpret_cast<MessageBase*>(RecvBuffer.PacketBytes.GetData());
	if ((size_t)Bunch->GetType() < 0 || (size_t)Bunch->GetType() >= (size_t)EMessageType::MAX)
	{
		// Array out of range
		return false;
	}

	const int32 PacketSize = G_MessageSize[ (size_t)Bunch->GetType() ];
	const int32 ReadPacketSize = PacketSize - RecvPacketSize;
	const bool bRead = ReadPacketSome(ReadPacketSize);

	if ((bRead || ReadPacketSize == 0) && RecvPacketSize >= PacketSize)
	{
		if (RecvPacketSize != PacketSize)
		{
			return false;
		}

		RecvPacketSize = 0;
		RecvBuffer.PlatformTimeSeconds = FPlatformTime::Seconds();

		if (!bNetDriverRecvThread)
		{
			return ProcessPacket(RecvBuffer);
		}
		else
		{
			return DispatchPacket(RecvBuffer);
		}
	}

	return false;
}

bool UWSNetConnection::DispatchPacket(FRawReceivedPacket& IncomingPacket)
{
	return ReceiveQueue.Enqueue(IncomingPacket);
}

bool UWSNetConnection::ProcessPacket(FRawReceivedPacket& InPacket)
{
	MessageBase* Bunch = reinterpret_cast<MessageBase*>(InPacket.PacketBytes.GetData());
	const int32 ChannelIndex = G_MessageChannels[ ( size_t )Bunch->GetType() ];

	if (ChannelIndex < 0 || ChannelIndex >= ( int32 )EMessageChannelType::MAX)
	{
		return false;
	}

	if ( Channels.IsValidIndex( ChannelIndex ) && Channels[ ChannelIndex ] )
	{
		UWSChannelBase* Channel = Cast<UWSChannelBase>(Channels[ChannelIndex]);
		if (Channel)
		{
			Channel->ReceivedBunch(*Bunch);
			return true;
		}
		else
		{
			check(false);
		}
	}
	check(false);
	return false;
}
