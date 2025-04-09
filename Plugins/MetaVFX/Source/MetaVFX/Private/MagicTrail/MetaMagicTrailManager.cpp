

#include "MagicTrail/MetaMagicTrailManager.h"

#include "FileMediaSource.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MemoryPoolObject.h"
#include "MagicTrail/MetaMagicTrailWidget.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"

UMetaMagicTrailManager::UMetaMagicTrailManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> MetaMagicTrailWidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/MetaVFX/MagicTrail/UI/WBP_MetaMagicTrail.WBP_MetaMagicTrail_C'"));
	if (MetaMagicTrailWidgetFinder.Succeeded())
	{
		MagicTrailWidgetClass = MetaMagicTrailWidgetFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<ANiagaraActor> MagicTrailNiagaraActorFinder(TEXT("/Script/Engine.Blueprint'/MetaVFX/MagicTrail/VFX/MagicTrail/BP_MagicTrailNiagaraActor.BP_MagicTrailNiagaraActor_C'"));
	if (MagicTrailNiagaraActorFinder.Succeeded())
	{
		MagicTrailNiagaraActorClass = MagicTrailNiagaraActorFinder.Class;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> SpriteMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Sprite.M_Sprite'"));
	if (SpriteMaterialFinder.Succeeded())
	{
		DefaultSpriteMaterial = SpriteMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> TrailMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Trail.M_Trail'"));
	if (TrailMaterialFinder.Succeeded())
	{
		DefaultTrailMaterial = TrailMaterialFinder.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> CircleTrailMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Trail_Circle.M_Trail_Circle'"));
	if (CircleTrailMaterialFinder.Succeeded())
	{
		CircleTrailMaterial = CircleTrailMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> SquareTrailMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Trail_Square.M_Trail_Square'"));
	if (SquareTrailMaterialFinder.Succeeded())
	{
		SquareTrailMaterial = SquareTrailMaterialFinder.Object;
	}
}

void UMetaMagicTrailManager::BeginPlay()
{
	Super::BeginPlay();

	if (!bUseFile) AssetPath.FilePath = TEXT(""); 

	PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This component can only be used as a component of a player controller."));
		return;
	}

	if (MagicTrailWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MagicTrailWidgetClass is nullptr"));
		return;
	}

	if (bUseEditUI)
	{
		MagicTrailWidget = CreateWidget<UMetaMagicTrailWidget>(PlayerController, MagicTrailWidgetClass);
		MagicTrailWidget->AddToViewport();
		MagicTrailWidget->HideWidget();
	}

	MediaPlayer = NewObject<UMediaPlayer>(PlayerController, UMediaPlayer::StaticClass(), TEXT("TrailMediaPlayer"));
	MediaTexture = NewObject<UMediaTexture>(PlayerController, UMediaTexture::StaticClass(), TEXT("TrailMediaTexture"));
	MediaPlayer->PlayOnOpen = true;
	MediaPlayer->SetLooping(true);
	MediaTexture->SetMediaPlayer(MediaPlayer);
	MediaTexture->UpdateResource();
	
	if (bVideo)
	{
		MediaPlayer->OpenSource(FileMediaSource);
		TextureFromAsset = MediaTexture;
	}
	else
	{
		TextureFromAsset = Texture2D;
	}
	
	ChangeMaskShape(MaskShape);
	SpriteMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DefaultSpriteMaterial, nullptr);
	SpriteMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), bUseFile ? TextureFromFile : TextureFromAsset);
	SetFilePath(FPaths::ConvertRelativePathToFull(AssetPath.FilePath));
	
	MemoryPoolObject = NewObject<UMemoryPoolObject>(PlayerController, TEXT("Memory Pool"));
	MemoryPoolObject->SetActorClass(MagicTrailNiagaraActorClass);
	MemoryPoolObject->CreateActors(InitCreatePoolNum);
	
	if (ViewportSize.X <= 0|| ViewportSize.Y <= 0)
	{
		PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);

		if (MagicTrailWidget)
		{
			MagicTrailWidget->SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
			MagicTrailWidget->MetaCursorResizing();
		}
	}

	if (bUseMouse)
	{
		// 액터의 InputComponent를 가져옵니다.
		if (UInputComponent* InputComp = PlayerController->FindComponentByClass<UInputComponent>())
		{
			InputComp->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &UMetaMagicTrailManager::OnLeftMouseDown);
			InputComp->BindKey(EKeys::LeftMouseButton, IE_Released, this, &UMetaMagicTrailManager::OnLeftMouseUp);
		}
	}

	FViewport::ViewportResizedEvent.AddUObject(this, &UMetaMagicTrailManager::OnViewportResized);
}

void UMetaMagicTrailManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	MediaPlayer = nullptr;
	MediaTexture = nullptr;
	
	if (MouseDelayHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
		MouseDelayHandle.Invalidate();
	}

	for (auto& LiDARActor : LiDARActors)
	{
		if (LiDARActor.Value.DelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(LiDARActor.Value.DelayHandle);
			LiDARActor.Value.DelayHandle.Invalidate();
		}
	}	

	FViewport::ViewportResizedEvent.Clear();
}

void UMetaMagicTrailManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentTime += DeltaTime;
	if (CurrentTime > ResetTime)
	{
		CurrentTime = 0;
		DeallocateAllMagicTrails();
	}
	
	if (bIsDragging) LeftMouseDrag();
}

void UMetaMagicTrailManager::NewMagicTrailWithLiDAR(const int32 ID, const FVector2D Percentage)
{
	if (!LiDARActors.Contains(ID))
	{
		FLiDARActor LiDARActor;
		LiDARActor.Actor = MemoryPoolObject->AllocateActor().Actor;
		LiDARActor.NiagaraComponent = Cast<ANiagaraActor>(LiDARActor.Actor)->GetNiagaraComponent();
		InitNiagaraComponent(LiDARActor.NiagaraComponent);

		LiDARActors.Emplace(ID, LiDARActor);
	}
	
	SetLiDARActorLocationFromScreenPercentage(ID, Percentage.X, Percentage.Y);
}

void UMetaMagicTrailManager::UpdateMagicTrailWithLiDAR(const int32 ID, const FVector2D Percentage)
{
	if (!LiDARActors.Contains(ID))
	{
		FLiDARActor LiDARActor;
		LiDARActor.Actor = MemoryPoolObject->AllocateActor().Actor;
		LiDARActor.NiagaraComponent = Cast<ANiagaraActor>(LiDARActor.Actor)->GetNiagaraComponent();
		InitNiagaraComponent(LiDARActor.NiagaraComponent);

		LiDARActors.Emplace(ID, LiDARActor);
	}
	
	SetLiDARActorLocationFromScreenPercentage(ID, Percentage.X, Percentage.Y);
}

void UMetaMagicTrailManager::RemoveMagicTrailWithLiDAR(const int32 ID)
{
	if (LiDARActors.Contains(ID))
	{
		if (LiDARActors[ID].DelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(LiDARActors[ID].DelayHandle);
			LiDARActors[ID].DelayHandle.Invalidate();
		}
		
		LiDARActors[ID].NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(LiDARActors[ID].Actor, false);

		LiDARActors[ID].Actor = nullptr;
		LiDARActors[ID].NiagaraComponent = nullptr;

		LiDARActors.Remove(ID);
	}
}

void UMetaMagicTrailManager::DeallocateAllMagicTrails()
{
	if (MouseActor)
	{
		if (MouseDelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
			MouseDelayHandle.Invalidate();
		}
		
		MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(MouseActor, false);

		MouseActor = nullptr;
		MouseNiagaraComponent = nullptr;
	}

	for (auto& LiDARActor : LiDARActors)
	{
		if (LiDARActor.Value.DelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(LiDARActor.Value.DelayHandle);
			LiDARActor.Value.DelayHandle.Invalidate();
		}
		
		LiDARActor.Value.NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(LiDARActor.Value.Actor, false);

		LiDARActor.Value.Actor = nullptr;
		LiDARActor.Value.NiagaraComponent = nullptr;
	}
	
	LiDARActors.Empty();
}

void UMetaMagicTrailManager::ShowWidget()
{
	if (MagicTrailWidget)
	{
		MagicTrailWidget->ShowWidget();
	}
}

void UMetaMagicTrailManager::HideWidget()
{
	if (MagicTrailWidget)
	{
		MagicTrailWidget->HideWidget();
	}
}

void UMetaMagicTrailManager::ChangeMaskShape(const EMaskShape Shape)
{
	if (TrailMaterialInstanceDynamic && MaskShape == Shape) return;

	MaskShape = Shape;
	switch (MaskShape)
	{
	case EMaskShape::Default:
		TrailMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DefaultTrailMaterial, nullptr);
		break;
	case EMaskShape::Circle:
		TrailMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(CircleTrailMaterial, nullptr);
		break;
	case EMaskShape::Square:
		TrailMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(SquareTrailMaterial, nullptr);
		break;
	}
	
	TrailMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), bUseFile ? TextureFromFile : TextureFromAsset);
}

bool UMetaMagicTrailManager::SetFilePath(const FString& NewFilePath)
{
	const FString FileExtensionWithDot  = FPaths::GetExtension(NewFilePath, true);
	
	// video
	if (FileExtensionWithDot.Equals(TEXT(".mp4"), ESearchCase::IgnoreCase))
	{
		const FString URL = TEXT("file://") + NewFilePath;
		if (MediaPlayer->OpenUrl(URL))
		{
			TextureFromFile = MediaTexture;
			AssetPath.FilePath = NewFilePath;

			TrailMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), bUseFile ? TextureFromFile : TextureFromAsset);
			SpriteMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), bUseFile ? TextureFromFile : TextureFromAsset);

			return true;
		}
	}

	// image
	else
	{
		if (UTexture2D* LoadedTexture = UKismetRenderingLibrary::ImportFileAsTexture2D(GetWorld(), NewFilePath))
		{
			TextureFromFile = LoadedTexture;
			AssetPath.FilePath = NewFilePath;

			TrailMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), bUseFile ? TextureFromFile : TextureFromAsset);
			SpriteMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), bUseFile ? TextureFromFile : TextureFromAsset);

			return true;
		}	
	}

	TextureFromFile = nullptr;
	AssetPath.FilePath = TEXT("");
	if (bVideo)
	{
		MediaPlayer->OpenSource(FileMediaSource);
	}
	TrailMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), TextureFromAsset);
	SpriteMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), TextureFromAsset);
	return false;
}

void UMetaMagicTrailManager::ClearFilePath()
{
	TextureFromFile = nullptr;
	AssetPath.FilePath = TEXT("");
	TrailMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), TextureFromAsset);
	SpriteMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), TextureFromAsset);
}

void UMetaMagicTrailManager::OnViewportResized(FViewport* Viewport, unsigned int I)
{
	ViewportSize = Viewport->GetSizeXY();

	if (MagicTrailWidget)
	{
		MagicTrailWidget->SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
		MagicTrailWidget->MetaCursorResizing();
	}
}

void UMetaMagicTrailManager::OnLeftMouseDown()
{
	bIsDragging = true;
	if (MouseActor == nullptr)
	{
		MouseActor = MemoryPoolObject->AllocateActor().Actor;
		MouseNiagaraComponent = Cast<ANiagaraActor>(MouseActor)->GetNiagaraComponent();
		InitNiagaraComponent(MouseNiagaraComponent);

		SetMouseActorLocation();
	}
}

void UMetaMagicTrailManager::OnLeftMouseUp()
{
	bIsDragging = false;
	if (MouseActor)
	{
		if (MouseDelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
			MouseDelayHandle.Invalidate();
		}
		
		MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(MouseActor, false);
	
		MouseActor = nullptr;
		MouseNiagaraComponent = nullptr;
	}
}

void UMetaMagicTrailManager::LeftMouseDrag()
{
	if (MouseActor == nullptr)
	{
		MouseActor = MemoryPoolObject->AllocateActor().Actor;
		MouseNiagaraComponent = Cast<ANiagaraActor>(MouseActor)->GetNiagaraComponent();
		InitNiagaraComponent(MouseNiagaraComponent);
	}
	
	SetMouseActorLocation();
}

void UMetaMagicTrailManager::InitNiagaraComponent(UNiagaraComponent* NiagaraComponent)
{
	NiagaraComponent->SetVariableMaterial(TEXT("MI_Sprite"), SpriteMaterialInstanceDynamic);
	NiagaraComponent->SetVariableMaterial(TEXT("MI_Trail"), TrailMaterialInstanceDynamic);
	NiagaraComponent->SetVariableFloat(TEXT("LifeTime"), LifeTime);
	NiagaraComponent->SetVariableFloat(TEXT("Scale"), Scale);
	NiagaraComponent->SetVariableFloat(TEXT("SpriteScale"), SpriteScale);
	NiagaraComponent->SetVariableFloat(TEXT("SpriteRateScale"), SpriteRateScale);
	NiagaraComponent->SetVariableFloat(TEXT("EdgeThickness"), EdgeThickness);
	NiagaraComponent->SetVariableFloat(TEXT("EdgeIntensity"), EdgeIntensity);
	NiagaraComponent->SetVariableLinearColor(TEXT("EdgeColor"), EdgeColor);
	NiagaraComponent->SetVariableLinearColor(TEXT("SpriteColor"), SpriteColor);
	NiagaraComponent->SetVariableBool(TEXT("bAutoColor"), bAutoColor);
}

void UMetaMagicTrailManager::SetMouseActorLocation()
{
	if (bUseRay)
	{
		FHitResult HitResult;
		if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
		{
			MouseActor->SetActorLocation(HitResult.Location);
			if (!MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(MouseDelayHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
				}), ParticleActivationThresholdSec, false);		
			}
		}
		else
		{
			if (MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
				MouseDelayHandle.Invalidate();

				MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
			}
		}
	}
	else
	{
		FVector WorldPos = FVector::Zero();
		FVector WorldDir = FVector::Zero();
		if (PlayerController->DeprojectMousePositionToWorld(WorldPos, WorldDir))
		{
			MouseActor->SetActorLocation(WorldPos);
			if (!MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(MouseDelayHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
				}), ParticleActivationThresholdSec, false);		
			}
		}
	}
}

void UMetaMagicTrailManager::SetLiDARActorLocationFromScreenPercentage(const int32 ID, const float X, const float Y)
{
	FLiDARActor* LiDARActor = LiDARActors.Find(ID);
	const FVector2D ScreenPosition = FVector2D(ViewportSize.X * X, ViewportSize.Y * Y);

	if(LiDARActor)
	{
		if (bUseRay)
		{
			FHitResult HitResult;
			if (PlayerController->GetHitResultAtScreenPosition(ScreenPosition, ECC_Visibility, false, HitResult))
			{
				LiDARActor->Actor->SetActorLocation(HitResult.Location);
				if (!LiDARActor->DelayHandle.IsValid())
				{
					GetWorld()->GetTimerManager().SetTimer(LiDARActor->DelayHandle, FTimerDelegate::CreateLambda([this, ID]()->void
					{
						if (const FLiDARActor* LiDARActor = LiDARActors.Find(ID))
						{
							LiDARActor->NiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
						}
					}), ParticleActivationThresholdSec, false);		
				}
			}
			else
			{
				if (LiDARActor->DelayHandle.IsValid())
				{
					GetWorld()->GetTimerManager().ClearTimer(LiDARActor->DelayHandle);
					LiDARActor->DelayHandle.Invalidate();

					LiDARActor->NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
				}
			}
		}
		else
		{
			FVector WorldPos = FVector::Zero();
			FVector WorldDir = FVector::Zero();
			if (UGameplayStatics::DeprojectScreenToWorld(PlayerController, ScreenPosition, WorldPos, WorldDir))
			{
				LiDARActor->Actor->SetActorLocation(WorldPos);
				if (!LiDARActor->DelayHandle.IsValid())
				{
					GetWorld()->GetTimerManager().SetTimer(LiDARActor->DelayHandle, FTimerDelegate::CreateLambda([this, ID]()->void
					{
						if (const FLiDARActor* LiDARActor = LiDARActors.Find(ID))
						{
							LiDARActor->NiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
						}
					}), ParticleActivationThresholdSec, false);		
				}
			}
		}	
	}
}

bool UMetaMagicTrailManager::IsShowWidget()
{
	if (MagicTrailWidget == nullptr) return false;

	return MagicTrailWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible;
}
