#include "EmulatorMain.hpp"

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

EmulatorMain::EmulatorMain()
{
    m_emulator = std::make_unique<ggb::Emulator>();
    m_audioHandler = std::make_unique<Audio>(m_emulator->getSampleBuffer());
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

//    saveCartridgeRAM();
//    saveCartridgeRTC();
}

void EmulatorMain::loadROM(const std::filesystem::path& path)
{
//    saveCartridgeRAM();
//    saveCartridgeRTC();

    try
    {
        m_emulator->loadCartridge(path);
    }
    catch (const std::exception& e)
    {
        // TODO handle errors (maybe a message can be send to java part of the emulator?)
    }

//    loadRAM();
//    loadRTC();
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


