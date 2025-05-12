// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "AIGAPathObstacleSpawner.h"
#include "AIGGameMode.h"


void AAIGPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AAIGPlayerController::RespawnPlayer()
{
    //1. 먼저 PathObstacleSpawner를 찾아서 ResetLevel 호출
    //TArray<AActor*> FoundSpawners;
    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIGAPathObstacleSpawner::StaticClass(), FoundSpawners);

    //if (FoundSpawners.Num() > 0)
    //{
    //    AAIGAPathObstacleSpawner* Spawner = Cast<AAIGAPathObstacleSpawner>(FoundSpawners[0]);
    //    if (Spawner)
    //    {
    //        Spawner->ResetLevel();
    //        UE_LOG(LogTemp, Log, TEXT("Level Reset Successfully!"));
    //    }
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("No AIGAPathObstacleSpawner found!"));
    //}
	if (GetPawn())
	{
		GetPawn()->Destroy();
	}
	AAIGGameMode* GM = Cast<AAIGGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->RestartPlayer(this);
	}

    APawn* NewPawn = GetPawn();
    if (NewPawn)
    {
        NewPawn->SetActorLocation(LastCheckpointLocation);
        UE_LOG(LogTemp, Log, TEXT("Pawn moved to checkpoint location: %s"), *LastCheckpointLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get new Pawn after respawn!"));
    }
}
