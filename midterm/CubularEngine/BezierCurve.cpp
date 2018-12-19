#include "BezierCurve.h"



BezierCurve::BezierCurve(glm::vec2 start, glm::vec2 end, glm::vec2 controlPointStart, glm::vec2 controlPointEnd)
{

	this->startPoint = controlPointStart;
	this->endPoint = controlPointEnd;
	this->controlPointStart = start;
	this->controlPointEnd = end;

}

float BezierCurve::CalculatePoint(float p1, float p2, float t)
{
	float diff = p2 - p1;

	return p1 + (diff * t);
}

glm::vec2 BezierCurve::GetPoint(float t)
{
	//outer values

	float xa = CalculatePoint(controlPointStart.x, startPoint.x, t);
	float ya = CalculatePoint(controlPointStart.y, startPoint.y, t);

	float xb = CalculatePoint(startPoint.x, endPoint.x, t);
	float yb = CalculatePoint(startPoint.y, endPoint.y, t);

	float xc = CalculatePoint(endPoint.x, controlPointEnd.x, t);
	float yc = CalculatePoint(endPoint.y, controlPointEnd.y, t);

	//inner values

	float xi = CalculatePoint(xa, xb, t);
	float yi = CalculatePoint(ya, yb, t);

	float xj = CalculatePoint(xb, xc, t);
	float yj = CalculatePoint(yb, yc, t);

	//end values
	float finalX = CalculatePoint(xi, xj, t);
	float finalY = CalculatePoint(yi, yj, t);

	return glm::vec2(finalX, finalY);
}

BezierCurve::~BezierCurve()
{
}
