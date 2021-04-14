#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

class Shader {

public:
	Shader();
	~Shader();

	bool InitializeFromFile(const char* vertFilename, const char* fragFilename);

	bool SetUniform(std::string name, int value);
	bool SetUniform(std::string name, float value);
	bool SetUniform(std::string name, glm::vec3 value);
	bool SetUniform(std::string name, glm::vec4 value);
	bool SetUniform(std::string name, glm::mat3 value);
	bool SetUniform(std::string name, glm::mat4 value);

	bool AddAttribute(std::string attribute);
	GLuint operator[](std::string attribute);

	int GetProgramID();

	void Activate();
	void DeActivate();

private:
	bool CreateShader();
	int LoadShaderFromFile(const char* filename, GLenum shaderType);

private:
	int m_vertex;
	int m_fragment;
	int m_program;
	std::map<std::string, GLuint> m_attributeList;
};

#endif