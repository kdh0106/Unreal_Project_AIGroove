#include "AIGInquiryAPIService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Kismet/GameplayStatics.h"
#include "AIGGameInstance.h"

void UAIGInquiryAPIService::SendInquiryRequest(const FString& Title, const FString& Content)
{
	UAIGGameInstance* GI = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GI) return;

	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	//Json->SetStringField("userId", GI->GetUserName());
	Json->SetStringField("title", Title);
	Json->SetStringField("content", Content);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://ceprj.gachon.ac.kr:60002/game/settings/inquiry"));  // URL은 백엔드 확인!
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *GI->GetAuthToken()));
	Request->SetContentAsString(OutputString);
	Request->OnProcessRequestComplete().BindUObject(this, &UAIGInquiryAPIService::OnInquiryResponseReceived);
	Request->ProcessRequest();
}

void UAIGInquiryAPIService::OnInquiryResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Inquiry Failed (Network Error)"));
		return;
	}

	int32 Code = Response->GetResponseCode();
	if (Code == 201)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inquiry Success : %s"), *Response->GetContentAsString());
		if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(this))
		{
			UAIGGameInstance* GI = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(World));
			if (GI)
			{

				ABLOG(Warning, TEXT("After inquiry, GI Address : %p"), GI);
				ABLOG(Warning, TEXT("After inquiry, Token : %s"), *GI->GetAuthToken());
				GI->ShowMainMenu();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Inquiry Failed [%d]: %s"), Code, *Response->GetContentAsString());
	}
}
