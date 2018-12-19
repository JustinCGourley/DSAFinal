#pragma once
#include "stdafx.h"

class BezierCurve
{
private:

	float CalculatePoint(float p1, float p2, float t);


public:
	BezierCurve(glm::vec2 startPoint, glm::vec2 endPoint, glm::vec2 controlPointStart, glm::vec2 controlPointEnd);
	~BezierCurve();

	glm::vec2 startPoint;
	glm::vec2 endPoint;
	glm::vec2 controlPointStart;
	glm::vec2 controlPointEnd;

	std::vector<glm::vec2> curve;

	glm::vec2 GetPoint(float t);
};

