// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGRegisterAPIService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/Engine.h"

void UAIGRegisterAPIService::SendRegisterRequest(const FString& Username, const FString& Password, const FString& Nickname, const FString& Email)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("http://ceprj.gachon.ac.kr:60002/game/user/signup"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // JSON »ý¼º
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField(TEXT("username"), Username);
    JsonObject->SetStringField(TEXT("password"), Password);
    JsonObject->SetStringField(TEXT("nickname"), Nickname);
    JsonObject->SetStringField(TEXT("email"), Email);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    Request->SetContentAsString(OutputString);
    Request->OnProcessRequestComplete().BindUObject(this, &UAIGRegisterAPIService::OnRegisterResponseReceived);
    Request->ProcessRequest();
}

void UAIGRegisterAPIService::OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Response"));

    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Network Error"));
        return;
    }

    int32 StatusCode = Response->GetResponseCode();
    UE_LOG(LogTemp, Warning, TEXT("number : %d"), StatusCode);

    if (StatusCode == 201)
    {
        FString ResponseContent = Response->GetContentAsString();
        UE_LOG(LogTemp, Warning, TEXT("Success : %s"), *ResponseContent);
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Status Success"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Register Failed Code: %d, Detail: %s"),
            StatusCode, *Response->GetContentAsString());
    }
}
