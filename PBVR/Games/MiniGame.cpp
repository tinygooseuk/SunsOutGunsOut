// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "MiniGame.h"

// Engine
#include "EngineUtils.h"

void AMiniGame::BeginPlay()
{
	Super::BeginPlay();
	
	// Find action volume
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	TActorIterator<AActionVolume> It(World);
	GOOSE_BAIL(It);
	
	ActionVolume = *It;
}
	

int32 AMiniGame::GetStartingAmmo() const
{
	return -1;
}
float AMiniGame::GetStartingTime() const
{
	return 60.0f;
}
FText AMiniGame::GetTargetName() const
{
	return LOCTEXT("TEXT_THINGS", "Things");
}
int32 AMiniGame::GetTargetScore() const
{
	return 10;
}