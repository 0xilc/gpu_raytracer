#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include "glm/glm.hpp"

class Shader
{
public:
	GLuint m_Program;

	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
	Shader(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) = delete;
	~Shader();
	void Use();
	void Load(const char* vertexPath, const char* fragmentPath);


	// Uniform setters
	void SetUniform1i(const std::string& name, int i);
	void SetUniform1f(const std::string& name, float f);
	void SetUniform2f(const std::string& name, const glm::vec2& vector);
	void SetUniform3f(const std::string& name, const glm::vec3& vector);
	void SetUniformMat4fv(const std::string& name, const glm::mat4& matrix);


private:
	void checkCompileErrors(GLuint shader, std::string type);
};