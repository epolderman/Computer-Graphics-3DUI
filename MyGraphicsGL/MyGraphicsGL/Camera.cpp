#include "Camera.h"
#include <glm\gtx\transform.hpp>

//vDirection(-0.588254f, -0.805122f, -0.610186f),
//theUpVector(0.0f, 1.0f, 0.0f),
//position(2.18f, 1.715f, 1.4956f){}

Camera::Camera() :
vDirection(-0.588254f, -0.805122f, -0.610186f),
theUpVector(0.0f, 1.0f, 0.0f),
position(-2.0f, 4.0f, -2.0f){}

glm::vec3 Camera::getCameraPosition() const
{
	return position;
}
glm::vec3 Camera::getViewDirection() const
{
	return vDirection;
}
glm::mat4 Camera::getWorldtoViewMatrix() const
{
	//position(camera)
	//center(position camera vector + view vector) = look at point(vector addition) 1 unit ahead of view vector
	//up vector
	return(glm::lookAt(position, position + vDirection, theUpVector));
}

void Camera::theMouseUpdate(const glm::vec2& newMousePosition)
{
	//rotation left and right(horizontel)
	//rotating our view vector around the up vector
	//mouse delta 2Dimensional(new vector - old vector)
	glm::vec2 mouseDelta = newMousePosition - lastMousePosition;
	if (glm::length(mouseDelta) > 50.0f)
	{
		lastMousePosition = newMousePosition;
		return;
	}
	//mouse.x(rotation angle 90 px for a 90 degree turn), 
	//vector we are rotating around) * (apply it to our view direction) matrix * our view direction
	vDirection = glm::mat3(glm::rotate(-mouseDelta.x * ROTATION_SPEED, theUpVector)) * vDirection;

	//vertical
	//rotation up and down
	//cross product gives us the perpindicular vector of the View direction X theUpVector
	//then we can rotate around the perpindicular vector
	strafeDirection = glm::cross(vDirection, theUpVector);
	vDirection = glm::mat3(glm::rotate(-mouseDelta.y * ROTATION_SPEED, strafeDirection)) * vDirection;

	//updating the up vector to maintain the 90 degree view direction and up vector 
	//a lot of time our up vector is being projected onto our camera's up vector 
	theUpVector = glm::mat3(glm::rotate(-mouseDelta.y * ROTATION_SPEED, theUpVector)) * theUpVector;
	
	lastMousePosition = newMousePosition;


}
void Camera::moveForward()
{	//scaling the v direction vector -> by .1 then adding it to the position so you move
	position += MOVEMENT_SPEED * vDirection;
}
void Camera::moveBackward()
{
	position += -MOVEMENT_SPEED * vDirection;
}
void Camera::moveLeft()
{
	//scaling the perpindicular vector by .1 then adding to the position so you move left
	position += -MOVEMENT_SPEED * strafeDirection;
}
void Camera::moveRight()
{
	position += MOVEMENT_SPEED * strafeDirection;
}
void Camera::moveUp()
{
	position += MOVEMENT_SPEED * theUpVector;
}
void Camera::moveDown()
{
	position += -MOVEMENT_SPEED * theUpVector;
}