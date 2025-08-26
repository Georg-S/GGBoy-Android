#pragma once
#include <vector>

struct EmulatorMessage
{
    enum MessageType
    {
        Info, Warning, Error
    };

    MessageType type = MessageType::Info;
    std::string message;
};

class EmulatorMessageHandler
{
public:
    // Needs to be called before "getMessages" to retrieve the current messages
    void updateMessages();
    // Retrieves the messages for the specified type.
    // "updateMessages" needs to be called to retrieve the newest messages
    std::vector<std::string> getMessages(EmulatorMessage::MessageType type);
    void addMessage(EmulatorMessage::MessageType type, const std::string& message);

private:
    // Two messages objects, to prevent blocking the thread to often
    // Only block once to retrieve / update all types
    std::vector<EmulatorMessage> m_retrievableMessages;
    std::vector<EmulatorMessage> m_internalMessages;
    std::mutex m_emulatorMessagesMutex;
};