// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIGFNewPathObstacleData.h"
#include "AIGFPathTimingData.h"
#include "AIGStageData.h"
#include "JsonObjectConverter.h"
#include "Components/SplineComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "AIGAPathObstacleSpawner.generated.h"

UCLASS()
class AIGROOVE_API AAIGAPathObstacleSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AAIGAPathObstacleSpawner();

protected:
	virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly, Category = "StageData")
    FStageData SelectedStageData;

public:	
	virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    void DrawSafeZones();

    UFUNCTION(BlueprintCallable)
    void ResetLevel();

    UFUNCTION(BlueprintCallable)
    bool LoadPathTimingData(const FString& JsonPath, FPathTimingData& OutData);

    UPROPERTY(BlueprintReadWrite, Category = "Spawning")
    bool bStopObstacleSpawningAfterLoading = false;

    UPROPERTY(BlueprintReadWrite)
    bool bPathInitialized = false;

    UPROPERTY(BlueprintReadOnly)
    TArray<float> PathTurnTimes;

    UPROPERTY(BlueprintReadOnly)
    TArray<FVector> PlayerPathList; // �÷��̾� ���

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "obstacle")
    TArray<UMaterialInterface*> ObstacleMaterials;

    UFUNCTION(BlueprintCallable, Category = "StageData")
    void SetSelectedStage(const FStageData& InStageData);

    // ���Ӱ� ��θ� ����ϴ� �޼���
    UFUNCTION(BlueprintCallable)
    void CalculatePlayerPath(float fSongDuration);

    // ��� ������� ��ֹ� ��ġ�ϴ� �޼���
    UFUNCTION(BlueprintCallable)
    void GenerateObstaclesFromPath(float fSongDuration);

    UFUNCTION(BlueprintCallable, Category = "Path Init")
    void SetInitialTransform(const FVector& InPosition, float InYaw);

    UPROPERTY(BlueprintReadWrite)
    float CurrentTime = 0.0f;

private:

    UPROPERTY()
    TArray<FAIGFPathObstacleData> ObstacleList; // ��ֹ� ����

    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> ObstacleClass;

    FVector FixedStartPosition;
    float FixedBaseYaw{};
    const float PathSimulateInterval = 0.001f;
};
