#pragma once

#include "Components/ActorComponent.h"
#include "IPAddress.h"
#include "MetaClientSocket.generated.h"

USTRUCT(BlueprintType)
struct FTCPConnectionProperties
{
	GENERATED_BODY()

	/** Default sending socket IP string in form e.g. 127.0.0.1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	FString ConnectionIP = FString(TEXT("127.0.0.1"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	int32 ConnectionPort = 27777;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	FString ClientSocketName = FString(TEXT("unreal-tcp-client"));

	/** in bytes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	int32 BufferMaxSize = 2 * 1024 * 1024;	//default roughly 2mb

	/** If true will auto-connect on begin play to IP/port specified as a client. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	bool bShouldAutoConnectOnBeginPlay = true;

	/** Whether we should process our data on the game thread or the TCP thread. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	bool bReceiveDataOnGameThread = true;

	/** When a send failure occurs, should we automatically try to disconnect? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	bool bAutoDisconnectOnSendFailure = true;

	/** When a send failure occurs, should we automatically try to reconnect after disconnection? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	bool bAutoReconnectOnSendFailure = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTCPEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTCPMessageSignature, const TArray<uint8>&, Bytes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTCPClientSignature, const FString&, Client);

class MetaClientSocket
{
public:
	//Async events

	/** On message received on the receiving socket. */
	UPROPERTY(BlueprintAssignable, Category = "TCP Events")
	FTCPMessageSignature OnReceivedBytes;

	/** Callback when we've connected to end point*/
	UPROPERTY(BlueprintAssignable, Category = "TCP Events")
	FTCPEventSignature OnConnected;

	/** Callback when we've disconnected from end point only captured on send failure */
	UPROPERTY(BlueprintAssignable, Category = "TCP Events")
	FTCPEventSignature OnDisconnected;
	
	/**
	* Connect to a TCP endpoint, optional method if auto-connect is set to true.
	* Emit function will then work as long the network is reachable. By default
	* it will attempt this setup for this socket on beginplay.
	**/

	UFUNCTION(BlueprintCallable, Category = "TCP Functions")
	void ConnectToSocketAsClient(const FTCPConnectionProperties& MetaClientSocketOption);

	// Close the sending socket. This is usually automatically done on endplay.

	UFUNCTION(BlueprintCallable, Category = "TCP Functions")
	void CloseSocket();
	
	// Emit specified bytes to the TCP channel. 
	UFUNCTION(BlueprintCallable, Category = "TCP Functions")
	bool Emit(const TArray<uint8>& Bytes, const FTCPConnectionProperties& MetaClientSocketOption);
	
	UFUNCTION(BlueprintPure, Category = "TCP Functions")
	bool IsConnected();
	
protected:
	FSocket* ClientSocket = nullptr;
	FThreadSafeBool bShouldReceiveData;
	FThreadSafeBool bShouldAttemptConnection;
	TFuture<void> ClientConnectionFinishedFuture;

	//FTCPSocketReceiver* TCPReceiver;
	FString SocketDescription;
	TSharedPtr<FInternetAddr> RemoteAddress;

private:
	bool bCloseSocket = false; // han - Variable to prevent program from can't quit due to infinite repetition of threads
};

static TFuture<void> RunLambdaOnBackGroundThread(TFunction< void()> InFunction);
static TFuture<void> RunLambdaOnGameThread(TFunction< void()> InFunction);