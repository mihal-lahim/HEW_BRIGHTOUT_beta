#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H


#include <unordered_map>
#include "input_device.h"
#include "command_set.h"
#include "Component.h"


class InputSystem : public Component
{
private:
	// 入力デバイス
	InputDevice* m_InputDevice = nullptr;

	// コマンドセット
	CommandSet* m_CommandSet = nullptr;
public:
	// コンストラクタ・デストラクタ
	InputSystem(InputDevice* inputDevice, CommandSet* commandSet = nullptr)
		: m_InputDevice(inputDevice), m_CommandSet(commandSet)
	{}
	~InputSystem() = default;

	// デバイス設定
	void SetDevice(InputDevice* inputDevice) { m_InputDevice = inputDevice; }

	// 入力デバイスの設定
	void SetInputDevice(InputDevice* inputDevice) { m_InputDevice = inputDevice; }

	// コマンドセットの設定
	void SetCommandSet(CommandSet* commandSet) { m_CommandSet = commandSet; }

	// 登録済みのコマンドかどうかを確認
	template<CommandType COM>
	bool IsRegistered() const { return m_CommandSet ? m_CommandSet->IsRegistered<COM>() : false; }

	// コマンドの数値を取得
	template<CommandType COM>
	float GetValue() const;

	// コマンドが発行されているかどうかを取得
	template<CommandType COM>
	bool IsIssued() const { return GetValue<COM>() != 0.0f; }
};


template<CommandType COM>
inline float InputSystem::GetValue() const
{
	// コマンドセットが設定されていなければ0を返す
	if (!m_CommandSet) return 0.0f;

	// コマンド情報を取得
	const auto* info = m_CommandSet->GetCommandInfo<COM>();

	// 登録されていれば値を返す
	if (info)
		return m_InputDevice->GetInputValue(info->first, info->second);

	// 未登録の場合は0を返す
	return 0.0f;
}

#endif
