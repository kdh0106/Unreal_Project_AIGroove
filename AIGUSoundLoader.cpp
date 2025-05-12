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
    // ������ �����ϴ��� üũ
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

    // ���⼭���� WAV ��� �м� �� ������ ������ �ʿ�
    // �� ���� �ܰ迡�� ���� (2�ܰ�: Wav �Ľ�)
    UE_LOG(LogTemp, Log, TEXT("WAV file load success (%s), Data size: %d bytes"), *FilePath, RawFileData.Num());

    return CreateSoundWaveFromWavData(RawFileData);
}

USoundWave* UAIGUSoundLoader::CreateSoundWaveFromWavData(const TArray<uint8>& WavData)
{
    // �ּ����� WAV ��� ũ�� Ȯ��
    if (WavData.Num() <= 44)
    {
        UE_LOG(LogTemp, Error, TEXT("WAV Data is too Low"));
        return nullptr;
    }

    // ----- WAV ��� �Ľ� -----
    int32 NumChannels = *reinterpret_cast<const uint16*>(&WavData[22]);
    int32 SampleRate = *reinterpret_cast<const uint32*>(&WavData[24]);
    int32 BitsPerSample = *reinterpret_cast<const uint16*>(&WavData[34]);

    // �α� Ȯ��
    UE_LOG(LogTemp, Log, TEXT("Parsed WAV Header: Channels=%d, SampleRate=%d, BitsPerSample=%d"),
        NumChannels, SampleRate, BitsPerSample);

    if (BitsPerSample != 16)
    {
        UE_LOG(LogTemp, Error, TEXT("Only 16-bit PCM is supported."));
        return nullptr;
    }

    // WAV ��� ������ PCM �����͸� ����
    const int32 HeaderSize = 44;
    const uint8* PCMData = WavData.GetData() + HeaderSize;
    int32 PCMDataSize = WavData.Num() - HeaderSize;

    USoundWaveProcedural* Sound = NewObject<USoundWaveProcedural>();
    if (!Sound)
    {
        UE_LOG(LogTemp, Error, TEXT("SoundWaveProcedural Create Fail"));
        return nullptr;
    }

    //�Ķ���� ���� (���� �Ľ� ��Ȯ�� �ϸ� ����)
    Sound->NumChannels = NumChannels;
    //Sound->SampleRate = SampleRate; //�����Ϸ��� ������ 44100�� �⺻���� �Ѵٰ� �ؼ� �ּ�ó����
    Sound->Duration = (float)PCMDataSize / (SampleRate * NumChannels * (BitsPerSample / 8)); //���� ����
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

    // Procedural ������ ��쿣 ĳ���� �ʿ�
    USoundWaveProcedural* Procedural = Cast<USoundWaveProcedural>(SoundWave);
    if (Procedural)
    {
        // Procedural ���忡�� ���� SoundClassObject �Ҵ� �Ұ�
        // �������Ʈ���� ����� ������Ʈ�� �Ҵ��ϵ��� ����
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

    // ���� ������ �ε�
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

    // Multipart body ����
    FString FileHeader = FString::Printf(TEXT(
        "%s\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"
        "Content-Type: audio/wav\r\n\r\n"
    ), *HeaderBoundary, *Filename);

    TArray<uint8> Payload;
    // ���
    Payload.Append((uint8*)TCHAR_TO_ANSI(*FileHeader), FileHeader.Len());
    // ���� ������
    Payload.Append(FileData);
    // ���� �ٿ����
    FString Footer = FString::Printf(TEXT("\r\n%s\r\n"), *EndingBoundary);
    Payload.Append((uint8*)TCHAR_TO_ANSI(*Footer), Footer.Len());

    // HTTP ��û ����
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
