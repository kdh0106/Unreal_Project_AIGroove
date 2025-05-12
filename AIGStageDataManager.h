// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIGStageData.h"
#include "AIGStageDataManager.generated.h"

UCLASS(Blueprintable)
class AIGROOVE_API AAIGStageDataManager : public AActor
{
	GENERATED_BODY()
	
public:
    // FStageData 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
    TArray<FStageData> StageList;

    // 배열을 반환하는 함수
    UFUNCTION(BlueprintCallable, Category = "StageData")
    const TArray<FStageData>& GetAllStages() const;
};
