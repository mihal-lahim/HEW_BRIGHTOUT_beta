
#ifndef RESULT_H
#define RESULT_H

#include "Scene.h"

class Result : public Scene
{
public:
	void Initialize() override;
	void Finalize() override;
};

#endif
