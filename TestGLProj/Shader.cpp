#include "Shader.h"

Shader::Shader()
{

}

Shader::~Shader()
{
	//glDeleteProgram(m_program);
}

bool Shader::InitializeFromFile(const char* vertFilename, const char* fragFilename)
{
	//std::cout << "Compiling shaders: " << vertFilename << " | " << fragFilename << std::endl;

	m_vertex = LoadShaderFromFile(vertFilename, GL_VERTEX_SHADER);
	m_fragment = LoadShaderFromFile(fragFilename, GL_FRAGMENT_SHADER);

	if(m_vertex == -1 || m_fragment == -1)
		return false;

	return CreateShader();
}

bool Shader::SetUniform(std::string name, int value)
{
	int loc = glGetUniformLocation(m_program, name.c_str());

	if(loc == -1)
		return false;

	glUniform1i(loc, value);

	return true;
}

bool Shader::SetUniform(std::string name, float value)
{
	int loc = glGetUniformLocation(m_program, name.c_str());

	if(loc == -1)
		return false;

	glUniform1f(loc, value);

	return true;
}

bool Shader::SetUniform(std::string name, glm::vec3 value)
{
	int loc = glGetUniformLocation(m_program, name.c_str());

	if(loc == -1)
		return false;

	glUniform3f(loc, value.x, value.y, value.z);

	return true;
}

bool Shader::SetUniform(std::string name, glm::vec4 value)
{
	int loc = glGetUniformLocation(m_program, name.c_str());

	if(loc == -1)
		return false;

	glUniform4f(loc, value.x, value.y, value.z, value.w);

	return true;
}

bool Shader::SetUniform(std::string name, glm::mat3 value)
{
	int loc = glGetUniformLocation(m_program, name.c_str());

	if(loc == -1)
		return false;

	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));

	return true;
}

bool Shader::SetUniform(std::string name, glm::mat4 value)
{
	int loc = glGetUniformLocation(m_program, name.c_str());

	if(loc == -1)
		return false;

	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));

	return true;
}

bool Shader::AddAttribute(std::string attribute)
{
	int loc = glGetAttribLocation(m_program, attribute.c_str());

	if(loc == -1)
		return false;

	m_attributeList[attribute] = loc;

	return true;
}

GLuint Shader::operator[](std::string attribute)
{
	return m_attributeList[attribute];
}

int Shader::GetProgramID()
{
	return m_program;
}

void Shader::Activate()
{
	glUseProgram(m_program);
}

void Shader::DeActivate()
{
	glUseProgram(0);
}

bool Shader::CreateShader()
{
	if(m_vertex == 0 || m_fragment == 0)
		return false;

	m_program = glCreateProgram();
	glAttachShader(m_program, m_vertex);
	glAttachShader(m_program, m_fragment);
	glLinkProgram(m_program);

	GLint info;
	GLchar log[1024];
	glGetProgramiv(m_program, GL_LINK_STATUS, &info);
	if(info != GL_TRUE)
	{
		glGetProgramInfoLog(m_program, sizeof(log), NULL, log);
		std::cerr << "Error linking shader program: " << log << std::endl;
		return false;
	}

	glValidateProgram(m_program);
	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &info);
	if(info != GL_TRUE)
	{
		glGetProgramInfoLog(m_program, sizeof(log), NULL, log);
		std::cerr << "Invalid shader program: " << log << std::endl;
		return false;
	}

	glDeleteShader(m_vertex);
	glDeleteShader(m_fragment);

	return true;
}

int Shader::LoadShaderFromFile(const char* filename, GLenum shaderType)
{
	std::ifstream file(filename);
	std::string filetext;

	if(!file) {
		std::cerr << "Unable to open file: " << filename << std::endl;
		return -1;
	}

	while(file.good()) {
		std::string line;
		getline(file, line);
		filetext.append(line + "\n");
	}

	file.close();

	GLchar const* shader_source = filetext.c_str();
	GLint const shader_length = filetext.size();

	GLint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shader_source, &shader_length);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		std::cerr << "Failed to compile shader: " << filename << std::endl;
		GLchar log[1024];
		glGetShaderInfoLog(shader, 1024, NULL, log);
		std::cerr << log << std::endl;
		return -1;
	}

	return shader;
}