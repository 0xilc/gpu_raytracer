#pragma once
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "UBO.h"
#include "Input.h"
#define M_PI 3.14159265358979323846

class Camera
{
public:
	Camera() = default;
	Camera(int screenWidth, int screenHeight, float fov);
	~Camera();

	void Update(float dt);

	void SetUniforms(UBO& ubo);
	void OnResize(int screenWidth, int screenHeight);
	void UpdateMatrices();

	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessKeyboard(float deltaTime);

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

private:
	static void MouseCallback(void* self, float x, float y);

	// Camera Attributes
	int m_ScreenWidth;
	int m_ScreenHeight;

	float m_FOV = 90.0f;
	float m_AspectRatio;
	float m_Near = 0.2f;
	float m_Far = 200.0f;

	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;
	glm::vec3 m_PlaneCenter;
	
	// Euler Angles
	float m_Yaw;
	float m_Pitch;
	
	// Camera options
	float m_MovementSpeed;
	float m_MouseSensitivity;
	float m_Zoom;
	int m_CallbackId;
	
	void UpdateCameraVectors();
};