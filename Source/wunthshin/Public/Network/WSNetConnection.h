#pragma once
#include "IpConnection.h"
#include "Containers/CircularQueue.h"

#include "WSNetConnection.generated.h"

UCLASS()
class WUNTHSHIN_API UWSNetConnection : public UIpConnection
{
	GENERATED_BODY()
	friend class UWSNetDriver;
public:
	UWSNetConnection();
	
	virtual void LowLevelSend(void* Data, int32 CountBits, FOutPacketTraits& Traits) override;

	virtual void OnConnect(bool bInNetDriverRecvThread);

	virtual struct MessageBase* GetPacket();

	virtual void TickDispatch(float DeltaTime);

protected:
	bool ReadPacketSome(const uint32 InReadSize);
	bool ReadPacket();

	struct FRawReceivedPacket
	{
		TArray<uint8> PacketBytes;
		ESocketErrors Error;
		double PlatformTimeSeconds;

		FRawReceivedPacket()
			: Error(SE_NO_ERROR)
			, PlatformTimeSeconds(0.0)
		{
		}
	};

	TCircularQueue<FRawReceivedPacket> ReceiveQueue{ 1 << 10 };

	bool DispatchPacket(FRawReceivedPacket& IncomingPacket);
	bool ProcessPacket(FRawReceivedPacket& InPacket);

	FRawReceivedPacket RecvBuffer;
	int32 RecvPacketSize = 0;
	bool bNetDriverRecvThread = false;

};
