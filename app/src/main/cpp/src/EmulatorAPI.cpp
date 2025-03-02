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
