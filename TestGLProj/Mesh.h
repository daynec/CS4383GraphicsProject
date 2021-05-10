#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "Shader.h"

class Mesh {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
	};

public:
	Mesh();
	Mesh(char* filename, Shader* shader);
	~Mesh(void);

	void render(glm::mat4 ModelView, glm::mat4 Projection);
	void recalculateVertexNormals();
	void reverseNormals();
	void changeNormals();

private:
	bool LoadMesh();
	bool Count(std::vector<std::string*> text);
	bool CreateData();
	bool CreateBuffers();

private:
	Shader* m_shader;
	std::vector<Vertex> Vertices;
	std::vector<GLuint> Indices;
	std::map<GLuint, GLuint> vMap;

	glm::vec3* position;
	glm::vec3* normals;
	GLuint* pIndices;
	GLuint* nIndices;

	char* m_filename;
	GLuint m_vCount;
	GLuint m_iCount;
	GLuint m_nCount;

	GLuint m_VBO;
	GLuint m_IBO;

	bool UsingDefault;
};
