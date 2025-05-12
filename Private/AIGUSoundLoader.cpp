// Fill out your copyright notice in the Description page of Project Settings.

#include "AIGUSoundLoader.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundWaveProcedural.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"


USoundWave* UAIGUSoundLoader::LoadWavFromDisk(const FString& FilePath)
{
    // 파일이 존재하는지 체크
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("WAV not exist: %s"), *FilePath);
        return nullptr;
    }

    TArray<uint8> RawFileData;
    if (!FFileHelper::LoadFileToArray(RawFileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("WAV file load fail: %s"), *FilePath);
        return nullptr;
    }

    // 여기서부터 WAV 헤더 분석 및 데이터 추출이 필요
    // → 다음 단계에서 진행 (2단계: Wav 파싱)
    UE_LOG(LogTemp, Log, TEXT("WAV file load success (%s), Data size: %d bytes"), *FilePath, RawFileData.Num());

    return CreateSoundWaveFromWavData(RawFileData);
}

USoundWave* UAIGUSoundLoader::CreateSoundWaveFromWavData(const TArray<uint8>& WavData)
{
    // 최소한의 WAV 헤더 크기 확인
    if (WavData.Num() <= 44)
    {
        UE_LOG(LogTemp, Error, TEXT("WAV Data is too Low"));
        return nullptr;
    }

    // ----- WAV 헤더 파싱 -----
    int32 NumChannels = *reinterpret_cast<const uint16*>(&WavData[22]);
    int32 SampleRate = *reinterpret_cast<const uint32*>(&WavData[24]);
    int32 BitsPerSample = *reinterpret_cast<const uint16*>(&WavData[34]);

    // 로그 확인
    UE_LOG(LogTemp, Log, TEXT("Parsed WAV Header: Channels=%d, SampleRate=%d, BitsPerSample=%d"),
        NumChannels, SampleRate, BitsPerSample);

    if (BitsPerSample != 16)
    {
        UE_LOG(LogTemp, Error, TEXT("Only 16-bit PCM is supported."));
        return nullptr;
    }

    // WAV 헤더 이후의 PCM 데이터만 추출
    const int32 HeaderSize = 44;
    const uint8* PCMData = WavData.GetData() + HeaderSize;
    int32 PCMDataSize = WavData.Num() - HeaderSize;

    USoundWaveProcedural* Sound = NewObject<USoundWaveProcedural>();
    if (!Sound)
    {
        UE_LOG(LogTemp, Error, TEXT("SoundWaveProcedural Create Fail"));
        return nullptr;
    }

    //파라미터 설정 (추후 파싱 정확히 하면 수정)
    Sound->NumChannels = NumChannels;
    //Sound->SampleRate = SampleRate; //수정하려고 했으나 44100을 기본제공 한다고 해서 주석처리함
    Sound->Duration = (float)PCMDataSize / (SampleRate * NumChannels * (BitsPerSample / 8)); //길이 계산식
    Sound->SoundGroup = SOUNDGROUP_Default;
    Sound->bLooping = false;

    Sound->QueueAudio(PCMData, PCMDataSize);

    //static ConstructorHelpers::FObjectFinder<USoundClass> SoundClassFinder(TEXT("/Game/HandMade/Sounds/SC_Master.SC_Master"));
    //if (SoundClassFinder.Succeeded())
    //{
    //    YourSoundClass = SoundClassFinder.Object;
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("SC_Master not found at path!"));
    //}

    UE_LOG(LogTemp, Log, TEXT("Created SoundWaveProcedural: Size=%d"), PCMDataSize);
    return Sound;
}

USoundWave* UAIGUSoundLoader::PrepareSoundWaveWithClass(USoundWave* SoundWave, USoundClass* SoundClass)
{
    if (!SoundWave || !SoundClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("PrepareSoundWaveWithClass: Invalid SoundWave or SoundClass"));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "OMG SOUNDCLASS MISSED");
        return nullptr;
    }

    // Procedural 사운드일 경우엔 캐스팅 필요
    USoundWaveProcedural* Procedural = Cast<USoundWaveProcedural>(SoundWave);
    if (Procedural)
    {
        // Procedural 사운드에는 직접 SoundClassObject 할당 불가
        // 블루프린트에서 오디오 컴포넌트에 할당하도록 유도
        UE_LOG(LogTemp, Log, TEXT("PrepareSoundWaveWithClass: SoundClass will be assigned at spawn time"));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "CAN NOT PREPARE SOUND");
    }
    else
    {
        SoundWave->SoundClassObject = SoundClass;
    }

    return SoundWave;
}

void UAIGUSoundLoader::UploadMusicToServer(const FString& FilePath, const FString& ServerURL)
{
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Upload Failed: File does not exist: %s"), *FilePath);
        return;
    }

    // 파일 데이터 로드
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to read file for upload."));
        return;
    }

    FString Boundary = "----AIGBoundary";
    FString HeaderBoundary = "--" + Boundary;
    FString EndingBoundary = HeaderBoundary + "--";

    FString Filename = FPaths::GetCleanFilename(FilePath);

    // Multipart body 구성
    FString FileHeader = FString::Printf(TEXT(
        "%s\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"
        "Content-Type: audio/wav\r\n\r\n"
    ), *HeaderBoundary, *Filename);

    TArray<uint8> Payload;
    // 헤더
    Payload.Append((uint8*)TCHAR_TO_ANSI(*FileHeader), FileHeader.Len());
    // 파일 데이터
    Payload.Append(FileData);
    // 종료 바운더리
    FString Footer = FString::Printf(TEXT("\r\n%s\r\n"), *EndingBoundary);
    Payload.Append((uint8*)TCHAR_TO_ANSI(*Footer), Footer.Len());

    // HTTP 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(ServerURL);
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), FString("multipart/form-data; boundary=") + Boundary);
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetContent(Payload);

    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
        {
            if (bSuccess && Resp.IsValid())
            {
                UE_LOG(LogTemp, Log, TEXT("Upload Success: %s"), *Resp->GetContentAsString());
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Upload Success");
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Upload Failed"));
            }
        });

    Request->ProcessRequest();
}
