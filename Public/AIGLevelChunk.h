// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIGLevelChunk.generated.h"

UCLASS()
class AIGROOVE_API AAIGLevelChunk : public AActor 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIGLevelChunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Chunk")
	bool bIsVisible = false;

	UPROPERTY(EditAnywhere, Category = "Chunk")
	bool bIsFirstChunk = false;

	UPROPERTY(EditAnywhere, Category = "Chunk")
	float ActivationDistance = 1000.0f;

	void ShowChunk();

	void HideChunk();

	UPROPERTY(EditAnywhere, Category = "Chunk")
	float StartZOffset = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Chunk")
	float FallSpeed = 500.0f;

	bool bIsFalling = false;
	FVector TargetLocation;

	void ShowChunkWithAnimation();

};
