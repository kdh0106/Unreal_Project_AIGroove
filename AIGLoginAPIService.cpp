// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGLoginAPIService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "AIGGameInstance.h"
#include "Engine/Engine.h"

void UAIGLoginAPIService::SendLoginRequest(const FString& Username, const FString& Password)
{
    // 1. JSON 데이터 구성
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField(TEXT("username"), Username);
    JsonObject->SetStringField(TEXT("password"), Password);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // 2. HTTP 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("http://ceprj.gachon.ac.kr:60002/game/user/login")); 
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(OutputString);

    // 3. 응답 바인딩
    Request->OnProcessRequestComplete().BindUObject(this, &UAIGLoginAPIService::OnLoginResponseReceived);

    // 4. 요청 전송
    Request->ProcessRequest();
}

void UAIGLoginAPIService::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Response"));

    if (!bWasSuccessful || !Response.IsValid())
    {
        OnLoginFail(TEXT("Network Error! login fail!"));
        return;
    }

    int32 StatusCode = Response->GetResponseCode();
    ABLOG(Warning, TEXT("number : %d"), StatusCode);

    if (StatusCode == 200)
    {
        FString ResponseContent = Response->GetContentAsString();
        ABLOG(Warning, TEXT("Login Success : %s"), *ResponseContent);
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Status Success"));

        //응답을 JSON으로 파싱
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            FString Token = JsonObject->GetStringField("token"); // 예: {"token": "abcd1234"}
            FString UserName = JsonObject->GetStringField("username");

            ABLOG(Warning, TEXT("Received Token: %s"), *Token);
            ABLOG(Warning, TEXT("UserName: %s"), *UserName);

            OnLoginSuccess(Token, UserName);
        }
        else
        {
            OnLoginFail(TEXT("JSON Pharsing failed"));
        }
    }
    else
    {
        OnLoginFail(FString::Printf(TEXT("Respond code : %d - Login Failed"), StatusCode));
    }

}

void UAIGLoginAPIService::OnLoginSuccess(const FString& Token, const FString& UserName)
{
    ABLOG(Warning, TEXT("[Login Success] Token: %s, UserName: %s"), *Token, *UserName);
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Login Success!"));

    if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(this))
    {
        UAIGGameInstance* AIGInstance = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(World));
        if (AIGInstance)
        {
            AIGInstance->ShowMainMenu();
            AIGInstance->SetAuthToken(Token);
            AIGInstance->SetUserName(UserName);
            ABLOG(Warning, TEXT("Token stored in GameInstance"));
            ABLOG(Warning, TEXT("After Login, GI Address : %p"), AIGInstance);
        }
    }
}

void UAIGLoginAPIService::OnLoginFail(const FString& ErrorMessage)
{
    ABLOG(Error, TEXT("[Login Fail] %s"), *ErrorMessage);
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Login Failed!"));
    if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(this))
    {
        UAIGGameInstance* AIGInstance = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(World));
        if (AIGInstance)
        {
            AIGInstance->ShowLoginErrorPopup();
            ABLOG(Warning, TEXT("login fail until here sibal"));
        }
    }
}