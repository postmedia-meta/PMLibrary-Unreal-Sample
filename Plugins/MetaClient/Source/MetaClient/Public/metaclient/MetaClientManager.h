#pragma once

#include "CoreMinimal.h"
#include "MetaClientSocket.h"
#include "proto/proto_protocol.h"
#include "MetaClientManager.generated.h"

struct FTCPConnectionProperties;
class MetaClientSocket;

USTRUCT(BlueprintType)
struct FMetaMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	int32 Number;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	FString Text;
};

USTRUCT(BlueprintType)
struct FLiDARPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	int32 idx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	int32 x;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	int32 y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	float originPercentX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	float originPercentY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	float percentX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	float percentY;
};

USTRUCT(BlueprintType)
struct FLiDARFrameData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	FString screen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	TArray<FLiDARPosition> newTouches;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	TArray<FLiDARPosition> removeTouches;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	TArray<FLiDARPosition> updateTouches;
};

USTRUCT(BlueprintType)
struct FLiDARMapperMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	FLiDARFrameData body;
};

USTRUCT(BlueprintType)
struct FMetaBinaryMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	TArray<uint8> Buffer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meta Client")
	FString Info;
};


// DECLARE_DYNAMIC_DELEGATE_OneParam			// 1:1
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam	// 1:N
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMetaMessage, const FMetaMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFLiDARMapperMessage, const FLiDARMapperMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMetaBinaryMessage, const FMetaBinaryMessage&, Message);


class MyReceiver : public FBE::proto::Receiver
{
protected:
	virtual void onReceive(const proto::MetaMessage& value) override;
	virtual void onReceive(const proto::MetaBinaryMessage& value) override;
	virtual void onReceive(const proto::LiDARMapperMessage& value) override;
	virtual void onReceiveLog(const std::string& message) const override;

public:
	UMetaClientManager *MetaClientManager;
};

UCLASS(ClassGroup = "Networking", meta = (BlueprintSpawnableComponent))
class METACLIENT_API UMetaClientManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection Properties")
	FTCPConnectionProperties TCPConnectionProperties;
	
	UFUNCTION(BlueprintCallable, Category="Meta Client")
	void ReceiveProto(const TArray<uint8>& Bytes);

	UFUNCTION(BlueprintCallable, Category="Meta Client")
	void Ping();

	UFUNCTION(BlueprintCallable, Category="Meta Client")
	void ResetBuffer();
	
	UFUNCTION(BlueprintCallable, Category="Meta Client|SendMessage")
	void SendMetaMessage(const FMetaMessage& MetaMessage);

	// Han - Send Binary Message
	UFUNCTION(BlueprintCallable, Category="Meta Client|SendMessage")
	void SendMetaBinaryMessage(const FMetaBinaryMessage& MetaBinaryMessage);
	
	UPROPERTY(BlueprintAssignable, Category = "Meta Client Events")
	FOnEventSignature OnConnected;

	UPROPERTY(BlueprintAssignable, Category = "Meta Client Events")
	FOnEventSignature OnDisconnected;
	
	UPROPERTY(BlueprintAssignable, Category = "Meta Client Events")
	FOnMetaMessage OnMetaMessage;

	UPROPERTY(BlueprintAssignable, Category = "Meta Client Events")
	FOnMetaBinaryMessage OnMetaBinaryMessage;

	UPROPERTY(BlueprintAssignable, Category = "Meta Client Events")
	FOnFLiDARMapperMessage OnLiDARMapperMessage;

	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	MyReceiver receiver;
	MetaClientSocket MetaClientSocket;

	// Han
	UFUNCTION()
	void Connected();
	UFUNCTION()
	void Disconnected();
	
	// Han - Byte TArray to std::vector
	static const std::vector<uint8_t>& TArrayToStdVector(const TArray<uint8>& Buffer);
};

inline const std::vector<uint8_t>& UMetaClientManager::TArrayToStdVector(const TArray<uint8>& Buffer)
{
	static std::vector<uint8_t> Vector;
	Vector.clear();
	Vector.resize(Buffer.Num());
	
	for(int i = 0; i < Buffer.Num(); i++)
	{
		Vector[i] = Buffer[i];
	}
	
	return Vector;
}