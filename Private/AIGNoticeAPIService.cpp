#include "AIGNoticeAPIService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

void UAIGNoticeAPIService::SendGetNoticeRequest(UUserWidget* TargetWidget)
{
	StoredWidget = TargetWidget;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://ceprj.gachon.ac.kr:60002/game/notice"));  // ¼­¹ö URL
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->OnProcessRequestComplete().BindUObject(this, &UAIGNoticeAPIService::OnNoticeResponseReceived);
	Request->ProcessRequest();
}

void UAIGNoticeAPIService::OnNoticeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Notice Request Failed!!!"));
		return;
	}

	FString ResponseStr = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		FString Title = JsonObject->GetStringField("title");
		FString Content = JsonObject->GetStringField("content");

		if (StoredWidget.IsValid())
		{
			UUserWidget* Widget = StoredWidget.Get();

			UTextBlock* MessageText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("TextMainMessage")));
			if (MessageText)
			{
				MessageText->SetText(FText::FromString(Content));
			}
		}
	}
}
