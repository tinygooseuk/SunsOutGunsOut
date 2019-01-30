// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/GameStateBase.h"
#include "PBVRGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PBVR_API APBVRGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	APBVRGameStateBase();
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	int32 TargetScore;

	UPROPERTY(BlueprintReadOnly)
	FText TargetName;
	
	UPROPERTY(BlueprintReadOnly)
	float TimerSeconds;

	UPROPERTY(BlueprintReadOnly)
	int32 Score;

	UPROPERTY(BlueprintReadOnly)
	int32 NegativeScore;

	UPROPERTY(BlueprintReadOnly)
	bool bHasGameBegun;

	// Reset gamestate to minigame values, where appropriate 
	void Reset();
};
