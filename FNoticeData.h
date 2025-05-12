#pragma once

#include "CoreMinimal.h"
#include "FNoticeData.generated.h"

USTRUCT(BlueprintType)
struct FNoticeData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Title;

	UPROPERTY(BlueprintReadWrite)
	FString Content;

	UPROPERTY(BlueprintReadWrite)
	FString CreatedAt;
};
