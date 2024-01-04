// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaClientManager.h"

#include "PMLibrary/Public/PMLogMacros.h"

void UMetaClientManager::InitializeComponent()
{
	Super::InitializeComponent();
}

void UMetaClientManager::UninitializeComponent()
{
	Super::UninitializeComponent();
}

void UMetaClientManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Enable logging
	receiver.logging(true);
	receiver.MetaClientManager = this;
}

void UMetaClientManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UMetaClientManager::ReceiveProto(const TArray<uint8>& Bytes)
{
	PM_LOG("UMetaClientManager ReceiveProto Bytes: %d", Bytes.Num());
	receiver.receive(Bytes.GetData(), Bytes.Num());
}

void UMetaClientManager::Ping()
{
	FMetaMessage Message;
	Message.Type = TEXT("PING");
	Message.Number = 0;
	Message.Text = nullptr;
	
	SendMetaMessage(Message);
}

void UMetaClientManager::ResetBuffer()
{
	receiver.reset();
}

void UMetaClientManager::SendMetaMessage(const FMetaMessage& MetaMessage)
{
	proto::MetaMessage message;
	message.body.type = TCHAR_TO_UTF8(*MetaMessage.Type);
	message.body.number = MetaMessage.Number;
	message.body.text = TCHAR_TO_UTF8(*MetaMessage.Text);

	// Serialize the account to the FBE stream
	FBE::proto::MetaMessageModel writer;
	writer.serialize(message);
	if (writer.Verify())
	{
		TArray Bytes(writer.buffer().data(), writer.buffer().size());
		TCPClient->Emit(Bytes);
	}
}

void UMetaClientManager::SendMetaBinaryMessage(const FMetaBinaryMessage& MetaBinaryMessage)
{
	proto::MetaBinaryMessage message;
	message.body.type = TCHAR_TO_UTF8(*MetaBinaryMessage.Type);
	message.body.data = TArrayToStdVector(MetaBinaryMessage.Buffer);
	message.body.info = TCHAR_TO_UTF8(*MetaBinaryMessage.Info);

	// Serialize the account to the FBE stream
	FBE::proto::MetaBinaryMessageModel writer;
	writer.serialize(message);
	if (writer.Verify())
	{
		TArray Bytes(writer.buffer().data(), writer.buffer().size());
		TCPClient->Emit(Bytes);
	}
}

void MyReceiver::onReceive(const proto::MetaMessage& value)
{
	Receiver::onReceive(value);

	FMetaMessage message;
	message.Type = FString(UTF8_TO_TCHAR(value.body.type.c_str()));
	message.Number = value.body.number.value_or(0);
	message.Text = FString(UTF8_TO_TCHAR(value.body.text.value_or("").c_str()));

	PM_LOG("MyReceiver onReceive MetaMessage type: %s", *message.Type);
	
	MetaClientManager->OnMetaMessage.Broadcast(message);
}

void MyReceiver::onReceive(const proto::MetaBinaryMessage& value)
{
	Receiver::onReceive(value);

	FMetaBinaryMessage message;
	message.Type = FString(UTF8_TO_TCHAR(value.body.type.c_str()));
	message.Buffer.Append(&value.body.data.buffer()[0], value.body.data.buffer().size());
	message.Info = FString(UTF8_TO_TCHAR(value.body.info.value_or("").c_str()));
	
	PM_LOG("MyReceiver onReceive MetaBinaryMessage type : %s", *message.Type);
	
	MetaClientManager->OnMetaBinaryMessage.Broadcast(message);
}

void MyReceiver::onReceive(const proto::LiDARMapperMessage& value)
{
	Receiver::onReceive(value);

	FLiDARFrameData frame;
	frame.screen = FString(UTF8_TO_TCHAR(value.body.screen.c_str()));
	for (auto touch : value.body.newTouches)
	{
		FLiDARPosition tc;
		tc.idx = touch.idx;
		tc.x = touch.x;
		tc.y = touch.y;
		tc.originPercentX = touch.originPercentX;
		tc.originPercentY = touch.originPercentY;
		tc.percentX = touch.percentX;
		tc.percentY = touch.percentY;
		frame.newTouches.Add(tc);
	}
	for (auto touch : value.body.removeTouches)
	{
		FLiDARPosition tc;
		tc.idx = touch.idx;
		tc.x = touch.x;
		tc.y = touch.y;
		tc.originPercentX = touch.originPercentX;
		tc.originPercentY = touch.originPercentY;
		tc.percentX = touch.percentX;
		tc.percentY = touch.percentY;
		frame.removeTouches.Add(tc);
	}
	for (auto touch : value.body.updateTouches)
	{
		FLiDARPosition tc;
		tc.idx = touch.idx;
		tc.x = touch.x;
		tc.y = touch.y;
		tc.originPercentX = touch.originPercentX;
		tc.originPercentY = touch.originPercentY;
		tc.percentX = touch.percentX;
		tc.percentY = touch.percentY;
		frame.updateTouches.Add(tc);
	}
	
	FLiDARMapperMessage message;
	message.body = frame;
	
	PM_LOG("MyReceiver onReceive LiDARMapperMessage type: %s", *message.body.screen);
	
	MetaClientManager->OnLiDARMapperMessage.Broadcast(message);
}

void MyReceiver::onReceiveLog(const std::string& message) const
{
	Receiver::onReceiveLog(message);
	FString msg = FString(UTF8_TO_TCHAR(message.c_str()));
	//PM_LOG("MyReceiver onReceiveLog message: %s", *msg);
}