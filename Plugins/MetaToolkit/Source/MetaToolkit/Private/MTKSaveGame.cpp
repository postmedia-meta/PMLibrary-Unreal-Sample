// Fill out your copyright notice in the Description page of Project Settings.


#include "MTKSaveGame.h"
#include "Kismet/GameplayStatics.h"

UMTKSaveGame* UMTKSaveGame::MTKSaveGame = nullptr;

FString UMTKSaveGame::SaveSlotName = TEXT("MetaToolkit");
	
uint32 UMTKSaveGame::UserIndex = 0;

UMTKSaveGame::UMTKSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaToolkitSaveGame Constructor (%p)"), static_cast<void*>(this));
}

UMTKSaveGame::~UMTKSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("UMetaToolkitSaveGame Destructor (%p)"), static_cast<void*>(this));

	if (MTKSaveGame != nullptr)
	{
		MTKSaveGame = nullptr;
	}
}

UMTKSaveGame* UMTKSaveGame::Get()
{
	if (MTKSaveGame == nullptr)
	{
		check(IsInGameThread());
		LoadSaveGame();
	}
	
	return MTKSaveGame;
}

void UMTKSaveGame::LoadSaveGame()
{
	// 싱글톤에 저장된 데이터 불러오기
	MTKSaveGame = Cast<UMTKSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if(MTKSaveGame == nullptr)
	{
		// 불러오지 못했다면 새로운 파일을 생성
		MTKSaveGame = GetMutableDefault<UMTKSaveGame>(); // Gets the mutable default object of a class.
		if(MTKSaveGame == nullptr)
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
		UE_LOG(LogTemp, Warning, TEXT("Load MetaToolkit save data Successed: %s"), *MTKSaveGame->GraphicsSettings.Resolution.ToString());
	}
}

void UMTKSaveGame::SaveGame()
{
	if (MTKSaveGame != nullptr)
	{
		if (UGameplayStatics::SaveGameToSlot(MTKSaveGame, SaveSlotName, UserIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Save Success"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Save Failed!!!"));
		}
	}
}