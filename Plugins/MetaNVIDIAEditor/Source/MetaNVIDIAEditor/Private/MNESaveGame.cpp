// Fill out your copyright notice in the Description page of Project Settings.


#include "MNESaveGame.h"
#include "Kismet/GameplayStatics.h"

UMNESaveGame* UMNESaveGame::MNESaveGame = nullptr;

FString UMNESaveGame::SaveSlotName = TEXT("NVIDIA_DLSS");
	
uint32 UMNESaveGame::UserIndex = 0;

UMNESaveGame::UMNESaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaToolkitSaveGame Constructor (%p)"), static_cast<void*>(this));
}

UMNESaveGame::~UMNESaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaToolkitSaveGame Destructor (%p)"), static_cast<void*>(this));

	if (MNESaveGame != nullptr)
	{
		MNESaveGame = nullptr;
	}
}

UMNESaveGame* UMNESaveGame::Get()
{
	if (MNESaveGame == nullptr)
	{
		check(IsInGameThread());
		LoadSaveGame();
	}
	
	return MNESaveGame;
}

void UMNESaveGame::LoadSaveGame()
{
	// 싱글톤에 저장된 데이터 불러오기
	MNESaveGame = Cast<UMNESaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if(MNESaveGame == nullptr)
	{
		// 불러오지 못했다면 새로운 파일을 생성
		MNESaveGame = GetMutableDefault<UMNESaveGame>(); // Gets the mutable default object of a class.
		if(MNESaveGame == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("LoadGameInstance == nullptr"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Create New MetaToolkit.sav"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load MetaToolkit save data Successed"));
	}
}

void UMNESaveGame::SaveGame()
{
	if (MNESaveGame != nullptr)
	{
		if (UGameplayStatics::SaveGameToSlot(MNESaveGame, SaveSlotName, UserIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Save Success"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Save Failed!!!"));
		}
	}
}