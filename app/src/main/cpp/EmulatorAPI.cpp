#include <jni.h>
#include <string>
#include <filesystem>
#include <Emulator.hpp>

static std::unique_ptr<ggb::Emulator> s_emulator = nullptr;

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_initEmulator(JNIEnv* env, jobject /* this */)
{
    s_emulator = std::make_unique<ggb::Emulator>();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ggboy_MainActivity_loadROM(JNIEnv* env, jobject /* this */, jstring romPath)
{
    std::string pathStr = env->GetStringUTFChars(romPath, nullptr);

    bool test = std::filesystem::exists(pathStr);
    s_emulator->loadCartridge(pathStr);
}
