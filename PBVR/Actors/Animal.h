// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/Actor.h"

// Game
#include "Misc/AnimalEnum.h"

#include "Animal.generated.h"

UCLASS()
class PBVR_API AAnimal : public AActor
{
	GENERATED_BODY()
	
public:	
	AAnimal();

	// Initialise the animal with a given animal type
	void Init(EAnimal Animal, bool bIsTarget = false);

	// AActor:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	
	// Set this animal as popped up/down
	void SetPoppedUp(bool bPopup);

	// Shatter the animal into a million pieces
	void Shatter();

	// Return the animal type for this animal
	EAnimal GetAnimal() const;

private:
	// Get the mesh to use for a given animal
	class UStaticMesh* GetStaticMeshForAnimal(EAnimal Animal) const;

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class UStaticMesh* SM_Lion;

	UPROPERTY()
	class UStaticMesh* SM_Elephant;

	UPROPERTY()
	class UStaticMesh* SM_Pig;

	UPROPERTY()
	class UStaticMesh* SM_Panda; 

	///////////////////////// Components /////////////////////////
	UPROPERTY()
	class USceneComponent* SceneRoot;

	UPROPERTY()
	class UStaticMeshComponent* AnimalMesh;	

	///////////////////////// Properties /////////////////////////
	bool bIsTarget;
	EAnimal Animal;

	///////////////////////// State /////////////////////////
	bool bPoppedUp;
	bool bShattered;
};
