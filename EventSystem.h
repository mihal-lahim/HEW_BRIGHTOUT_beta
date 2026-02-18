#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

class EventSystem
{
public:
    using Callback = std::function<void()>;

    static void Register(const std::string& eventName, Callback cb);
    static void Send(const std::string& eventName);

private:
    static std::unordered_map<std::string, std::vector<Callback>> m_Events;
};