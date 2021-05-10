//#include "shader3.h"
//
//shader3::shader3()
//{
//
//}
//
//shader3::~shader3()
//{
//	//glDeleteProgram(m_program);
//}
//
//bool shader3::InitializeFromFile(const char* vertFilename, const char* fragFilename)
//{
//	//std::cout << "Compiling shader3s: " << vertFilename << " | " << fragFilename << std::endl;
//
//	m_vertex = Loadshader3FromFile(vertFilename, GL_VERTEX_shader);
//	m_fragment = Loadshader3FromFile(fragFilename, GL_FRAGMENT_shader);
//
//	if (m_vertex == -1 || m_fragment == -1)
//		return false;
//
//	return Createshader3();
//}
//
//bool shader3::SetUniform(std::string name, int value)
//{
//	int loc = glGetUniformLocation(m_program, name.c_str());
//
//	if (loc == -1)
//		return false;
//
//	glUniform1i(loc, value);
//
//	return true;
//}
//
//bool shader3::SetUniform(std::string name, float value)
//{
//	int loc = glGetUniformLocation(m_program, name.c_str());
//
//	if (loc == -1)
//		return false;
//
//	glUniform1f(loc, value);
//
//	return true;
//}
//
//bool shader3::SetUniform(std::string name, glm::vec3 value)
//{
//	int loc = glGetUniformLocation(m_program, name.c_str());
//
//	if (loc == -1)
//		return false;
//
//	glUniform3f(loc, value.x, value.y, value.z);
//
//	return true;
//}
//
//bool shader3::SetUniform(std::string name, glm::vec4 value)
//{
//	int loc = glGetUniformLocation(m_program, name.c_str());
//
//	if (loc == -1)
//		return false;
//
//	glUniform4f(loc, value.x, value.y, value.z, value.w);
//
//	return true;
//}
//
//bool shader3::SetUniform(std::string name, glm::mat3 value)
//{
//	int loc = glGetUniformLocation(m_program, name.c_str());
//
//	if (loc == -1)
//		return false;
//
//	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
//
//	return true;
//}
//
//bool shader3::SetUniform(std::string name, glm::mat4 value)
//{
//	int loc = glGetUniformLocation(m_program, name.c_str());
//
//	if (loc == -1)
//		return false;
//
//	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
//
//	return true;
//}
///* // I Got rid of this because it was not needed. I wrapped this into the [] operator
//bool shader3::AddAttribute(std::string attribute)
//{
//	int loc = glGetAttribLocation(m_program, attribute.c_str());
//
//	if(loc == -1)
//		return false;
//
//	m_attributeList[attribute] = loc;
//
//	return true;
//}*/
//
//GLuint shader3::operator[](std::string attribute)
//
//{
//	int loc = glGetAttribLocation(m_program, attribute.c_str());
//
//	m_attributeList[attribute] = loc;
//
//	return m_attributeList[attribute];
//}
//
//int shader3::GetProgramID()
//{
//	return m_program;
//}
//
//void shader3::Activate()
//{
//	glUseProgram(m_program);
//}
//
//void shader3::DeActivate()
//{
//	glUseProgram(0);
//}
//
//bool shader3::Createshader3()
//{
//	if (m_vertex == 0 || m_fragment == 0)
//		return false;
//
//	m_program = glCreateProgram();
//	glAttachshader3(m_program, m_vertex);
//	glAttachshader3(m_program, m_fragment);
//	glLinkProgram(m_program);
//
//	GLint info;
//	GLchar log[1024];
//	glGetProgramiv(m_program, GL_LINK_STATUS, &info);
//	if (info != GL_TRUE)
//	{
//		glGetProgramInfoLog(m_program, sizeof(log), NULL, log);
//		std::cerr << "Error linking shader3 program: " << log << std::endl;
//		return false;
//	}
//
//	glValidateProgram(m_program);
//	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &info);
//	if (info != GL_TRUE)
//	{
//		glGetProgramInfoLog(m_program, sizeof(log), NULL, log);
//		std::cerr << "Invalid shader3 program: " << log << std::endl;
//		return false;
//	}
//
//	glDeleteshader3(m_vertex);
//	glDeleteshader3(m_fragment);
//
//	return true;
//}
//
//int shader3::Loadshader3FromFile(const char* filename, GLenum shader3Type)
//{
//	std::ifstream file(filename);
//	std::string filetext;
//
//	if (!file) {
//		std::cerr << "Unable to open file: " << filename << std::endl;
//		return -1;
//	}
//
//	while (file.good()) {
//		std::string line;
//		getline(file, line);
//		filetext.append(line + "\n");
//	}
//
//	file.close();
//
//	GLchar const* shader3_source = filetext.c_str();
//	GLint const shader3_length = filetext.size();
//
//	GLint shader3 = glCreateshader3(shader3Type);
//	glshader3Source(shader3, 1, &shader3_source, &shader3_length);
//	glCompileshader3(shader3);
//
//	GLint status;
//	glGetshader3iv(shader3, GL_COMPILE_STATUS, &status);
//	if (status != GL_TRUE) {
//		std::cerr << "Failed to compile shader3: " << filename << std::endl;
//		GLchar log[1024];
//		glGetshader3InfoLog(shader3, 1024, NULL, log);
//		std::cerr << log << std::endl;
//		return -1;
//	}
//
//	return shader3;
//}