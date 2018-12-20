#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Input.h"

Camera::Camera(glm::vec3 position,
    glm::vec3 forward, 
    glm::vec3 up, 
    float fovAngleY, 
    float width, 
    float height,
    float nearZ, 
    float farZ,
	GLFWwindow* window,
	bool controllable)
{
    this->position = position;
    this->forward = forward; 
    this->up = up;
	
    //does this calculation once
    this->fov = fovAngleY * glm::pi<float>() / 180.0f;
    this->width = width;
    this->height = height;
    this->nearZ = nearZ;
    this->farZ = farZ;
	this->finalX = 0;
	this->finalY = 0;
	this->lastX = 0;
	this->lastY = 0;
	this->start = true;
	this->window = window;
	this->yaw = 90;
	this->pitch = 0;
	this->speed = 1;
	this->sensativity = 0.04f;
	this->controllable = controllable;
}

Camera::~Camera()
{
}

//Update the camera
void Camera::Update()
{
	if (controllable)//if controllable activate all the controls for the camera
	{
		//update played based movement for the camera
		if (Input::GetInstance()->IsKeyDown(GLFW_KEY_A))
		{
			position -= right * speed;
		}
		else if (Input::GetInstance()->IsKeyDown(GLFW_KEY_D))
		{
			position += right * speed;
		}
		if (Input::GetInstance()->IsKeyDown(GLFW_KEY_W))
		{
			position += forward * speed;
		}
		else if (Input::GetInstance()->IsKeyDown(GLFW_KEY_S))
		{
			position -= forward * speed;
		}
		if (Input::GetInstance()->IsKeyDown(GLFW_KEY_E))
		{
			position += up * speed;
		}
		else if (Input::GetInstance()->IsKeyDown(GLFW_KEY_Q))
		{
			position -= up * speed;
		}


		//mouse movement setting pitch and yaw
		if (start)
		{
			double *getX = &lastX;
			double *getY = &lastY;
			glfwGetCursorPos(window, getX, getY);

			start = false;
		}

		double *getX = &finalX;
		double *getY = &finalY;
		glfwGetCursorPos(window, getX, getY);

		float xDistance = lastX - finalX;
		float yDistance = lastY - finalY;

		xDistance *= sensativity;
		yDistance *= sensativity;

		pitch += yDistance;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		yaw += xDistance;

		//setting forward vec based on pitch and yaw
		forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward.y = sin(glm::radians(pitch));
		forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		lastX = finalX;
		lastY = finalY;
	}

    //this call may not be needed every frame
    forward = glm::normalize<3>(forward);   

	right = glm::cross(up, forward);

    //view matrix
    //we use forward here instead of a lookTarget because it's easier to track
    //and conceptualize.
    viewMatrix = glm::lookAtLH(
        position,               //where the camera is
        position + forward,     //where the camera is looking at 
        up                      //what is 'up' for the camera
    );

    //create the projection matrix
    projectionMatrix = glm::perspectiveFovLH<GLfloat>(
        fov,
        width,                  //the width of the window in float
        height,                 //the height of the window in float
        nearZ,                  //the near Z-plane
        farZ                    //the far Z-plane
    );
}
