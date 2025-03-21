// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MTKPlayerController.h"

#include "Components/MetaGraphicsSettingsComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AMTKPlayerController::AMTKPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	MetaGraphicsSettingsComponent = CreateDefaultSubobject<UMetaGraphicsSettingsComponent>(TEXT("Meta Graphics Settings Component"));
}

void AMTKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AMTKPlayerController::Escape);
}

void AMTKPlayerController::Escape()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Type::Quit, false);
}
