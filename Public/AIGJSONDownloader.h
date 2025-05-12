// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIGJSONDownloader.generated.h"

/**
 * 
 */
UCLASS()
class AIGROOVE_API UAIGJSONDownloader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Network")
	static void DownloadJSONToTimingDataFolder(const FString& URL, const FString& FileName);
};
