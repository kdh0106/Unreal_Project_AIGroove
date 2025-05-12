// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGFObstacleData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObstacleData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Scale;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool bSpawned = false;
};
