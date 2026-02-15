#pragma once

#include <Input.hpp>
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

class InputHandler
{
public:
    InputHandler() = default;
    void setButtonState(BUTTON button, bool pressed);
    ggb::GameboyInput getButtonState();

private:
    std::mutex m_mutex;
    std::unordered_map<BUTTON, bool> m_newStates;
};