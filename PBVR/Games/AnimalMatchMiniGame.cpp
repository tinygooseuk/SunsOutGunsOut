// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "AnimalMatchMiniGame.h"

// Engine
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// Game
#include "Actors/Animal.h"
#include "Actors/Bomb.h"
#include "System/PBVRGameModeBase.h"

// Logic
AAnimalMatchMiniGame::AAnimalMatchMiniGame()
	: State(EAnimalMatchMiniGameState::AMMGS_ReRack)
	, Timer(0.0f)
	, TargetAnimal(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// Resources
	SM_AnimalMatchMachine = FIND_RESOURCE(StaticMesh, SM_AnimalMatchMachine, "StaticMeshes");
	SM_BigAssArrow = FIND_RESOURCE(StaticMesh, SM_BigAssArrow, "StaticMeshes");

	Sound_Bang = FIND_RESOURCE(SoundWave, Bang, "Sounds");
	Sound_Correct = FIND_RESOURCE(SoundWave, Correct, "Sounds");

	// Create the animal match machine
	MachineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MachineMesh"));
	MachineMesh->SetStaticMesh(SM_AnimalMatchMachine);
	SetRootComponent(MachineMesh);
	
	BigAssArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BigAssArrow0Mesh"));
	BigAssArrowMesh->SetRelativeLocation(FVector(100.0f, 160.0f, 860.0f));
	BigAssArrowMesh->SetStaticMesh(SM_BigAssArrow);
	BigAssArrowMesh->SetupAttachment(MachineMesh);
}

void AAnimalMatchMiniGame::BeginPlay()
{
	Super::BeginPlay();

	// Move animal match machine to middle of play space
	GOOSE_BAIL(ActionVolume.IsValid());

	FVector Origin, Size;
	ActionVolume->GetActorBounds(false, Origin, Size);

	SetActorLocation(Origin - FVector(0.0f, 0.0f, Size.Z + 100.0f));
}
void AAnimalMatchMiniGame::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch (State)
	{
		// Ready to re-rack animals
		case EAnimalMatchMiniGameState::AMMGS_ReRack:
			// Rack em up
			TearDownAnimals(); // Remove any existing
			SetupAnimals(); // Add new animals
			
			// Wait 1 sec to show animals
			Timer = 1.0f;
			State = EAnimalMatchMiniGameState::AMMGS_AwaitShowAnimals;
			break;

		// Waiting to show the main animals (target animal will now be showing)
		case EAnimalMatchMiniGameState::AMMGS_AwaitShowAnimals:
			Timer -= DeltaSeconds;

			// Timer elapsed; reveal all animals
			if (Timer <= 0.0f)
			{
				// Show all
				for (AAnimal* Animal : SpawnedAnimals)
				{
					Animal->SetPoppedUp(true);
				}

				// Await user's shot
				State = EAnimalMatchMiniGameState::AMMGS_AwaitShot;
			}
			break;

		// Waiting for a shot
		case EAnimalMatchMiniGameState::AMMGS_AwaitShot:
			// Waiting...
			break;

		// Shot confirmed, now awaiting re-racking
		case EAnimalMatchMiniGameState::AMMGS_AwaitReRack:
			Timer -= DeltaSeconds;

			// Finally re-rack
			if (Timer <= 0.0f)
			{
				State = EAnimalMatchMiniGameState::AMMGS_ReRack;
			}
			break;
	}	

}
void AAnimalMatchMiniGame::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	// Remove any animals at end of game
	TearDownAnimals();
}

void AAnimalMatchMiniGame::NotifyShotAnimal(EAnimal Animal, FVector Location)
{
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	APBVRGameModeBase* GameMode = Cast<APBVRGameModeBase>(World->GetAuthGameMode());
	GOOSE_BAIL(GameMode);

	// Work out if it was the *right* animal
	bool bCorrect = Animal == TargetAnimal->GetAnimal();
	if (bCorrect)
	{
		GameMode->AddScore(1);

		UGameplayStatics::PlaySoundAtLocation(this, Sound_Correct, Location, 1.0f, FMath::RandRange(0.8f, 1.2f));
	}
	else
	{
		GameMode->AddNegativeScore(1);

		ABomb::SpawnExplosion(this, Location);
		UGameplayStatics::PlaySoundAtLocation(this, Sound_Bang, Location, 1.0f, FMath::RandRange(0.8f, 1.2f));
	}

	// Pop all animals down
	for (AAnimal* SpawnedAnimal : SpawnedAnimals)
	{
		SpawnedAnimal->SetPoppedUp(false);
	}
	TargetAnimal->SetPoppedUp(false);	

	// Get ready to re-rack
	Timer = 1.2f; // 1.2s timer for reracking
	State = EAnimalMatchMiniGameState::AMMGS_AwaitReRack;
}

void AAnimalMatchMiniGame::SetupAnimals()
{
	// Spawn top target
	SpawnTargetAnimal();
	// Spawn the rest
	SpawnRackOfAnimals();	
}

void AAnimalMatchMiniGame::TearDownAnimals()
{
	// Remove all animals from rack
	for (AAnimal* Animal : SpawnedAnimals)
	{
		Animal->Destroy();
	}
	SpawnedAnimals.Empty();

	// Remove target animal from top
	if (TargetAnimal)
	{
		TargetAnimal->Destroy();
		TargetAnimal = nullptr;
	}
}

FName AAnimalMatchMiniGame::GetSocketNameForPosition(int32 X, int32 Y) const
{
	TCHAR* XName = TEXT("");
	TCHAR* YName = TEXT("");

	switch (X)
	{
		case 0: XName = TEXT("Left"); break;
		case 1: XName = TEXT("Centre"); break;
		case 2: XName = TEXT("Right"); break;
	}

	switch (Y)
	{
		case 0: YName = TEXT("Bottom"); break;
		case 1: YName = TEXT("Middle"); break;
		case 2: YName = TEXT("Top"); break;
	}

	// Format is yyy_xxx, i.e. "Top_Right"
	return FName(*FString::Printf(TEXT("%s_%s"), YName, XName)); // Safe to use temp string because FName strcpy()s :)
}
FName AAnimalMatchMiniGame::GetSocketNameForTarget() const
{
	// Hardcoded :)
	return FName("Target");
}


AAnimal* AAnimalMatchMiniGame::SpawnTargetAnimal()
{
	// Try to spawn some animales
	UWorld* World = GetWorld();
	GOOSE_BAIL_RETURN(World, nullptr);
		
	// Get the position
	FName SocketName = GetSocketNameForTarget();
	FTransform SpawnTransform = MachineMesh->GetSocketTransform(SocketName);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Work out which animal to give
	EAnimal RandomAnimal = (EAnimal)FMath::RandRange(0, EAnimal::A_MAX - 1);

	// Create and initialise
	AAnimal* NewAnimal = World->SpawnActor<AAnimal>(AAnimal::StaticClass(), SpawnTransform, Params);
	NewAnimal->Init(RandomAnimal, true);

	TargetAnimal = NewAnimal;

	return TargetAnimal;
}
void AAnimalMatchMiniGame::SpawnRackOfAnimals()
{
	// Try to spawn some animales
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	constexpr int32 GRID_WIDTH = 3;
	constexpr int32 GRID_HEIGHT = 3;

	TArray<EAnimal> AnimalsToUse;
	AnimalsToUse.Reserve(GRID_WIDTH * GRID_HEIGHT);

	// Create 8 randoms (not the target animal)
	for (int32 I = 0; I < (GRID_WIDTH * GRID_HEIGHT) - 1; I++)
	{
		EAnimal RandomAnimal = TargetAnimal->GetAnimal();
		while (RandomAnimal == TargetAnimal->GetAnimal())
		{
			RandomAnimal = (EAnimal)FMath::RandRange(0, EAnimal::A_MAX - 1);
		}	
		AnimalsToUse.Add(RandomAnimal);
	}
	// Insert the target one
	AnimalsToUse.Insert(TargetAnimal->GetAnimal(), FMath::RandRange(0, AnimalsToUse.Num() - 1));

	// Lay out into rack
	for (int32 X = 0; X < GRID_WIDTH; X++)
	{
		for (int32 Y = 0; Y < GRID_HEIGHT; Y++)
		{
			// Get this socket position
			FName SocketName = GetSocketNameForPosition(X, Y);
			FTransform SpawnTransform = MachineMesh->GetSocketTransform(SocketName);

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// Grab animal to use
			EAnimal RandomAnimal = AnimalsToUse.Pop();

			// Create and initialise
			AAnimal* NewAnimal = World->SpawnActor<AAnimal>(AAnimal::StaticClass(), SpawnTransform, Params);
			NewAnimal->Init(RandomAnimal);

			SpawnedAnimals.Add(NewAnimal);
		}
	}
}

// Overrides
int32 AAnimalMatchMiniGame::GetStartingAmmo() const
{
	return 20;
}
float AAnimalMatchMiniGame::GetStartingTime() const
{
	return 45.0f;
}
FText AAnimalMatchMiniGame::GetTargetName() const
{
	return LOCTEXT("MATCHING_ANIMALS", "Matching Animals");
}
int32 AAnimalMatchMiniGame::GetTargetScore() const
{
	return 10;
}