// Fill out your copyright notice in the Description page of Project Settings.


#include "FluidArt/MetaFluidArtSaveGame.h"

#include "Kismet/GameplayStatics.h"

UMetaFluidArtSaveGame* UMetaFluidArtSaveGame::MetaFluidArtSaveGame = nullptr;

FString UMetaFluidArtSaveGame::SaveSlotName = TEXT("MetaFluidArt");
	
uint32 UMetaFluidArtSaveGame::UserIndex = 0;

UMetaFluidArtSaveGame::UMetaFluidArtSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaFluidArtSaveGame Constructor (%p)"), static_cast<void*>(this));
}

UMetaFluidArtSaveGame::~UMetaFluidArtSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaFluidArtSaveGame Destructor (%p)"), static_cast<void*>(this));

	if (MetaFluidArtSaveGame != nullptr)
	{
		MetaFluidArtSaveGame = nullptr;
	}
}

UMetaFluidArtSaveGame* UMetaFluidArtSaveGame::Get()
{
	if (MetaFluidArtSaveGame == nullptr)
	{
		check(IsInGameThread());
		LoadSaveGame();
	}
	
	return MetaFluidArtSaveGame;
}

void UMetaFluidArtSaveGame::LoadSaveGame()
{
	// 싱글톤에 저장된 데이터 불러오기
	MetaFluidArtSaveGame = Cast<UMetaFluidArtSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if(MetaFluidArtSaveGame == nullptr)
	{
		// 불러오지 못했다면 새로운 파일을 생성
		MetaFluidArtSaveGame = GetMutableDefault<UMetaFluidArtSaveGame>(); // Gets the mutable default object of a class.
		if(MetaFluidArtSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("LoadGameInstance == nullptr"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Create New MetaFluidArt.sav"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load MetaToolkit save data Successed"));
	}
}

void UMetaFluidArtSaveGame::SaveGame()
{
	if (MetaFluidArtSaveGame != nullptr)
	{
		if (UGameplayStatics::SaveGameToSlot(MetaFluidArtSaveGame, SaveSlotName, UserIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Save Success"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Save Failed!!!"));
		}
	}
}
