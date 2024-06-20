#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <string>

enum SSBOBindingPoints
{
	BVHNodes = 2,
	Primitives = 3,
	Materials = 4,
	Lights = 5
};

class SSBO
{
public:
	SSBO();
	~SSBO();

	void CreateSSBO(const std::string& name, GLuint bindingPoint, GLsizeiptr size);
	void UpdateSSBO(const std::string& name, GLsizeiptr offset, GLsizeiptr size, const void* data);
	void BindSSBO(const std::string& name);
	GLuint GetSSBO(const std::string& name);

private:
	std::unordered_map<std::string, std::pair<GLuint, GLuint>> m_Cache; // name -> {ssboID, bindingPoint}
};