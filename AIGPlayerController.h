// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIGroove.h"
#include "GameFramework/PlayerController.h"
#include "AIGPlayerController.generated.h"

/**
 *
 */
UCLASS()
class AIGROOVE_API AAIGPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void RespawnPlayer();

	UPROPERTY(BlueprintReadWrite)
	FVector LastCheckpointLocation = FVector(270.0f, 5580.0f, 122.0f);

	UPROPERTY(BlueprintReadWrite)
	float LastCheckpointBGMTime = 0.0f;
};
