
#include "metaclient/MetaClientSocket.h"

//#include "IPAddressAsyncResolve.h"  // v5.2 or higher
#include "IPAddress.h"
#include "Async/Async.h"
#include "SocketSubsystem.h"
#include "Networking.h"

void MetaClientSocket::ConnectToSocketAsClient(const FTCPConnectionProperties& MetaClientSocketOption)
{
	//Already connected? attempt reconnect
	if (IsConnected())
	{
		CloseSocket();
		ConnectToSocketAsClient(MetaClientSocketOption); // han
	}

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	if (SocketSubsystem == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("TCPClientComponent: SocketSubsystem is nullptr"));
        return;
    }

	const auto ResolveInfo = SocketSubsystem->GetHostByName(TCHAR_TO_ANSI(*MetaClientSocketOption.ConnectionIP));
	while (!ResolveInfo->IsComplete()) {}

	const auto Error = ResolveInfo->GetErrorCode();

	if (Error != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("TCPClientComponent: DNS resolve error code %d"), Error);
        return;
    }

	RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	RemoteAddress->SetRawIp(ResolveInfo->GetResolvedAddress().GetRawIp()); // todo: somewhat wasteful, we could probably use the same address object?
	RemoteAddress->SetPort(MetaClientSocketOption.ConnectionPort);

	ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, MetaClientSocketOption.ClientSocketName, false);
	
	//Set Send Buffer Size
	int32 BufferMaxSize = MetaClientSocketOption.BufferMaxSize; // han
	ClientSocket->SetSendBufferSize(BufferMaxSize, BufferMaxSize);
	ClientSocket->SetReceiveBufferSize(BufferMaxSize, BufferMaxSize);

	//Listen for data on our end
	ClientConnectionFinishedFuture = RunLambdaOnBackGroundThread([&]()
	{
		double LastConnectionCheck = FPlatformTime::Seconds();

		uint32 BufferSize = 0;
		TArray<uint8> ReceiveBuffer;
		bShouldAttemptConnection = true;
		bCloseSocket = false; // han - Initialize variable to false when attempting to connect to the server

		while (bShouldAttemptConnection && !bCloseSocket)
		{
			if (ClientSocket->Connect(*RemoteAddress))
			{
				RunLambdaOnGameThread([&]()
				{
					OnConnected.Broadcast();
				});
				bShouldAttemptConnection = false;
				continue;
			}
		
			//reconnect attempt every 3 sec
			FPlatformProcess::Sleep(3.f);
		}

		bShouldReceiveData = true;

		while (bShouldReceiveData && !bCloseSocket)
		{
			if (ClientSocket->HasPendingData(BufferSize))
			{
				ReceiveBuffer.SetNumUninitialized(BufferSize);

				int32 Read = 0;
				ClientSocket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read);

				if (MetaClientSocketOption.bReceiveDataOnGameThread) // han
				{
					//Copy buffer so it's still valid on game thread
					TArray<uint8> ReceiveBufferGT;
					ReceiveBufferGT.Append(ReceiveBuffer);
				
					//Pass the reference to be used on game thread
					AsyncTask(ENamedThreads::GameThread, [&, ReceiveBufferGT]()
					{
						OnReceivedBytes.Broadcast(ReceiveBufferGT);
					});
				}
				else
				{
					OnReceivedBytes.Broadcast(ReceiveBuffer);
				}
			}
			//sleep until there is data or 10 ticks (0.1micro seconds
			ClientSocket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan(10));
		}
	});
}

void MetaClientSocket::CloseSocket()
{
	if (ClientSocket)
	{
		bCloseSocket = true; // han - When the socket is disconnected, change to true to exit the thread.
		bShouldReceiveData = false;
		ClientConnectionFinishedFuture.Get();
		
		ClientSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
		ClientSocket = nullptr;
	
		OnDisconnected.Broadcast();
	}
}

bool MetaClientSocket::Emit(const TArray<uint8>& Bytes, const FTCPConnectionProperties& MetaClientSocketOption)
{
	if (IsConnected())
	{
		int32 BytesSent = 0;
		bool bDidSend = ClientSocket->Send(Bytes.GetData(), Bytes.Num(), BytesSent);
		

		//If we're supposedly connected but failed to send
		if (IsConnected() && !bDidSend)
		{
			UE_LOG(LogTemp, Warning, TEXT("Sending Failure detected"));

			if (MetaClientSocketOption.bAutoDisconnectOnSendFailure)
			{
				UE_LOG(LogTemp, Warning, TEXT("disconnecting socket."));
				CloseSocket();
			}
			
			if (MetaClientSocketOption.bAutoReconnectOnSendFailure)
			{
				UE_LOG(LogTemp, Warning, TEXT("reconnecting..."));
				ConnectToSocketAsClient(MetaClientSocketOption);
			}
		}
		return bDidSend;
	}
	return false;
}

bool MetaClientSocket::IsConnected()
{
	return (ClientSocket && (ClientSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected));
}

static TFuture<void> RunLambdaOnBackGroundThread(TFunction< void()> InFunction)
{
	return Async(EAsyncExecution::Thread, InFunction);
}

static TFuture<void> RunLambdaOnGameThread(TFunction< void()> InFunction)
{
	return Async(EAsyncExecution::TaskGraphMainThread, InFunction);
}	