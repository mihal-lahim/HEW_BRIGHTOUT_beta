#include "EventSystem.h"

std::unordered_map<std::string, std::vector<EventSystem::CallbackEntry>>
EventSystem::m_Events;

EventSystem::HandleID EventSystem::m_NextID = 1;

EventSystem::HandleID EventSystem::Register(const std::string& eventName, Callback cb)
{
  HandleID id = m_NextID++;
    m_Events[eventName].push_back({ id, std::move(cb) });
    return id;
}

void EventSystem::Unregister(const std::string& eventName, HandleID handle)
{
    auto it = m_Events.find(eventName);
    if (it == m_Events.end())
   return;

    auto& vec = it->second;
    vec.erase(
    std::remove_if(vec.begin(), vec.end(),
            [handle](const CallbackEntry& entry) { return entry.id == handle; }),
        vec.end());
}

void EventSystem::Send(const std::string& eventName)
{
    auto it = m_Events.find(eventName);
    if (it == m_Events.end())
        return;

    for (auto& entry : it->second)
        entry.callback();
}

void EventSystem::ClearAll()
{
    m_Events.clear();
}