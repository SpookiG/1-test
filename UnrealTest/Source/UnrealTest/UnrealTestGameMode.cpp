// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealTestGameMode.h"
#include "UnrealTestPawn.h"

AUnrealTestGameMode::AUnrealTestGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AUnrealTestPawn::StaticClass();
}
