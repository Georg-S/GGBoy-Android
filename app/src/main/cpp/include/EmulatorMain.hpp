#pragma once
#include <cassert>
#include <iostream>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <Emulator.hpp>
#include <RenderingUtility.hpp>
#include <thread>

#include "Video.hpp"
#include "Audio.hpp"
#include "InputHandler.hpp"

class EmulatorMain
{
public:
    EmulatorMain();
    ~EmulatorMain();
    void run();
    void setROM(std::filesystem::path path);
    bool hasNewImage() const;
    std::vector<ggb::RGB> getNewImage();

private:
    void runInThread();
    void loadROM(const std::filesystem::path& path);

    std::thread m_emulatorThread;
    std::unique_ptr<ggb::Emulator> m_emulator = nullptr;
    std::unique_ptr<Audio> m_audioHandler = nullptr;
    InputHandler* m_inputHandler = nullptr;
    AndroidRenderer* m_androidRenderer = nullptr;
    bool m_quit = false;
    std::filesystem::path m_romToBeLoaded;
    std::mutex m_inputMutex;
    std::mutex m_emulatorEventsMutex;

};