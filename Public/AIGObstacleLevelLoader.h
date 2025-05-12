// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGFObstacleData.h"

/**
 * 
 */
class FObstacleLevelLoader
{
public:
    static bool LoadLevelDataFromJson(const FString& FilePath, TArray<FObstacleData>& OutObstacles);
};
