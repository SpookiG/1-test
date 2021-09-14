// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealTestGameMode.h"
#include "UnrealTestPawn.h"
#include "CarPilot.h"

AUnrealTestGameMode::AUnrealTestGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AFloatyCar::StaticClass();

	PlayerControllerClass = ACarPilot::StaticClass();
}
