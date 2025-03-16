#include "InputHandler.hpp"

#include <iostream>

InputHandler::InputHandler()
{
}

bool InputHandler::isAPressed()
{
    return m_emulatorStates[BUTTON::A];
}

bool InputHandler::isBPressed()
{
    return m_emulatorStates[BUTTON::B];
}

bool InputHandler::isStartPressed()
{
    return m_emulatorStates[BUTTON::START];
}

bool InputHandler::isSelectPressed()
{
    return m_emulatorStates[BUTTON::SELECT];
}

bool InputHandler::isUpPressed()
{
    return m_emulatorStates[BUTTON::UP];
}

bool InputHandler::isDownPressed()
{
    return m_emulatorStates[BUTTON::DOWN];
}

bool InputHandler::isLeftPressed()
{
    return m_emulatorStates[BUTTON::LEFT];
}

bool InputHandler::isRightPressed()
{
    return m_emulatorStates[BUTTON::RIGHT];
}

void InputHandler::setButtonState(BUTTON button, bool pressed)
{
    std::scoped_lock lock(m_mutex);
    m_newStates[button] = pressed;
}

void InputHandler::updateButtonStates()
{
    std::scoped_lock lock(m_mutex);
    m_emulatorStates = m_newStates;
}
