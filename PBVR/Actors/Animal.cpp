// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "Animal.h"

// Engine
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"

// Plugin
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"

// Game
#include "Games/AnimalMatchMiniGame.h"

AAnimal::AAnimal()
	: bIsTarget(false)
	, Animal(EAnimal::A_Pig)
	, bPoppedUp(false)
	, bShattered(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// Resources
	SM_Lion = FIND_RESOURCE(StaticMesh, SM_Lion, "StaticMeshes/Animals");
	SM_Elephant = FIND_RESOURCE(StaticMesh, SM_Elephant, "StaticMeshes/Animals");
	SM_Pig = FIND_RESOURCE(StaticMesh, SM_Pig, "StaticMeshes/Animals");
	SM_Panda = FIND_RESOURCE(StaticMesh, SM_Panda, "StaticMeshes/Animals");

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	AnimalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnimalMesh"));
	AnimalMesh->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	AnimalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -200.0f));
	AnimalMesh->SetupAttachment(SceneRoot);
}

void AAnimal::Init(EAnimal InAnimal, bool bInIsTarget)
{
	Animal = InAnimal;
	bIsTarget = bInIsTarget;
	bPoppedUp = bInIsTarget;

	UStaticMesh* Mesh = GetStaticMeshForAnimal(Animal);
	AnimalMesh->SetStaticMesh(Mesh);	
}

void AAnimal::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShattered)
	{
		return;
	}

	// Move if needed
	FVector Location = AnimalMesh->GetRelativeTransform().GetLocation();

	bool bLocationDirty = false;

	float TargetX = bPoppedUp ? 0.0f : +100.0;
	float TargetZ = bPoppedUp ? 0.0f : -200.0f;

	if (!FMath::IsNearlyEqual(Location.X, TargetX, 0.1f))
	{
		Location.X = FMath::Lerp(Location.X, TargetX, 0.6f);
		bLocationDirty = true;
	}

	if (!FMath::IsNearlyEqual(Location.Z, TargetZ, 0.1f))
	{
		Location.Z = FMath::Lerp(Location.Z, TargetZ, 0.6f);
		bLocationDirty = true;
	}

	if (bLocationDirty)
	{
		// Apply new location according to popped-up state
		AnimalMesh->SetRelativeLocation(Location);
	}
}

float AAnimal::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// Shot whilst visible and not the target (i.e. the top one in the window)
	if (!bIsTarget && bPoppedUp)
	{
		// Shatter the animal
		Shatter();

		TActorIterator<AAnimalMatchMiniGame> It(GetWorld());
	
		if (GOOSE_VERIFY(It))
		{
			// Tell the minigame about the hit
			AAnimalMatchMiniGame* MatchGame = *It;
			MatchGame->NotifyShotAnimal(Animal, GetActorLocation());
		}
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AAnimal::Shatter()
{
	if (bShattered)
	{
		return;
	}

	bShattered = true;

	// Remove static mesh, add destructible
	FVector Location = AnimalMesh->GetComponentLocation();
	FRotator Rotator = AnimalMesh->GetComponentRotation();
	FVector Scale = AnimalMesh->GetComponentScale();
	FString AnimalDMName = FString::Printf(TEXT("%s_DM"), *AnimalMesh->GetStaticMesh()->GetName());
	AnimalMesh->SetWorldLocation(FVector(0.0f, 0.0f, 2000.0f));
	AnimalMesh->DestroyComponent();

	FAttachmentTransformRules Attachment = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);

	UDestructibleMesh* DestructibleMesh_DM = UGooseUtil::GetObject<UDestructibleMesh>(*AnimalDMName, TEXT("StaticMeshes/Animals"));

	UDestructibleComponent* Destructible = NewObject<UDestructibleComponent>(this, TEXT("Destructible"));
	Destructible->SetWorldLocationAndRotation(Location, Rotator);
	Destructible->SetDestructibleMesh(DestructibleMesh_DM);
	Destructible->SetWorldScale3D(Scale);
	Destructible->RegisterComponent();
	//Destructible->AttachToComponent(GetRootComponent(), Attachment);
	SetRootComponent(Destructible);

	Destructible->SetSimulatePhysics(true);
	Destructible->ApplyRadiusDamage(1.0f, Location, 10.0f, 100.0f, true);
//	Destructible->AddRadialImpulse(Location, 10.0f, 100.0f, ERadialImpulseFalloff::RIF_Linear, true);
}

void AAnimal::SetPoppedUp(bool bInPopup)
{
	bPoppedUp = bInPopup;
}

EAnimal AAnimal::GetAnimal() const
{
	return Animal;
}

UStaticMesh* AAnimal::GetStaticMeshForAnimal(EAnimal Animal) const
{
	switch (Animal)
	{
		case EAnimal::A_Lion:		return SM_Lion;
		case EAnimal::A_Elephant:	return SM_Elephant;
		case EAnimal::A_Pig:		return SM_Pig;
		case EAnimal::A_Panda:		return SM_Panda;
	}

	// Pig?
	return SM_Pig;
}

