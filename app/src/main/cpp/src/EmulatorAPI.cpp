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
    std::string pathStr = env->GetStringUTFChars(romPath, nullptr);
    s_emulator->setROM(pathStr);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_setBasePath(JNIEnv* env, jobject /* this */, jstring appDataPath)
{
    std::string pathStr = env->GetStringUTFChars(appDataPath, nullptr);
    s_emulator->setBasePath(pathStr);
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
    auto javaArray = env->NewObjectArray(messages.size(),
                                         env->FindClass("java/lang/String"),
                                         env->NewStringUTF(""));
    for (size_t i = 0; i < messages.size(); i++)
    {
        env->SetObjectArrayElement(javaArray, i, env->NewStringUTF(messages[i].c_str()));
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