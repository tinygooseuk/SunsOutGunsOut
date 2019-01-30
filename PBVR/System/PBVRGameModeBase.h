// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/GameModeBase.h"
#include "PBVRGameModeBase.generated.h"

UCLASS()
class PBVR_API APBVRGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	APBVRGameModeBase();

	virtual void Tick(float DeltaSeconds) override;

	///////////////////////// Hooks /////////////////////////
	// Start a given minigame
	UFUNCTION(BlueprintCallable)
	void BeginMiniGame(class AMiniGame* Game);

	// Create and start a given minigame
	UFUNCTION(BlueprintCallable)
	void BeginMiniGameOfClass(TSubclassOf<AMiniGame> GameClass);

	// Get the currently-playing minigame
	UFUNCTION(BlueprintCallable)
	class AMiniGame* GetMiniGame() const;

	// Timer is up - stop the game
	UFUNCTION()
	void TimerFinished();	

	///////////////////////// Getters /////////////////////////
	// Get the GameState
	UFUNCTION(BlueprintCallable)
	class APBVRGameStateBase* GetPBVRGameState() const;

	///////////////////////// Public API /////////////////////////
	// Set doors open/closed
	UFUNCTION(BlueprintCallable)
	void SetDoorsOpen(bool bNewOpen);

	// Add a positive score
	UFUNCTION(BlueprintCallable)
	void AddScore(int32 Score);

	// Add a negative score
	UFUNCTION(BlueprintCallable)
	void AddNegativeScore(int32 Score);


private:
	UPROPERTY()
	class AMiniGame* MiniGame;
};
