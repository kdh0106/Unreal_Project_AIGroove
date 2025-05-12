// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIGrooveGameMode.h"
#include "AIGrooveCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAIGrooveGameMode::AAIGrooveGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
