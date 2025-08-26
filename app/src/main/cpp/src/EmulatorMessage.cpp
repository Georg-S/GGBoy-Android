#include "EmulatorMessage.hpp"

void EmulatorMessageHandler::updateMessages()
{
    std::unique_lock lock(m_emulatorMessagesMutex);
    m_retrievableMessages = std::move(m_internalMessages);
    m_internalMessages.clear();
}

std::vector<std::string> EmulatorMessageHandler::getMessages(EmulatorMessage::MessageType type)
{
    std::vector<std::string> result;
    for (const auto& message : m_retrievableMessages)
    {
        if (type == message.type)
            result.emplace_back(message.message);
    }

    return result;
}

void EmulatorMessageHandler::addMessage(EmulatorMessage::MessageType type, const std::string &message)
{
    EmulatorMessage emuMessage = {};
    emuMessage.type = type;
    emuMessage.message = message;

    std::unique_lock lock(m_emulatorMessagesMutex);
    m_internalMessages.emplace_back(std::move(emuMessage));
}
