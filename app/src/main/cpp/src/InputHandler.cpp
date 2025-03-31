#include "InputHandler.hpp"

#include <iostream>

bool InputHandler::isAPressed()
{
    return m_a;
}

bool InputHandler::isBPressed()
{
    return m_b;
}

bool InputHandler::isStartPressed()
{
    return m_start;
}

bool InputHandler::isSelectPressed()
{
    return m_select;
}

bool InputHandler::isUpPressed()
{
    return m_up;
}

bool InputHandler::isDownPressed()
{
    return m_down;
}

bool InputHandler::isLeftPressed()
{
    return m_left;
}

bool InputHandler::isRightPressed()
{
    return m_right;
}

void InputHandler::setButtonState(BUTTON button, bool pressed)
{
    std::scoped_lock lock(m_mutex);
    m_newStates[button] = pressed;
}

void InputHandler::updateButtonStates()
{
    std::scoped_lock lock(m_mutex);
    m_a = m_newStates[BUTTON::A];
    m_b = m_newStates[BUTTON::B];
    m_start = m_newStates[BUTTON::START];
    m_select = m_newStates[BUTTON::SELECT];
    m_up = m_newStates[BUTTON::UP];
    m_down = m_newStates[BUTTON::DOWN];
    m_left = m_newStates[BUTTON::LEFT];
    m_right = m_newStates[BUTTON::RIGHT];
}
