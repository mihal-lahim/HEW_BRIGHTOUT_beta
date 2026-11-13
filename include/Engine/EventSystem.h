#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

class EventSystem
{
public:
    using Callback = std::function<void()>;

    // イベントハンドルID（登録解除に使用）
    using HandleID = uint64_t;

    static HandleID Register(const std::string& eventName, Callback cb);
    static void Unregister(const std::string& eventName, HandleID handle);
    static void Send(const std::string& eventName);
    static void ClearAll();

private:
    struct CallbackEntry
    {
        HandleID id;
        Callback callback;
    };

    static std::unordered_map<std::string, std::vector<CallbackEntry>> m_Events;
    static HandleID m_NextID;
};