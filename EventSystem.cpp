#include "EventSystem.h"

std::unordered_map<std::string, std::vector<EventSystem::Callback>>
EventSystem::m_Events;

void EventSystem::Register(const std::string& eventName, Callback cb)
{
    m_Events[eventName].push_back(cb);
}

void EventSystem::Send(const std::string& eventName)
{
    if (m_Events.find(eventName) == m_Events.end())
        return;

    for (auto& cb : m_Events[eventName])
        cb();
}