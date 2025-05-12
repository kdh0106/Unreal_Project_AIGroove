#include "AIGAndroidFileOpener.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <jni.h>
#endif

#if PLATFORM_ANDROID
// 선택된 파일의 전체 경로
static FString LastSelectedFilePath = TEXT("");

// ClassLoader 기반 클래스 로딩 함수
jclass GetClassViaClassLoader(JNIEnv* Env, const char* ClassName)
{
    jobject activity = FAndroidApplication::GetGameActivityThis();
    jclass activityClass = Env->GetObjectClass(activity);

    jmethodID getClassLoaderMethod = Env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject classLoaderObj = Env->CallObjectMethod(activity, getClassLoaderMethod);

    jclass classLoaderClass = Env->FindClass("java/lang/ClassLoader");
    jmethodID loadClassMethod = Env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring classNameStr = Env->NewStringUTF(ClassName);
    jclass result = (jclass)Env->CallObjectMethod(classLoaderObj, loadClassMethod, classNameStr);

    Env->DeleteLocalRef(classNameStr);
    return result;
}
#endif

// 블루프린트에서 경로 가져오기
FString UAIGAndroidFileOpener::GetLastSelectedFilePath()
{
#if PLATFORM_ANDROID
    if (LastSelectedFilePath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("GetLastSelectedFilePath() called, but path is empty!"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("GetLastSelectedFilePath() returned: %s"), *LastSelectedFilePath);
    }

    return LastSelectedFilePath;
#else
    return FString("NOT_ANDROID");
#endif
}

// (이전 방식에서 사용한 것 - 필요 없으면 제거 가능)
//FString UAIGAndroidFileOpener::GetSelectedWavAbsolutePath()
//{
////#if PLATFORM_ANDROID
////    FString BasePath = FPaths::ProjectPersistentDownloadDir();  // /Android/data/패키지명/files/
////    FString FileName = FPaths::GetCleanFilename(LastSelectedFilePath);
////    FString FullPath = FPaths::Combine(BasePath, TEXT("AIGAssets/Audio"), FileName);
////    return FullPath;
////#else
////    return FString("NOT_ANDROID");
////#endif
//}

void UAIGAndroidFileOpener::OpenAndroidFilePicker()
{
#if PLATFORM_ANDROID
    if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
    {
        jobject Activity = FAndroidApplication::GetGameActivityThis();

        // ClassLoader를 통해 AIGFilePickerHelper 클래스 로딩
        jclass HelperClass = GetClassViaClassLoader(Env, "com.GameFourMan.AIGroove.AIGFilePickerHelper");
        if (!HelperClass)
        {
            UE_LOG(LogTemp, Error, TEXT("JNI: Failed to load AIGFilePickerHelper via ClassLoader"));
            return;
        }

        // openFilePicker(Activity activity) 메서드 ID 얻기
        jmethodID MethodID = Env->GetStaticMethodID(HelperClass, "openFilePicker", "(Landroid/app/Activity;)V");
        if (!MethodID)
        {
            UE_LOG(LogTemp, Error, TEXT("JNI: Failed to find static method openFilePicker"));
            return;
        }

        // 호출
        Env->CallStaticVoidMethod(HelperClass, MethodID, Activity);
    }
#endif
}

#if PLATFORM_ANDROID
// Java에서 전체 경로 전달받기
extern "C"
JNIEXPORT void JNICALL
Java_com_GameFourMan_AIGroove_AIGFilePickerHelper_nativeOnFileSelected(JNIEnv* Env, jobject Thiz, jstring fullPath)
{
    if (!fullPath)
    {
        UE_LOG(LogTemp, Error, TEXT("JNI: fullPath is null"));
        LastSelectedFilePath = TEXT("INVALID_PATH");
        return;
    }

    const char* UTF8Chars = Env->GetStringUTFChars(fullPath, nullptr);
    if (UTF8Chars)
    {
        LastSelectedFilePath = UTF8_TO_TCHAR(UTF8Chars);
        Env->ReleaseStringUTFChars(fullPath, UTF8Chars);
    }
    else
    {
        LastSelectedFilePath = TEXT("UTF8_ERROR");
    }

    UE_LOG(LogTemp, Warning, TEXT("[JNI] Selected File Path: %s"), *LastSelectedFilePath);
}
#endif