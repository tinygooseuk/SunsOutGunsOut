// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "Duck.h"

// Engine
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/PBVRGameModeBase.h"
#include "Sound/SoundWave.h"

// Plugins
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"

// Game
#include "Bomb.h"

ADuck::ADuck()
	: bExploded(false)
{
	// Resources
	UStaticMesh* SM_Ducky = FIND_RESOURCE(StaticMesh, SM_Ducky, "StaticMeshes");

	Sound_Quack = FIND_RESOURCE(SoundWave, Quack, "Sounds");

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetStaticMesh(SM_Ducky);
	StaticMeshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetNotifyRigidBodyCollision(true);
	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ADuck::OnHit);
	StaticMeshComponent->SetupAttachment(SceneRoot);
}

void ADuck::BeginPlay()
{
	Super::BeginPlay();
	
	// Set life time
	SetLifeSpan(3.0f);
}

float ADuck::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) 
{
	Explode(true);

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void ADuck::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Hit ground? go away!
	if (Cast<ADuck>(OtherActor) || Cast<ABomb>(OtherActor))
	{
		// No duck deaths!
		return;
	}

	Explode(false);
}

void ADuck::Explode(bool bAwardPoints)
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	if (bAwardPoints)
	{
		UWorld* World = GetWorld();
		GOOSE_BAIL(World);

		APBVRGameModeBase* GameMode = Cast<APBVRGameModeBase>(World->GetAuthGameMode());
		GOOSE_BAIL(GameMode);

		// Add score
		GameMode->AddScore(1);

		// Play death sound
		UGameplayStatics::PlaySoundAtLocation(this, Sound_Quack, GetActorLocation(), 1.0f, FMath::RandRange(0.8f, 1.4f));
	}

	SetLifeSpan(3.0f);

	// Remove static mesh, add destructible
	FVector Location = StaticMeshComponent->GetComponentLocation();
	FRotator Rotator = StaticMeshComponent->GetComponentRotation();
	FVector Velocity = StaticMeshComponent->GetPhysicsLinearVelocity();
	FVector AngularVel = StaticMeshComponent->GetPhysicsAngularVelocityInDegrees();
	FVector Scale = StaticMeshComponent->GetComponentScale();

	StaticMeshComponent->DestroyComponent();

	FAttachmentTransformRules Attachment = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);

	UDestructibleMesh* SM_Ducky_Simple_DM = UGooseUtil::GetObject<UDestructibleMesh>(TEXT("SM_Ducky_Simple_DM"), TEXT("StaticMeshes"));
	
	UDestructibleComponent* Destructible = NewObject<UDestructibleComponent>(this, TEXT("Destructible"));
	Destructible->SetWorldLocationAndRotation(Location, Rotator);
	Destructible->SetDestructibleMesh(SM_Ducky_Simple_DM);
	Destructible->SetWorldScale3D(Scale);
	Destructible->RegisterComponent();
	Destructible->AttachToComponent(GetRootComponent(), Attachment);

	Destructible->SetSimulatePhysics(true);
	Destructible->SetPhysicsLinearVelocity(Velocity);
	Destructible->SetPhysicsAngularVelocityInDegrees(AngularVel);
	Destructible->ApplyRadiusDamage(1.0f, Location, 10.0f, 100.0f, true);
	Destructible->AddRadialImpulse(Location, 10.0f, 100.0f, ERadialImpulseFalloff::RIF_Linear, true);
}