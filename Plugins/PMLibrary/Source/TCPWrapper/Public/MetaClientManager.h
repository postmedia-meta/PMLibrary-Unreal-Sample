#pragma once

#include "CoreMinimal.h"
#include "TCPWrapper/proto/proto_protocol.h"
#include "TCPClientComponent.h"
#include "MetaClientManager.generated.h"


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

// Han
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMetaMessage, const FMetaMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFLiDARMapperMessage, const FLiDARMapperMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMetaBinaryMessage, const FMetaBinaryMessage&, Message); // Han


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
class TCPWRAPPER_API UMetaClientManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Meta Client")
	void ReceiveProto(const TArray<uint8>& Bytes);

	UFUNCTION(BlueprintCallable, Category="Meta Client")
	void Ping();

	UFUNCTION(BlueprintCallable, Category="Meta Client")
	void ResetBuffer();
	
	// UFUNCTION(BlueprintCallable)
	// void SendMetaMessage(const FString& Type);

	// Han
	UFUNCTION(BlueprintCallable, Category="Meta Client")
	void SendMetaMessage(const FMetaMessage& MetaMessage);

	UPROPERTY(BlueprintAssignable, Category = "Meta Client Events")
	FOnMetaMessage OnMetaMessage;

	UPROPERTY(BlueprintAssignable, Category = "Meta Client Events")
	FOnFLiDARMapperMessage OnFLiDARMapperMessage;

	UPROPERTY(BlueprintAssignable, Category = "Meta Client Events") // Han
	FOnMetaBinaryMessage OnMetaBinaryMessage;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Server")
	UTCPClientComponent *TCPClient;

	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	MyReceiver receiver;
};
