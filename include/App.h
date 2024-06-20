#pragma once
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include "Camera.h"
#include "Shader.h"
#include "SSBO.h"

static struct WindowState
{
	WindowState() :width(800), height(600), title("OpenGL"), fps(0) {}
	WindowState(int _width, int _height, std::string _title) :width(_width), height(_height), title(_title), fps(0) {}
	int width;
	int height;
	std::string title;
	float fps;
	GLFWwindow* window;
} s_WindowState;

class App
{
public:
	App();
	~App();

	void Init();
	void Run();
	void Update(float deltaTime);
	void Render();
	void ProcessInput();

private:
	GLuint m_QuadVAO;
	std::shared_ptr<Shader> m_RayTracingShader;
	std::unique_ptr<Camera> m_Camera;
	std::shared_ptr<UBO> m_UBO;
	std::shared_ptr<SSBO> m_SSBO;
};