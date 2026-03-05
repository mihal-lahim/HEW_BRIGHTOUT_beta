#ifndef RESULT_H
#define RESULT_H

#include "Scene.h"
#include <string>

class Result : public Scene
{
public:
	void Initialize() override;
	void Finalize() override;
};

// 設定された場合、Result シーンで再生する BGM を変更するためのヘルパ
void SetResultBgm(const std::string& path);

#endif
