// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "Engine/LevelScriptActor.h"
#include "PBLevelScriptActor_Base.generated.h"

UCLASS()
class PBVR_API APBLevelScriptActor_Base : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	// Open the main barrier doors
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OpenDoors();

	// Close the main barrier doors
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void CloseDoors();
};
