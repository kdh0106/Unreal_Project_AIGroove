#pragma once

#include "AIGroove.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "AIGInquiryAPIService.generated.h"

UCLASS(Blueprintable)
class AIGROOVE_API UAIGInquiryAPIService : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HTTP|Inquiry")
	void SendInquiryRequest(const FString& Title, const FString& Content);

private:
	void OnInquiryResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
