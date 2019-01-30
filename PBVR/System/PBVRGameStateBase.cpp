// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "PBVRGameStateBase.h"

// Engine
#include "Engine/World.h"

// Game
#include "Games/MiniGame.h"
#include "PBVRGameModeBase.h"

APBVRGameStateBase::APBVRGameStateBase()
	: bHasGameBegun(false)
	, TimerSeconds(60.0f)
{
	//Reset();
}

void APBVRGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	Reset();
}

void APBVRGameStateBase::Reset()
{
	// Get values from game
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	APBVRGameModeBase* GameMode = Cast<APBVRGameModeBase>(World->GetAuthGameMode());
	GOOSE_BAIL(GameMode);

	AMiniGame* MiniGame = GameMode->GetMiniGame();
	GOOSE_BAIL(MiniGame);

	// Query for targets
	TargetScore = MiniGame->GetTargetScore();
	TargetName = MiniGame->GetTargetName();

	TimerSeconds = MiniGame->GetStartingTime();

	// Reset scores
	Score = 0;
	NegativeScore = 0;
}