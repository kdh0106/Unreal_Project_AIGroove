// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGLevelChunk.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAIGLevelChunk::AAIGLevelChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void AAIGLevelChunk::BeginPlay()
{
	Super::BeginPlay();

	TargetLocation = GetActorLocation();  // ���� ���� ��ġ ����
	FVector StartLocation = TargetLocation + FVector(0.0f, 0.0f, StartZOffset);
	SetActorLocation(StartLocation);

	if (bIsFirstChunk)
	{
		ShowChunk();
	}

}

// Called every frame
void AAIGLevelChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
		if (Distance < ActivationDistance)
		{
			ShowChunk();
		}
	}

	if (bIsFalling)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, FallSpeed * 0.01f);

		SetActorLocation(NewLocation);

		// ��ǥ ��ġ�� �����ϸ� ����
		if (FVector::Dist(NewLocation, TargetLocation) < 1.0f)
		{
			SetActorLocation(TargetLocation);
			bIsFalling = false;  // �ִϸ��̼� ����
		}
	}
}

void AAIGLevelChunk::ShowChunk()
{
	SetActorHiddenInGame(false);
	bIsVisible = true;
	bIsFalling = true;
}

void AAIGLevelChunk::HideChunk()
{
	SetActorHiddenInGame(true);
	bIsVisible = false;
}
