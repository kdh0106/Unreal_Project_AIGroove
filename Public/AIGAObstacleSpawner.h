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

    // ���� ��� �ð�
    float CurrentTime = 0.0f;

    // ��ֹ� Ŭ���� (BP_Obstacle ����)
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> ObstacleClass;

    // JSON ���� �д� �Լ�
    void LoadObstacleData();
};
