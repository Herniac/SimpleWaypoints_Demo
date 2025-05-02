// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleWaypoints_DemoGameMode.h"
#include "SimpleWaypoints_DemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASimpleWaypoints_DemoGameMode::ASimpleWaypoints_DemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
