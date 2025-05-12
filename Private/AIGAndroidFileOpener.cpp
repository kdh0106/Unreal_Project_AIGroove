#include "AIGAndroidFileOpener.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <jni.h>
#endif

#if PLATFORM_ANDROID
// ���õ� ������ ��ü ���
static FString LastSelectedFilePath = TEXT("");

// ClassLoader ��� Ŭ���� �ε� �Լ�
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

// �������Ʈ���� ��� ��������
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

// (���� ��Ŀ��� ����� �� - �ʿ� ������ ���� ����)
//FString UAIGAndroidFileOpener::GetSelectedWavAbsolutePath()
//{
////#if PLATFORM_ANDROID
////    FString BasePath = FPaths::ProjectPersistentDownloadDir();  // /Android/data/��Ű����/files/
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

        // ClassLoader�� ���� AIGFilePickerHelper Ŭ���� �ε�
        jclass HelperClass = GetClassViaClassLoader(Env, "com.GameFourMan.AIGroove.AIGFilePickerHelper");
        if (!HelperClass)
        {
            UE_LOG(LogTemp, Error, TEXT("JNI: Failed to load AIGFilePickerHelper via ClassLoader"));
            return;
        }

        // openFilePicker(Activity activity) �޼��� ID ���
        jmethodID MethodID = Env->GetStaticMethodID(HelperClass, "openFilePicker", "(Landroid/app/Activity;)V");
        if (!MethodID)
        {
            UE_LOG(LogTemp, Error, TEXT("JNI: Failed to find static method openFilePicker"));
            return;
        }

        // ȣ��
        Env->CallStaticVoidMethod(HelperClass, MethodID, Activity);
    }
#endif
}

#if PLATFORM_ANDROID
// Java���� ��ü ��� ���޹ޱ�
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