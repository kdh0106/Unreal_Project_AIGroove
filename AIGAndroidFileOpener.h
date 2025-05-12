// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIGAndroidFileOpener.generated.h"

/**
 * 
 */
UCLASS()
class AIGROOVE_API UAIGAndroidFileOpener : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:


    UFUNCTION(BlueprintCallable, Category = "Android")
    static void OpenAndroidFilePicker();

    UFUNCTION(BlueprintCallable, Category = "Android")
    static FString GetLastSelectedFilePath();

    //UFUNCTION(BlueprintCallable, Category = "Android")
    //static FString GetSelectedWavAbsolutePath();
};
