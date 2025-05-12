// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGFPathTimingData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPathTimingData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> TurnTimes;
};