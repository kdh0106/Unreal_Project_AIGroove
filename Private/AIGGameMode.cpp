// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGGameMode.h"
#include "AIGPawn.h"
#include "AIGPlayerController.h"

AAIGGameMode::AAIGGameMode()
{
	DefaultPawnClass = AAIGPawn::StaticClass();
	PlayerControllerClass = AAIGPlayerController::StaticClass();
}

