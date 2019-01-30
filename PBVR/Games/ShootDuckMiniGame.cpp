// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "ShootDuckMiniGame.h"

// Engine
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// Game
#include "Actors/Bomb.h"
#include "Actors/Duck.h"
#include "Kismet/GameplayStatics.h"
#include "System/PBVRGameModeBase.h"
#include "System/PBVRGameStateBase.h"

namespace
{
	template <typename TActor>
	TActor* SpawnFlungObject(UWorld* World, const TWeakObjectPtr<AActionVolume>& ActionVolume)
	{
		GOOSE_BAIL_RETURN(ActionVolume.IsValid(), nullptr);
		GOOSE_BAIL_RETURN(World, nullptr);

		// Get size of action volume
		FVector Centre, Extents;
		ActionVolume->GetActorBounds(false, Centre, Extents);

		// Pick random point on left or right plane
		bool bOnLeft = FMath::RandBool();

		float XCoord = FMath::RandRange(-Extents.X, +Extents.X);
		float YCoord = bOnLeft ? -Extents.Y : +Extents.Y;
		float ZCoord = FMath::RandRange(0.0f, +Extents.Z);

		// Spawn!
		FVector SpawnLocation = Centre + FVector(XCoord, YCoord, ZCoord);

		TActor* Object = World->SpawnActor<TActor>(SpawnLocation, FRotator::ZeroRotator);
		if (Object)
		{
			UPrimitiveComponent* PrimitiveComp = Object->FindComponentByClass<UPrimitiveComponent>();
			if (GOOSE_VERIFY(PrimitiveComp))
			{
				// Apply physics!
				FVector AngularImpulse = FVector(FMath::RandRange(-900.0f, +900.0f), FMath::RandRange(-900.0f, +900.0f), FMath::RandRange(-900.0f, +900.0f));
				PrimitiveComp->AddAngularImpulseInDegrees(AngularImpulse, NAME_None, true);

				constexpr float FORCE = 18'00.0f;

				PrimitiveComp->AddImpulse(
					FVector(
						FMath::RandRange(-FORCE * 0.01f, +FORCE * 0.01f),
						FMath::RandRange(+FORCE * 0.7f, +FORCE * 0.8f) * (bOnLeft ? 1.0f : -1.0f),
						FMath::RandRange(+FORCE * 0.3f, +FORCE * 0.7f)
					), NAME_None, true
				);
			}
		}

		return Object;
	}
}

// Logic
AShootDuckMiniGame::AShootDuckMiniGame()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShootDuckMiniGame::BeginPlay()
{
	Super::BeginPlay();

	// Set timer
	DuckSpawnTimer = 0.0f;
}
void AShootDuckMiniGame::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DuckSpawnTimer -= DeltaSeconds;

	// Time to spawn a duck or bomb
	if (DuckSpawnTimer <= 0.0f)
	{
		bool bSpawnBomb = FMath::RandRange(0.0f, 1.0f) > 0.6f; // 40% chance of bomb

		if (bSpawnBomb) 
		{
			SpawnBomb();
		}
		else
		{
			SpawnDuck();
		}

		// Spawn another in 0.1..1.0 secs
		DuckSpawnTimer = FMath::RandRange(0.1f, 1.0f);
	}
}
void AShootDuckMiniGame::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

}

ADuck* AShootDuckMiniGame::SpawnDuck()
{
	return SpawnFlungObject<ADuck>(GetWorld(), ActionVolume);
}
ABomb* AShootDuckMiniGame::SpawnBomb()
{
	return SpawnFlungObject<ABomb>(GetWorld(), ActionVolume);
}

// Overrides
int32 AShootDuckMiniGame::GetStartingAmmo() const
{
	return 100;
}
float AShootDuckMiniGame::GetStartingTime() const
{
	return 30.0f;
}
FText AShootDuckMiniGame::GetTargetName() const
{
	return LOCTEXT("DUCKS", "Ducks");
}
int32 AShootDuckMiniGame::GetTargetScore() const
{
	return 20;
}