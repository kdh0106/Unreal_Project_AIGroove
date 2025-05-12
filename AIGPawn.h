// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIGroove.h"
#include "GameFramework/Pawn.h"
#include "AIGLevelChunk.h"
#include "AIGGameInstance.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AIGPawn.generated.h"

UCLASS()
class AIGROOVE_API AAIGPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAIGPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 800.0f;

	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Trail")
	float SpawnDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Trail")
	FVector CubeSize = FVector(100.0f, 100.0f, 100.0f);

	int RotationDirection = 1;

	UFUNCTION(BlueprintCallable)
	void Turn();

	//void CreateTrailCube();

	FVector LastSpawnLocation;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> CubeBlueprint;
};
