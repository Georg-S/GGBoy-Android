#include "EmulatorMain.hpp"

#include <regex>

static std::filesystem::path cartridgePath = "";
static const std::filesystem::path SAVE_STATE_BASE_PATH = "Savestates/";
static const std::filesystem::path RAM_BASE_PATH = "RAM/";
static const std::filesystem::path CARTRIDGE_DATA_BASE_PATH = "CARTRIDGE_DATA/";
static const std::filesystem::path GAMES_BASE_PATH = "Roms/Games/";
static const std::string RAM_FILE_ENDING = ".bin";
static const std::string RAM_FILE_SUFFIX = "_ram";
static const std::string RTC_FILE_SUFFIX = "_RTC";
static const std::string SAVESTATE_FILE_ENDING = ".bin";

namespace
{
    class Timer
    {
    public:
        Timer(long long timeout, std::function<void()> timeoutCallback)
        {
            m_timeout = timeout;
            m_timeoutCallback = std::move(timeoutCallback);
        }

        void update(long long duration)
        {
            m_counter += duration;
            if (m_counter < m_timeout)
                return;

            m_counter -= m_timeout;
            m_timeoutCallback();
        };

    private:
        long long m_counter = 0;
        long long m_timeout = 0;
        std::function<void()> m_timeoutCallback;
    };
}

template <typename TP>
static std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());

    return system_clock::to_time_t(sctp);
}

static bool createDirectoriesForFilePath(const std::filesystem::path& filePath)
{
    auto dir = filePath.parent_path();
    std::error_code ec;
    if (!std::filesystem::exists(filePath, ec))
    {
        std::filesystem::create_directories(dir, ec);
        if (ec)
            return false;
    }

    return std::filesystem::is_directory(dir, ec);
}

EmulatorMain::EmulatorMain()
{
    m_messageHandler = std::make_unique<EmulatorMessageHandler>();
    m_emulator = std::make_unique<ggb::Emulator>();
    m_audioHandler = std::make_unique<Audio>(m_emulator->getSampleBuffer(), m_messageHandler.get());
    auto inputHandler = std::make_unique<InputHandler>();
    m_inputHandler = inputHandler.get();

    auto tileDataDimensions = m_emulator->getTileDataDimensions();
    auto gameWindowDimensions = m_emulator->getGameWindowDimensions();
    //m_tileDataRenderer = std::make_unique<SDLRenderer>(tileDataDimensions.width, tileDataDimensions.height, 4);
    auto gameRenderer = std::make_unique<AndroidRenderer>(gameWindowDimensions.width, gameWindowDimensions.height);
    m_androidRenderer = gameRenderer.get();

    m_emulator->setGameRenderer(std::move(gameRenderer));
    m_emulator->setInput(std::move(inputHandler));
}

EmulatorMain::~EmulatorMain()
{
    m_quit = true;
    if (m_emulatorThread.joinable())
        m_emulatorThread.join();
}

void EmulatorMain::run()
{
    m_emulatorThread = std::thread(&EmulatorMain::runInThread, this);
}

void EmulatorMain::setROM(std::filesystem::path path)
{
    std::scoped_lock lock(m_emulatorEventsMutex);
    m_romToBeLoaded = std::move(path);
}

void EmulatorMain::setBasePath(std::filesystem::path path)
{
    std::scoped_lock lock(m_emulatorEventsMutex);
    m_basePath = std::move(path);
}

void EmulatorMain::runInThread()
{
    static constexpr long long NANO_SECONDS_PER_SECOND = 1000000000;
    static constexpr int UPDATE_AFTER_STEPS = 100;

    bool running = true;

    auto inputTimer = Timer(NANO_SECONDS_PER_SECOND / 100, [this]()
    {
        m_inputHandler->updateButtonStates();
        m_inputHandler->update();
    });

    auto emulatorEventsTimer = Timer(NANO_SECONDS_PER_SECOND / 3, [this, &running]()
    {
        std::scoped_lock lock(m_emulatorEventsMutex);
        running = !m_quit;
        if (m_saveRAMAndRTC)
        {
            autoSaveCartridgeRAM();
            autoSaveCartridgeRTC();
            m_saveRAMAndRTC = false;
        }
        if (m_emulationSpeed)
        {
            m_emulator->setEmulationSpeed(*m_emulationSpeed);
            m_emulationSpeed.reset();
        }
        if (!m_toSaveSaveStatePath.empty())
        {
            assert(!m_toSaveImagePath.empty());
            bool saveStateWritten = false;
            if (!createDirectoriesForFilePath(m_toSaveSaveStatePath))
                m_messageHandler->addMessage(EmulatorMessage::Warning, "Unable to create directories for save state file");
            else
                saveStateWritten = m_emulator->saveEmulatorState(m_toSaveSaveStatePath);

            if (!saveStateWritten)
            {
                m_messageHandler->addMessage(EmulatorMessage::Warning, "Was not able to write the save state");
            }
            else
            {
                if (!createDirectoriesForFilePath(m_toSaveImagePath))
                    m_messageHandler->addMessage(EmulatorMessage::Warning, "Unable to create directories for save state image file");
                else if (!m_androidRenderer->writeLastImage(m_toSaveImagePath))
                        m_messageHandler->addMessage(EmulatorMessage::Warning, "Was not able to write the save state image");
            }

            m_toSaveSaveStatePath.clear();
            m_toSaveImagePath.clear();
        }
        if (!m_toLoadSaveState.empty())
        {
            if (!m_emulator->loadEmulatorState(m_toLoadSaveState))
                m_messageHandler->addMessage(EmulatorMessage::Warning, "Was not able to load save state");
            m_toLoadSaveState.clear();
        }
        if (m_pauseRequest)
        {
            m_pauseRequest = false;
            if (m_pauseValue)
            {
                m_emulator->pause();
                m_audioHandler->setAudioPlaying(false);
            }
            else
            {
                m_emulator->resume();
                m_audioHandler->setAudioPlaying(true);
            }
        }
        if (m_romToBeLoaded.empty())
            return;
        loadROM(m_romToBeLoaded);
        m_romToBeLoaded.clear();
    });

    long long lastTimeStamp = ggb::getCurrentTimeInNanoSeconds();
    int stepCounter = 0;
    while (running)
    {
        if (!m_emulator->isCartridgeLoaded())
        {
            const auto currentTime = ggb::getCurrentTimeInNanoSeconds();
            const auto timePast = currentTime - lastTimeStamp;
            emulatorEventsTimer.update(timePast);
            continue;
        }

        m_emulator->step();

        stepCounter++;
        if (stepCounter < UPDATE_AFTER_STEPS)
            continue;

        stepCounter = 0;
        const auto currentTime = ggb::getCurrentTimeInNanoSeconds();
        const auto timePast = currentTime - lastTimeStamp;
        lastTimeStamp = currentTime;

        inputTimer.update(timePast);
        emulatorEventsTimer.update(timePast);
    }

    autoSaveCartridgeRTC();
    autoSaveCartridgeRAM();
}

void EmulatorMain::loadROM(const std::filesystem::path& path)
{
    autoSaveCartridgeRAM();
    autoSaveCartridgeRTC();

    try
    {
        m_emulator->loadCartridge(path);
        m_messageHandler->addMessage(EmulatorMessage::Info, "ROM loaded");
    }
    catch (const std::exception& e)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error loading ROM: " + std::string(e.what()));
    }

    loadAutoSaveRAM();
    loadAutoSaveRTC();
}

bool EmulatorMain::hasNewImage() const
{
    return m_androidRenderer->hasNewImage();
}

std::vector<ggb::RGB> EmulatorMain::getNewImage()
{
    return m_androidRenderer->getCurrentFrame();
}

void EmulatorMain::setButtonState(BUTTON buttonID, bool pressed)
{
    m_inputHandler->setButtonState(buttonID, pressed);
}

void EmulatorMain::saveRAM()
{
    m_saveRAMAndRTC = true;
}

void EmulatorMain::setPause(bool pause)
{
    std::scoped_lock lock(m_emulatorEventsMutex);
    m_pauseRequest = true;
    m_pauseValue = pause;
}

void EmulatorMain::saveSaveStateAndLastImage(std::string saveStatePath, std::string imagePath)
{
    std::scoped_lock lock(m_emulatorEventsMutex);
    m_toSaveSaveStatePath = std::move(saveStatePath);
    m_toSaveImagePath = std::move(imagePath);
}

void EmulatorMain::loadSaveState(std::string saveStatePath)
{
    std::scoped_lock lock(m_emulatorEventsMutex);
    m_toLoadSaveState = std::move(saveStatePath);
}

void EmulatorMain::setEmulationSpeed(double emulationSpeed)
{
    std::scoped_lock lock(m_emulatorEventsMutex);
    m_emulationSpeed = emulationSpeed;
}


std::string EmulatorMain::getCartridgeName()
{
    auto loadedPath = m_emulator->getLoadedCartridgePath();
    if (loadedPath.empty())
        return {};

    auto gameName = loadedPath.filename().stem().u8string();
    if (gameName.empty())
    {
        m_messageHandler->addMessage(EmulatorMessage::Warning, "Unable to get cartridge name, "
                                             "even though a cartridge is loaded");
        return {};
    }

    return gameName;
}

static std::string regexEscape(const std::string& str)
{
    static const std::regex specialChars{ R"([-[\]{}()*+?.,\^$|#\s])" };

    return std::regex_replace(str, specialChars, R"(\$&)");
}

/// Returns the oldest / newest file given the specified path and name (only files with '<name>1', '<name>2' etc. are used)
static std::vector<std::pair<std::filesystem::path, std::time_t>> getFilePaths(const std::filesystem::path& base, const std::string& fileName)
{
    if (!std::filesystem::is_directory(base))
        return {};

    std::filesystem::path result = {};

    std::vector<std::pair<std::filesystem::path, std::time_t>> files;
    for (const auto& entry : std::filesystem::directory_iterator(base))
    {
        if (!std::filesystem::is_regular_file(entry))
            continue;

        const auto currentFileName = entry.path().stem().u8string();
        const auto regexString = regexEscape(fileName) + "\\d+";
        if (!std::regex_match(currentFileName, std::regex(regexString)))
            continue;

        const auto lastEditedTime = to_time_t(std::filesystem::last_write_time(entry));
        auto pair = std::make_pair(entry.path(), lastEditedTime);
        files.emplace_back(std::move(pair));
    }

    std::sort(files.begin(), files.end(), [](const std::pair<std::filesystem::path, std::time_t>& lhs, const std::pair<std::filesystem::path, std::time_t>& rhs)
    {
        return lhs.second > rhs.second;
    });

    if (files.empty())
        return {};

    return files;
}


std::filesystem::path EmulatorMain::getFileSavePath(const std::string& fileName, const std::string& fileExtension)
{
    auto gameName = getCartridgeName();
    if (gameName.empty())
        return {};

    std::filesystem::path path = m_basePath / CARTRIDGE_DATA_BASE_PATH;
    try
    {
        if (!std::filesystem::exists(m_basePath / CARTRIDGE_DATA_BASE_PATH))
            std::filesystem::create_directories(m_basePath / CARTRIDGE_DATA_BASE_PATH);

        auto paths = getFilePaths(m_basePath / CARTRIDGE_DATA_BASE_PATH, gameName + fileName);
        if (paths.size() <= 1)
        {
            auto toTestPath = m_basePath / CARTRIDGE_DATA_BASE_PATH / (gameName + (fileName + "0") + RAM_FILE_ENDING);
            if (!paths.empty() && (paths.front().first == toTestPath))
                path /= gameName + (fileName + "1") + RAM_FILE_ENDING;
            else
                path /= gameName + (fileName + "0") + RAM_FILE_ENDING;
        }
        else
        {
            // More than one file, use the oldest
            path = paths.back().first;
        }
        return path;
    }
    catch (const std::exception& e)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error finding a valid file path: " + std::string(e.what()));
    }

    return {};
}


void EmulatorMain::autoSaveCartridgeRAM()
{
    auto pathToWrite = getFileSavePath(RAM_FILE_SUFFIX, RAM_FILE_ENDING);
    if (pathToWrite.empty())
        return;

    try
    {
        m_emulator->saveRAM(pathToWrite);
        m_messageHandler->addMessage(EmulatorMessage::Info, "RAM saved");
    }
    catch (const std::exception& e)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error saving RAM: " + std::string(e.what()));
    }
}

void EmulatorMain::autoSaveCartridgeRTC()
{
    auto pathToWrite = getFileSavePath(RTC_FILE_SUFFIX, RAM_FILE_ENDING);
    if (pathToWrite.empty())
        return;

    try
    {
        m_emulator->saveRTC(pathToWrite);
    }
    catch (const std::exception& e)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error saving real time clock: " + std::string(e.what()));
    }
}

void EmulatorMain::loadAutoSaveRAM()
{
    auto gameName = getCartridgeName();
    if (gameName.empty())
        return;

    std::filesystem::path path = CARTRIDGE_DATA_BASE_PATH;
    auto paths = getFilePaths(m_basePath / CARTRIDGE_DATA_BASE_PATH, gameName + RAM_FILE_SUFFIX);
    if (paths.empty())
        return;

    const auto& pathToLoad = paths.front().first;
    try
    {
        m_emulator->loadRAM(pathToLoad);
    }
    catch (const std::exception& e)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error loading RAM: " + std::string(e.what()));
    }
}

void EmulatorMain::loadAutoSaveRTC()
{
    auto gameName = getCartridgeName();
    if (gameName.empty())
        return;

    std::filesystem::path path = CARTRIDGE_DATA_BASE_PATH;
    auto paths = getFilePaths(m_basePath/ CARTRIDGE_DATA_BASE_PATH, gameName + RTC_FILE_SUFFIX);
    if (paths.empty())
        return;
    const auto& pathToLoad = paths.front().first;

    try
    {
        m_emulator->loadRTC(pathToLoad);
    }
    catch (const std::exception& e)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error loading real time clock: " + std::string(e.what()));
    }
}

EmulatorMessageHandler *EmulatorMain::getMessageHandler()
{
    return m_messageHandler.get();
}
