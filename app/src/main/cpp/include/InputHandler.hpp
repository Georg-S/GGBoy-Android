#pragma once

#include <Input.hpp>
#include <Emulator.hpp>
#include <mutex>
#include <unordered_map>

enum class BUTTON
{
    A = 0,
    B = 1,
    START = 2,
    SELECT = 3,
    LEFT= 4,
    RIGHT = 5,
    UP = 6,
    DOWN = 7,
};

class InputHandler : public ggb::Input
{
public:
    InputHandler() = default;

    bool isAPressed() override;
    bool isBPressed() override;
    bool isStartPressed() override;
    bool isSelectPressed() override;
    bool isUpPressed() override;
    bool isDownPressed() override;
    bool isLeftPressed() override;
    bool isRightPressed() override;
    void setButtonState(BUTTON button, bool pressed);
    void updateButtonStates();

private:
    bool m_a = false;
    bool m_b = false;
    bool m_start = false;
    bool m_select = false;
    bool m_up = false;
    bool m_down = false;
    bool m_left = false;
    bool m_right = false;
    std::mutex m_mutex;
    std::unordered_map<BUTTON, bool> m_emulatorStates;
    std::unordered_map<BUTTON, bool> m_newStates;
};