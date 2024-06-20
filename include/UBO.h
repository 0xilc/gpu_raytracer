#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <string>

enum UBOBindingPoints
{
	CAMERA_DATA = 1
};

class UBO
{
public:
	UBO();
	~UBO();

	void CreateUBO(const std::string& name, GLuint bindingPoint, GLsizeiptr size);
	void UpdateUBO(const std::string& name, GLsizeiptr offset, GLsizeiptr size, const void* data);
	void BindUBO(const std::string& name);
	GLuint GetUBO(const std::string& name);

private:
	std::unordered_map<std::string, std::pair<GLuint, GLuint>> m_Cache; // name -> {uboID, bindingPoint}
};