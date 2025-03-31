#pragma once
#include "IpNetDriver.h"
#include "WSNetDriver.generated.h"

class UWSNetDriver;
class UWSNetConnection;
class ISocketSubsystem;

UCLASS()
class WUNTHSHIN_API UWSNetDriver : public UIpNetDriver
{
	GENERATED_BODY()
public:
	virtual bool InitConnectionClass() override;
	virtual bool InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error) override;
	virtual void TickDispatch(float DeltaTime) override;
	virtual void LowLevelDestroy() override;
	UWSNetConnection* GetWSServerConnection() { return WSServerConnection; }

	virtual FUniqueSocket CreateSocketForProtocol(const FName& ProtocolType);
	virtual FUniqueSocket CreateAndBindSocket(TSharedRef<FInternetAddr> BindAddr, int32 Port, bool bReuseAddressAndPort, int32 DesiredRecvSize, int32 DesiredSendSize, FString& Error) override;

private:
	/** Runnable object representing the receive thread, if enabled. */
	class FWSReceiveThreadRunnable : public FRunnable
	{
	public:
		FWSReceiveThreadRunnable(UWSNetDriver* InOwningNetDriver);

		bool IsInitialized();
		virtual bool Init() override;
		virtual uint32 Run() override;
		virtual void Stop() override;

	public:
		/** Running flag. The Run() function will return shortly after setting this to false. */
		TAtomic<bool> bIsRunning = false;

	private:
		/** The NetDriver which owns the receive thread */
		UWSNetDriver* OwningNetDriver;

		/** The socket subsystem used by the receive thread */
		ISocketSubsystem* SocketSubsystem;
	};

	/** Receive thread runnable object. */
	TUniquePtr<FWSReceiveThreadRunnable> ConsumerThread;
	TUniquePtr<FRunnableThread> ConsumerThreadHandle;
	bool bRecvThread = true;

private:
	UWSNetConnection* WSServerConnection = nullptr;

};
