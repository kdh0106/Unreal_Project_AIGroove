// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "AIGStageData.generated.h"


UENUM(BlueprintType)
enum class EStageDifficulty : uint8
{
	EASY UMETA(DisplayName = "Easy"),
	NORMAL UMETA(DisplayName = "Normal"),
	HARD UMETA(DisplayName = "Hard")
};

USTRUCT(BlueprintType)
struct FStageData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SongTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Artist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStageDifficulty Difficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Attempts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SongID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString JSONPath; // JSON 파일의 전체 경로 (ex: /storage/emulated/0/Android/data/.../MySong_TimingData.json)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MusicFilePath; // mp3 또는 wav 파일 경로 (ex: /storage/emulated/0/.../MySong.mp3)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ThumbnailPath; // 썸네일 이미지 경로 (옵션, PNG/JPG)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* MusicFile;
};
