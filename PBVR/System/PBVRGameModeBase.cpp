// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "PBVRGameModeBase.h"

// Engine
#include "Engine/World.h"

// Game
#include "Games/MiniGame.h"
#include "Pawns/PBPawn.h"
#include "PBVRGameStateBase.h"
#include "PBLevelScriptActor_Base.h"

APBVRGameModeBase::APBVRGameModeBase()
{	
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = APBPawn::StaticClass();
	GameStateClass = APBVRGameStateBase::StaticClass();
}

void APBVRGameModeBase::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);

	APBVRGameStateBase* GameState = GetPBVRGameState();
	GOOSE_BAIL(GameState);

	if (GameState->bHasGameBegun)
	{
		GameState->TimerSeconds -= DeltaSeconds;

		if (GameState->TimerSeconds <= 0.0f)
		{
			// Close doors
			SetDoorsOpen(false);
			GameState->bHasGameBegun = false;

			// Notify MiniGame, etc.
			TimerFinished();
		}
	}
}

void APBVRGameModeBase::BeginMiniGame(AMiniGame* InMiniGame)
{
	if (MiniGame && MiniGame != InMiniGame)
	{
		MiniGame->Destroy();
	}

	MiniGame = InMiniGame;

	// Reset gamestate
	APBVRGameStateBase* GameState = GetPBVRGameState();
	GOOSE_BAIL(GameState);
	
	GameState->Reset();
	GameState->bHasGameBegun = true;

	// Update player ammo
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);
	
	APlayerController* PC = World->GetFirstPlayerController();
	GOOSE_BAIL(PC);		
	
	APBPawn* Pawn = Cast<APBPawn>(PC->GetPawn());
	GOOSE_BAIL(Pawn);

	Pawn->SetAmmo(InMiniGame->GetStartingAmmo());

	// Open those doors!
	SetDoorsOpen(true);
}

void APBVRGameModeBase::BeginMiniGameOfClass(TSubclassOf<AMiniGame> MiniGameClass)
{
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AMiniGame* NewMiniGame = World->SpawnActor<AMiniGame>(MiniGameClass.Get(), ActorSpawnParams);
	GOOSE_BAIL(NewMiniGame);

	BeginMiniGame(NewMiniGame);
}

AMiniGame* APBVRGameModeBase::GetMiniGame() const
{
	return MiniGame;
}

void APBVRGameModeBase::TimerFinished()
{
	if (GOOSE_VERIFY(MiniGame))
	{
		MiniGame->Destroy();
		MiniGame = nullptr;
	}

	// Reset clock visually
	APBVRGameStateBase* GameState = GetPBVRGameState();
	GOOSE_BAIL(GameState);

	GameState->TimerSeconds = 60.0f;
	
	// Reset player ammo to "infinite"
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	APlayerController* PC = World->GetFirstPlayerController();
	GOOSE_BAIL(PC);

	APBPawn* Pawn = Cast<APBPawn>(PC->GetPawn());
	GOOSE_BAIL(Pawn);

	Pawn->SetAmmo(Ammo_Infinite);
}

APBVRGameStateBase* APBVRGameModeBase::GetPBVRGameState() const 
{ 
	return GetGameState<APBVRGameStateBase>(); 
}

void APBVRGameModeBase::SetDoorsOpen(bool bNewOpen)
{
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	APBLevelScriptActor_Base* LevelScript = Cast<APBLevelScriptActor_Base>(World->GetLevelScriptActor());
	GOOSE_BAIL(LevelScript);

	if (bNewOpen)
	{
		LevelScript->OpenDoors();
	}
	else
	{
		LevelScript->CloseDoors();
	}
}


void APBVRGameModeBase::AddScore(int32 InScore)
{
	APBVRGameStateBase* GameState = GetPBVRGameState();
	GOOSE_BAIL(GameState);

	GameState->Score += InScore;
}


void APBVRGameModeBase::AddNegativeScore(int32 InScore)
{
	APBVRGameStateBase* GameState = GetPBVRGameState();
	GOOSE_BAIL(GameState);

	GameState->NegativeScore += InScore;
}