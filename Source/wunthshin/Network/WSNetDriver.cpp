#include "WSNetDriver.h"
#include "Sockets.h"
#include "wunthshin/Network/WSNetConnection.h"

bool UWSNetDriver::InitConnectionClass()
{
    NetConnectionClassName = TEXT("/Script/wunthshin.WSNetConnection");
    return Super::InitConnectionClass();
}

bool UWSNetDriver::InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error)
{
	bool bResult = Super::InitConnect(InNotify, ConnectURL, Error);
	if (!bResult)
	{
		return bResult;
	}

	WSServerConnection = Cast<UWSNetConnection>(ServerConnection);
	GetSocket()->SetNonBlocking(false);
	GetSocket()->SetNoDelay(true);

	if (GetSocket()->Connect(*WSServerConnection->RemoteAddr))
	{
		GetSocket()->SetNonBlocking();

		WSServerConnection->OnConnect(bRecvThread);
		if (bRecvThread)
		{
			ConsumerThread = MakeUnique<decltype(ConsumerThread)::ElementType>(this);
			ConsumerThreadHandle.Reset(
				FRunnableThread::Create(
					ConsumerThread.Get(), 
					*FString::Printf(TEXT("WSNetDriver Receive Thread"))));
		}
		return true;
	}
	else
	{
		ServerConnection = nullptr;
		WSServerConnection = nullptr;
		return false;
	}
}

void UWSNetDriver::TickDispatch(float DeltaTime)
{
	UNetDriver::TickDispatch(DeltaTime);

	if (!WSServerConnection) { return; }

	if (!bRecvThread)
	{
		while (true)
		{
			if (!WSServerConnection->ReadPacket())
			{
				ISocketSubsystem* SocketSubsystem = GetSocketSubsystem();
				ESocketErrors Error = SocketSubsystem->GetLastErrorCode();

				if (Error == SE_ECONNRESET || Error == SE_ENOTCONN)
				{
					Shutdown();
					return;
				}
				else if (Error == SE_EWOULDBLOCK)
				{
					break;
				}
			}
		}
	}

	WSServerConnection->TickDispatch(DeltaTime);
}

void UWSNetDriver::LowLevelDestroy()
{
	if (FSocket* CurrentSocket = GetSocket();
		CurrentSocket != nullptr && !HasAnyFlags(RF_ClassDefaultObject))
	{
		ISocketSubsystem* SocketSubsystem = GetSocketSubsystem();

		if ( ConsumerThreadHandle.IsValid() && ConsumerThread.IsValid() )
		{
			ConsumerThreadHandle->Kill( true );
			if ( !CurrentSocket->Shutdown(ESocketShutdownMode::Read) )
			{
				const ESocketErrors ShutdownError = SocketSubsystem->GetLastErrorCode();
				UE_LOG(LogNet, Log, TEXT("%hs: Socket->Shutdown returned error %s (%d) for %s"), __FUNCTION__, SocketSubsystem->GetSocketError(ShutdownError), static_cast<int>(ShutdownError), *GetDescription());
			}
		}

		if (!CurrentSocket->Close())
		{
			UE_LOG(LogExit, Log, TEXT("closesocket error (%i)"), (int32)SocketSubsystem->GetLastErrorCode());
		}
	}

	Super::LowLevelDestroy();
}

FUniqueSocket UWSNetDriver::CreateSocketForProtocol(const FName& ProtocolType)
{
	if (ISocketSubsystem* SocketSubsystem = GetSocketSubsystem())
	{
		return SocketSubsystem->CreateUniqueSocket(NAME_Stream, TEXT("WS"), ProtocolType);
	}

	UE_LOG(LogNet, Warning, TEXT("%hs: Unable to find socket subsystem"), __FUNCTION__);
	return nullptr;
}

FUniqueSocket UWSNetDriver::CreateAndBindSocket(TSharedRef<FInternetAddr> BindAddr, int32 Port, bool bReuseAddressAndPort, int32 DesiredRecvSize, int32 DesiredSendSize, FString& Error)
{
	if (ISocketSubsystem* SocketSubsystem = GetSocketSubsystem())
	{
		if (FUniqueSocket NewSocket = CreateSocketForProtocol(BindAddr->GetProtocolType()))
		{
			if (!NewSocket->SetReuseAddr(bReuseAddressAndPort))
			{
				UE_LOG(LogNet, Error, TEXT("setsockopt with SO_REUSEADDR failed"));
				return nullptr;
			}

			if (!NewSocket->SetRecvErr())
			{
				UE_LOG(LogNet, Error, TEXT("setsockopt with IP_RECVERR failed"));
				return nullptr;
			}

			int32 ActualRecvSize(0);
			int32 ActualSendSize(0);

			if (!NewSocket->SetReceiveBufferSize(DesiredRecvSize, ActualRecvSize))
			{
				UE_LOG(LogNet, Error, TEXT("SetReceiveBufferSize failed"));
				return nullptr;
			}

			if (!NewSocket->SetSendBufferSize(DesiredSendSize, ActualSendSize))
			{
				UE_LOG(LogNet, Error, TEXT("SetSendBufferSize failed"));
				return nullptr;
			}
			
			UE_LOG(LogInit, Log, TEXT("%s: Socket queue. Rx: %i (config %i) Tx: %i (config %i)"), SocketSubsystem->GetSocketAPIName(),
				ActualRecvSize, DesiredRecvSize, ActualSendSize, DesiredSendSize);

			// Bind socket to our port.
			BindAddr->SetPort(Port);

			int32 AttemptPort = BindAddr->GetPort();
			int32 BoundPort = SocketSubsystem->BindNextPort(NewSocket.Get(), *BindAddr, MaxPortCountToTry + 1, 1);
			if (BoundPort == 0)
			{
				Error = FString::Printf(TEXT("%s: binding to port %i failed (%i)"), SocketSubsystem->GetSocketAPIName(), AttemptPort,
					(int32)SocketSubsystem->GetLastErrorCode());

				if (bExitOnBindFailure)
				{
					UE_LOG(LogNet, Fatal, TEXT("Fatal error: %s"), *Error);
				}

				return nullptr;
			}

			if (!NewSocket->SetNonBlocking())
			{
				Error = FString::Printf(TEXT("%s: SetNonBlocking failed (%i)"), SocketSubsystem->GetSocketAPIName(),
					(int32)SocketSubsystem->GetLastErrorCode());
				return nullptr;
			}

			return NewSocket;
		}

		Error = FString::Printf(TEXT("%s: socket failed (%i)"), SocketSubsystem->GetSocketAPIName(), (int32)SocketSubsystem->GetLastErrorCode());
		return nullptr;
	}

	Error = FString::Printf(TEXT("Unable to load the socket subsystem!"));
	return nullptr;
}

UWSNetDriver::FWSReceiveThreadRunnable::FWSReceiveThreadRunnable(UWSNetDriver* InOwningNetDriver) : OwningNetDriver(InOwningNetDriver)
{
}

bool UWSNetDriver::FWSReceiveThreadRunnable::IsInitialized()
{
	return OwningNetDriver != nullptr;
}

bool UWSNetDriver::FWSReceiveThreadRunnable::Init()
{
	SocketSubsystem = OwningNetDriver->GetSocketSubsystem();

	if (SocketSubsystem && OwningNetDriver)
	{
		bool Expected = false;
		while ( !bIsRunning.CompareExchange(Expected, true) ) {}
	}

	return SocketSubsystem && OwningNetDriver;
}

uint32 UWSNetDriver::FWSReceiveThreadRunnable::Run()
{
	while (bIsRunning && OwningNetDriver)
	{
		if ( !OwningNetDriver->GetWSServerConnection()->ReadPacket() )
		{
			ESocketErrors Error = SocketSubsystem->GetLastErrorCode();

			if (Error == SE_ECONNRESET || Error == SE_ENOTCONN)
			{
				break;
			}
			else if (Error == SE_EWOULDBLOCK)
			{
				continue;
			}
		}
	}

	UE_LOG(LogNet, Log, TEXT("%hs returning"), __FUNCTION__);
	return 0;
}

void UWSNetDriver::FWSReceiveThreadRunnable::Stop()
{
	if (bIsRunning)
	{
		bool Expected = true;
		while ( bIsRunning.CompareExchange(Expected, false) ) {}
	}
}
