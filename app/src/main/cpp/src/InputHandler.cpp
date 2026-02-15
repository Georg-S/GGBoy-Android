#include "InputHandler.hpp"

#include <iostream>

void InputHandler::setButtonState(BUTTON button, bool pressed)
{
    std::scoped_lock lock(m_mutex);
    m_newStates[button] = pressed;
}

ggb::GameboyInput InputHandler::getButtonState()
{
    ggb::GameboyInput result = {};

    std::scoped_lock lock(m_mutex);
    result.isAPressed = m_newStates[BUTTON::A];
    result.isBPressed = m_newStates[BUTTON::B];
    result.isStartPressed = m_newStates[BUTTON::START];
    result.isSelectPressed = m_newStates[BUTTON::SELECT];
    result.isUpPressed = m_newStates[BUTTON::UP];
    result.isDownPressed = m_newStates[BUTTON::DOWN];
    result.isLeftPressed = m_newStates[BUTTON::LEFT];
    result.isRightPressed = m_newStates[BUTTON::RIGHT];

    return result;
}
