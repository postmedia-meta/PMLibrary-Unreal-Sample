// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaToolkitSaveGame.h"

#include "Kismet/GameplayStatics.h"

UMetaToolkitSaveGame* UMetaToolkitSaveGame::MetaToolkitSaveGame = nullptr;

FString UMetaToolkitSaveGame::SaveSlotName = TEXT("MetaToolkit");
	
uint32 UMetaToolkitSaveGame::UserIndex = 0;

UMetaToolkitSaveGame::UMetaToolkitSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaToolkitSaveGame Constructor (%p)"), static_cast<void*>(this));
}

UMetaToolkitSaveGame::~UMetaToolkitSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaToolkitSaveGame Destructor (%p)"), static_cast<void*>(this));

	if (MetaToolkitSaveGame != nullptr)
	{
		MetaToolkitSaveGame = nullptr;
	}
}

UMetaToolkitSaveGame* UMetaToolkitSaveGame::Get()
{
	if (MetaToolkitSaveGame == nullptr)
	{
		check(IsInGameThread());
		LoadSaveGame();
	}
	
	return MetaToolkitSaveGame;
}

void UMetaToolkitSaveGame::LoadSaveGame()
{
	// 싱글톤에 저장된 데이터 불러오기
	MetaToolkitSaveGame = Cast<UMetaToolkitSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if(MetaToolkitSaveGame == nullptr)
	{
		// 불러오지 못했다면 새로운 파일을 생성
		MetaToolkitSaveGame = GetMutableDefault<UMetaToolkitSaveGame>(); // Gets the mutable default object of a class.
		if(MetaToolkitSaveGame == nullptr)
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
		UE_LOG(LogTemp, Warning, TEXT("Load MetaToolkit save data Successed: %s"), *MetaToolkitSaveGame->GraphicsSettings.Resolution.ToString());
	}
}

void UMetaToolkitSaveGame::SaveGame()
{
	if (MetaToolkitSaveGame != nullptr)
	{
		if (UGameplayStatics::SaveGameToSlot(MetaToolkitSaveGame, SaveSlotName, UserIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Save Success"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Save Failed!!!"));
		}
	}
}