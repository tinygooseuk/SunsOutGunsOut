// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "Bomb.h"

// Engine
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/Emitter.h"
#include "Particles/ParticleSystemComponent.h"
#include "System/PBVRGameModeBase.h"
#include "Sound/SoundWave.h"

// Plugins
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"

// Game
#include "Duck.h"

ABomb::ABomb()
{
	// Resources
	UStaticMesh* SM_Bomb = FIND_RESOURCE(StaticMesh, SM_Bomb, "StaticMeshes");

	Sound_Bang = FIND_RESOURCE(SoundWave, Bang, "Sounds");

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetStaticMesh(SM_Bomb);
	StaticMeshComponent->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetNotifyRigidBodyCollision(true);
	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ABomb::OnHit);
	
	SetRootComponent(StaticMeshComponent);
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();

	// Set life time
	SetLifeSpan(3.0f);
}

float ABomb::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Explode(true);

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void ABomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Hit ground? go away!
	if (Cast<ADuck>(OtherActor) || Cast<ABomb>(OtherActor))
	{
		// No duck deaths!
		return;
	}

	Explode(false);
}

void ABomb::SpawnExplosion(UObject* WorldContextObject, const FVector& WorldPosition, float Scale)
{
	UWorld* World = WorldContextObject->GetWorld();
	GOOSE_BAIL(World);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEmitter* Emitter = World->SpawnActor<AEmitter>(AEmitter::StaticClass(), WorldPosition, FRotator::ZeroRotator, SpawnParams);
	Emitter->SetActorScale3D(FVector(Scale, Scale, Scale));
	Emitter->bDestroyOnSystemFinish = true;

	UParticleSystem* P_Explosion = UGooseUtil::GetObject<UParticleSystem>(TEXT("P_Explosion"), TEXT("Particles"));
	GOOSE_BAIL(P_Explosion);

	UParticleSystemComponent* System = Emitter->GetParticleSystemComponent();
	System->SetTemplate(P_Explosion);
}

void ABomb::Explode(bool bAwardPoints)
{
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	if (bAwardPoints)
	{
		APBVRGameModeBase* GameMode = Cast<APBVRGameModeBase>(World->GetAuthGameMode());
		GOOSE_BAIL(GameMode);

		// Add score
		GameMode->AddNegativeScore(1);

		// Play death sound
		UGameplayStatics::PlaySoundAtLocation(this, Sound_Bang, GetActorLocation(), 1.0f, FMath::RandRange(0.8f, 1.4f));
	}

	// Add explosion FX
	SpawnExplosion(this, GetActorLocation(), 5.0f);
	
	// Destroy bomb
	Destroy();
}