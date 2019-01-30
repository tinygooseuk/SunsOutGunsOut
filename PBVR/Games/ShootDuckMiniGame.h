// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "Games/MiniGame.h"
#include "ShootDuckMiniGame.generated.h"

UCLASS()
class PBVR_API AShootDuckMiniGame : public AMiniGame
{
	GENERATED_BODY()
	
public:
	AShootDuckMiniGame();

	//AActor
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

private:
	// Spawn and launch a duck
	class ADuck* SpawnDuck();
	// Spawn and launch a bomb
	class ABomb* SpawnBomb();

public:
	//AMiniGame
	virtual int32 GetStartingAmmo() const override;
	virtual float GetStartingTime() const override;
	virtual FText GetTargetName() const override;
	virtual int32 GetTargetScore() const override;

private:
	float DuckSpawnTimer;
};
