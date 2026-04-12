#include <jni.h>
#include <string>
#include <filesystem>
#include <Emulator.hpp>

#include "EmulatorMain.hpp"

static std::unique_ptr<EmulatorMain> s_emulator = nullptr;

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_initEmulator(JNIEnv* env, jobject /* this */)
{
    s_emulator = std::make_unique<EmulatorMain>();
    s_emulator->run();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_loadROM(JNIEnv* env, jobject /* this */, jstring romPath)
{
    auto cstr = env->GetStringUTFChars(romPath, nullptr);
    std::string pathStr = cstr;
    s_emulator->setROM(pathStr);
    env->ReleaseStringUTFChars(romPath, cstr);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_setBasePath(JNIEnv* env, jobject /* this */, jstring appDataPath)
{
    auto cstr = env->GetStringUTFChars(appDataPath, nullptr);;
    std::string pathStr = cstr;
    s_emulator->setBasePath(pathStr);
    env->ReleaseStringUTFChars(appDataPath, cstr);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_Renderer_runRenderer(JNIEnv* env, jobject obj)
{
    jclass rendererClass = env->GetObjectClass(obj);
    jmethodID method = env->GetMethodID(rendererClass, "renderEmulatorImage", "([B)V");

    if (s_emulator->hasNewImage())
    {
        auto image = s_emulator->getNewImage();
        const auto byteCount = image.size() * sizeof(ggb::RGB);

        std::vector<std::byte> bytes;
        bytes.resize(byteCount);
        memcpy(bytes.data(), image.data(), byteCount);
        jbyteArray array = env->NewByteArray(byteCount);
        env->SetByteArrayRegion(array, 0, byteCount, reinterpret_cast<jbyte*>(bytes.data()));

        env->CallVoidMethod(obj, method, array);
        env->DeleteLocalRef(array);
    }

    env->DeleteLocalRef(rendererClass);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_setButtonState(JNIEnv* env, jobject /* this */, jint buttonID, jboolean pressed)
{
    s_emulator->setButtonState(BUTTON(buttonID), pressed);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_autoSaveRAMAndRTC(JNIEnv* env, jobject /* this */)
{
    s_emulator->saveRAM();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_Renderer_updateMessages(JNIEnv *env, jobject /* this */)
{
    s_emulator->getMessageHandler()->updateMessages();
}

static jobjectArray getJavaArrayMessages(EmulatorMessage::MessageType type, JNIEnv* env)
{
    auto messages = s_emulator->getMessageHandler()->getMessages(type);
    jclass stringClass = env->FindClass("java/lang/String");
    jstring emptyString = env->NewStringUTF("");
    auto javaArray = env->NewObjectArray(messages.size(), stringClass, emptyString);
    env->DeleteLocalRef(stringClass);
    env->DeleteLocalRef(emptyString);

    for (size_t i = 0; i < messages.size(); i++)
    {
        jstring stringElement = env->NewStringUTF(messages[i].c_str());
        env->SetObjectArrayElement(javaArray, i, stringElement);
        env->DeleteLocalRef(stringElement);
    }

    return javaArray;
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_com_example_ggboy_Renderer_getInfos(JNIEnv *env, jobject /* this */)
{
    return getJavaArrayMessages(EmulatorMessage::Info, env);
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_com_example_ggboy_Renderer_getWarnings(JNIEnv *env, jobject /* this */)
{
    return getJavaArrayMessages(EmulatorMessage::Warning, env);
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_com_example_ggboy_Renderer_getErrors(JNIEnv *env, jobject /* this */)
{
    return getJavaArrayMessages(EmulatorMessage::Error, env);
}
extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_pauseEmulator(JNIEnv *env, jobject /* this */, jboolean pause)
{
    s_emulator->setPause(pause);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_saveSaveState(JNIEnv* env, jobject /* this */, jstring saveStatePath, jstring imagePath)
{
    auto cstr1 = env->GetStringUTFChars(saveStatePath, nullptr);
    auto cstr2 = env->GetStringUTFChars(imagePath, nullptr);;
    std::string saveState = cstr1;
    std::string image = cstr2;

    s_emulator->saveSaveStateAndLastImage(saveState, image);

    env->ReleaseStringUTFChars(saveStatePath, cstr1);
    env->ReleaseStringUTFChars(imagePath, cstr2);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_loadSaveState(JNIEnv* env, jobject /* this */, jstring saveStatePath)
{
    auto cstr = env->GetStringUTFChars(saveStatePath, nullptr);
    std::string saveState = cstr;
    s_emulator->loadSaveState(saveState);
    env->ReleaseStringUTFChars(saveStatePath, cstr);
}


extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_setEmulationSpeed(JNIEnv* env, jobject /* this */, jdouble emulationSpeed)
{
    s_emulator->setEmulationSpeed(emulationSpeed);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_setEmulatorVolume(JNIEnv* env, jobject /* this */, jint volume)
{
    s_emulator->setEmulatorVolume(volume);
}
