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
