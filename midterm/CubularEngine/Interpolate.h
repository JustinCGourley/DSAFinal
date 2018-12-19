#pragma once
#include "stdafx.h"

class Interpolate
{
public:
	Interpolate();
	~Interpolate();

	glm::vec3 LERP(glm::vec3 start, glm::vec3 end, float time);
	glm::vec3 SLERP(glm::vec3 start, glm::vec3 end, float time);
};

