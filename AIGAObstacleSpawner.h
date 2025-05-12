// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIGFObstacleData.h"
#include "AIGAObstacleSpawner.generated.h"

UCLASS()
class AIGROOVE_API AAIGAObstacleSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AAIGAObstacleSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY()
    TArray<FObstacleData> SpawnList;

    // 현재 경과 시간
    float CurrentTime = 0.0f;

    // 장애물 클래스 (BP_Obstacle 연결)
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> ObstacleClass;

    // JSON 파일 읽는 함수
    void LoadObstacleData();
};
