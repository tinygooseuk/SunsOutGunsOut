// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "Games/MiniGame.h"

#include "Misc/AnimalEnum.h"

#include "AnimalMatchMiniGame.generated.h"

enum EAnimalMatchMiniGameState
{
	AMMGS_ReRack,
	AMMGS_AwaitShowAnimals,
	AMMGS_AwaitShot,
	AMMGS_AwaitReRack
};

UCLASS()
class PBVR_API AAnimalMatchMiniGame : public AMiniGame
{
	GENERATED_BODY()

public:
	AAnimalMatchMiniGame();

	//AActor
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	// Called when an animal is shot
	void NotifyShotAnimal(EAnimal Animal, FVector Location);

	// Add all animals into rack
	void SetupAnimals();
	// Remove all animals
	void TearDownAnimals();

private:
	// Get the socket name for a given (x,y) position
	FName GetSocketNameForPosition(int32 X, int32 Y) const;
	// Get the socket name for the top target animal slot
	FName GetSocketNameForTarget() const;

	// Spawn the target animal (in the window at the top)
	class AAnimal* SpawnTargetAnimal();
	// Spawn a full rack of animals
	void SpawnRackOfAnimals();

public:
	//AMiniGame
	virtual int32 GetStartingAmmo() const override;
	virtual float GetStartingTime() const override;
	virtual FText GetTargetName() const override;
	virtual int32 GetTargetScore() const override;

private:
	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class UStaticMesh* SM_AnimalMatchMachine;
		
	UPROPERTY()
	class UStaticMesh* SM_BigAssArrow;

	UPROPERTY()
	class USoundWave* Sound_Bang;

	UPROPERTY()
	class USoundWave* Sound_Correct;

	///////////////////////// Components /////////////////////////
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MachineMesh;
	
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* BigAssArrowMesh;

	///////////////////////// State /////////////////////////
	EAnimalMatchMiniGameState State;

	float Timer;

	///////////////////////// Actor Refs /////////////////////////
	UPROPERTY()
	AAnimal* TargetAnimal;

	UPROPERTY()
	TArray<AAnimal*> SpawnedAnimals;
};

