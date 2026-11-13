
#ifndef LOADING_MOVIE_SCENE_H
#define LOADING_MOVIE_SCENE_H

#include "Scene.h"

class LoadingMovieScene : public Scene
{
public:
	void Initialize() override;
	void Finalize() override;
};

#endif
