#include "Interpolate.h"



Interpolate::Interpolate()
{
}


glm::vec3 Interpolate::LERP(glm::vec3 start, glm::vec3 end, float time)
{
	return (start + time * (end - start));
}

glm::vec3 Interpolate::SLERP(glm::vec3 start, glm::vec3 end, float time)
{
	float dot = glm::dot(start, end);
	//std::cout << "Start: " << start.y << std::endl;
	//std::cout << "End: " << end.y << "\n";
	//std::cout << "dot: " << dot << "\n";
	dot = glm::clamp(dot, -1.f, 1.f);
	//std::cout << "clamed dot now: " << dot << "\n";
	float theta = glm::acos(dot) * time;
	//std::cout << "theta: " << theta << "\n";
	glm::vec3 relVec = (end - start) * dot;
	//std::cout << "relVec: " << relVec.x << "\n";
	relVec = glm::normalize(relVec);
	//std::cout << "normalizedRelVec: " << relVec.x << "\n";
	//std::cout << "outPutx: " << ((start * cos(theta)) + (relVec * sin(theta))).x << "\n";
	return ((start * cos(theta)) + (relVec * sin(theta)));
}

Interpolate::~Interpolate()
{
}
 