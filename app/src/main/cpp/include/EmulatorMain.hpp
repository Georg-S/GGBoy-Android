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
#include "EmulatorMessage.hpp"

class EmulatorMain
{
public:
    EmulatorMain();
    ~EmulatorMain();
    void run();
    void setROM(std::filesystem::path path);
    void setBasePath(std::filesystem::path path);
    bool hasNewImage() const;
    std::vector<ggb::RGB> getNewImage();
    void setButtonState(BUTTON buttonID, bool pressed);
    void saveRAM();
    EmulatorMessageHandler* getMessageHandler();
    void setPause(bool pause);
    void saveSaveStateAndLastImage(std::string saveStatePath, std::string imagePath);
    void loadSaveState(std::string saveStatePath);

private:
    void runInThread();
    void loadROM(const std::filesystem::path& path);
    std::string getCartridgeName();
    std::filesystem::path getFileSavePath(const std::string& fileName, const std::string& fileExtension);
    void autoSaveCartridgeRAM();
    void autoSaveCartridgeRTC();
    void loadAutoSaveRAM();
    void loadAutoSaveRTC();

    std::thread m_emulatorThread;
    std::unique_ptr<EmulatorMessageHandler> m_messageHandler = nullptr;
    std::unique_ptr<ggb::Emulator> m_emulator = nullptr;
    std::unique_ptr<Audio> m_audioHandler = nullptr;
    InputHandler* m_inputHandler = nullptr;
    AndroidRenderer* m_androidRenderer = nullptr;
    std::atomic<bool> m_quit = false;
    std::atomic<bool> m_saveRAMAndRTC = false;
    bool m_pauseRequest = false;
    bool m_pauseValue = false;
    std::filesystem::path m_romToBeLoaded;
    std::filesystem::path m_basePath;
    std::filesystem::path m_toSaveSaveStatePath;
    std::filesystem::path m_toSaveImagePath;
    std::filesystem::path m_toLoadSaveState;
    std::mutex m_inputMutex;
    std::mutex m_emulatorEventsMutex;
};