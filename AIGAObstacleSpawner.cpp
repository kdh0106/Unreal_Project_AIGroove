// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGAObstacleSpawner.h"
#include "AIGObstacleLevelLoader.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAIGAObstacleSpawner::AAIGAObstacleSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIGAObstacleSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	LoadObstacleData();
}

// Called every frame
void AAIGAObstacleSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += DeltaTime;

	for (FObstacleData& ObstacleData : SpawnList)
	{
		if (!ObstacleData.bSpawned && CurrentTime >= ObstacleData.SpawnTime)
		{
			FActorSpawnParameters SpawnParams;
			AActor* SpawnedObstacle = GetWorld()->SpawnActor<AActor>(
				ObstacleClass,
				ObstacleData.Position,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (SpawnedObstacle)
			{
				SpawnedObstacle->SetActorScale3D(ObstacleData.Scale);
				ObstacleData.bSpawned = true;
			}
		}
	}
}

void AAIGAObstacleSpawner::LoadObstacleData()
{
	FString RelativePath = TEXT("HandMade/Systems/LevelSpawner/TestLevel.json");

	bool bSuccess = FObstacleLevelLoader::LoadLevelDataFromJson(RelativePath, SpawnList);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Obstacle Data Loaded: %d items"), SpawnList.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Load Obstacle Data"));
	}
}

