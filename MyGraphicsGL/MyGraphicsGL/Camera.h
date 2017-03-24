#ifndef CAMERA_H
#define CAMERA_H
#include <glm\glm.hpp>
class Camera
{

private:
	glm::vec3 vDirection; //view direction
	glm::vec3 theUpVector;
	glm::vec2 lastMousePosition;
	const float MOVEMENT_SPEED = 0.9f;
	const float ROTATION_SPEED = 0.2f;
	glm::vec3 strafeDirection;
public:
	Camera();
	glm::vec3 position;
	glm::vec3 getCameraPosition() const;
	glm::vec3 getViewDirection() const;
	glm::mat4 getWorldtoViewMatrix() const;
	void theMouseUpdate(const glm::vec2& newMousePosition);
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
};
#endif
