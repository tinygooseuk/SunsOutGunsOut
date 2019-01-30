// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "TimerClock.h"

// Engine
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/Material.h"

// Game
#include "System/PBVRGameModeBase.h"
#include "System/PBVRGameStateBase.h"

ATimerClock::ATimerClock()
{
	PrimaryActorTick.bCanEverTick = true;

	// Resources
	UStaticMesh* Engine_Cylinder = FIND_ENGINE_RESOURCE(StaticMesh, Cylinder, "BasicShapes");
	UStaticMesh* Engine_Plane = FIND_ENGINE_RESOURCE(StaticMesh, Plane, "BasicShapes");
	
	UMaterialInterface* M_ClockFace = FIND_RESOURCE(Material, M_Clock, "Materials");
	UMaterialInterface* M_GunMetal = FIND_RESOURCE(Material, M_GunMetal, "Materials");

	FaceMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FaceMesh"));
	FaceMeshComponent->SetWorldScale3D(FVector(1.5f, 1.5f, 1.0f));
	FaceMeshComponent->SetStaticMesh(Engine_Plane);
	FaceMeshComponent->SetMaterial(0, M_ClockFace);
	SetRootComponent(FaceMeshComponent);

	HandOffsetComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandOffset"));
	HandOffsetComponent->SetupAttachment(FaceMeshComponent);

	HandMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandMesh"));
	HandMeshComponent->SetRelativeLocation(FVector(0.0f, 15.0f, 0.0f));
	HandMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	HandMeshComponent->SetWorldScale3D(FVector(0.03f, 0.03f, 0.5f));
	HandMeshComponent->SetStaticMesh(Engine_Cylinder);
	HandMeshComponent->SetMaterial(0, M_GunMetal);
	HandMeshComponent->SetupAttachment(HandOffsetComponent);
}

void ATimerClock::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATimerClock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	APBVRGameModeBase* GameMode = Cast<APBVRGameModeBase>(World->GetAuthGameMode());
	GOOSE_BAIL(GameMode);

	APBVRGameStateBase* GameState = GameMode->GetPBVRGameState();
	GOOSE_BAIL(GameState);

	// Get timer value, apply hand rotation
	float TimeLeftSecs = GameState->TimerSeconds / 60.0f;
	FRotator HandRotation = FRotator(0.0f, TimeLeftSecs * 360.0f, 0.0f);

	HandOffsetComponent->SetRelativeRotation(HandRotation);
}

