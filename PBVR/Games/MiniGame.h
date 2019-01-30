// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/Actor.h"

// Game
#include "Misc/ActionVolume.h"

#include "MiniGame.generated.h"

UCLASS()
class PBVR_API AMiniGame : public AActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual int32 GetStartingAmmo() const;
	virtual float GetStartingTime() const;
	virtual FText GetTargetName() const;
	virtual int32 GetTargetScore() const;

protected:
	TWeakObjectPtr<AActionVolume> ActionVolume;
};
