#include "Interpolate.h"
#include "glm/gtc/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "glm/gtx/transform2.hpp"

#include <glm/gtx/vector_angle.hpp>


Interpolate::Interpolate()
{
}

//Simple LERP function (takes a start, end, and time, and sends back given position based on time)
glm::vec3 Interpolate::LERP(glm::vec3 start, glm::vec3 end, float time)
{
	return (start + time * (end - start));
}

//SLERP function, interpolated between start and end, giving back a vec3 based on given time
glm::vec3 Interpolate::SLERP(glm::vec3 start, glm::vec3 end, float time)
{

	float angle = glm::orientedAngle(start, end, glm::vec3(0.f, 1.0f, 0.f));

	glm::vec3 vec1 = ((glm::sin(1.0f - time)*angle) / glm::sin(angle)) * start;
	glm::vec3 vec2 = (glm::sin(time*angle) / glm::sin(angle)) * end;
	glm::vec3 vec = vec1 + vec2;

	return vec;
}

Interpolate::~Interpolate()
{
}
 