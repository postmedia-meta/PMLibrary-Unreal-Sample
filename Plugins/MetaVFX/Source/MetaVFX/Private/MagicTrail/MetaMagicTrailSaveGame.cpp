// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicTrail/MetaMagicTrailSaveGame.h"
#include "Kismet/GameplayStatics.h"

UMetaMagicTrailSaveGame* UMetaMagicTrailSaveGame::MetaMagicTrailSaveGame = nullptr;

FString UMetaMagicTrailSaveGame::SaveSlotName = TEXT("MetaMagicTrail");
	
uint32 UMetaMagicTrailSaveGame::UserIndex = 0;

UMetaMagicTrailSaveGame::UMetaMagicTrailSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaMagicTrailSaveGame Constructor (%p)"), static_cast<void*>(this));
}

UMetaMagicTrailSaveGame::~UMetaMagicTrailSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaMagicTrailSaveGame Destructor (%p)"), static_cast<void*>(this));

	if (MetaMagicTrailSaveGame != nullptr)
	{
		MetaMagicTrailSaveGame = nullptr;
	}
}

UMetaMagicTrailSaveGame* UMetaMagicTrailSaveGame::Get()
{
	if (MetaMagicTrailSaveGame == nullptr)
	{
		check(IsInGameThread());
		LoadSaveGame();
	}
	
	return MetaMagicTrailSaveGame;
}

void UMetaMagicTrailSaveGame::LoadSaveGame()
{
	// 싱글톤에 저장된 데이터 불러오기
	MetaMagicTrailSaveGame = Cast<UMetaMagicTrailSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if(MetaMagicTrailSaveGame == nullptr)
	{
		// 불러오지 못했다면 새로운 파일을 생성
		MetaMagicTrailSaveGame = GetMutableDefault<UMetaMagicTrailSaveGame>(); // Gets the mutable default object of a class.
		if(MetaMagicTrailSaveGame == nullptr)
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

void UMetaMagicTrailSaveGame::SaveGame()
{
	if (MetaMagicTrailSaveGame != nullptr)
	{
		if (UGameplayStatics::SaveGameToSlot(MetaMagicTrailSaveGame, SaveSlotName, UserIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Save Success"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Save Failed!!!"));
		}
	}
}
