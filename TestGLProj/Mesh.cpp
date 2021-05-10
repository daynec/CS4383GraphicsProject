#include "Mesh.h"

Mesh::Mesh()
{

}

Mesh::Mesh(char* filename, Shader* shader)
{
	m_vCount = 0;
	m_iCount = 0;
	m_nCount = 0;
	m_filename = filename;
	m_shader = shader;
	UsingDefault = true;
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	LoadMesh();
}

Mesh::~Mesh(void)
{

}

void Mesh::render(glm::mat4 ModelView, glm::mat4 Projection) {

	m_shader->Activate(); // Bind shader.
	m_shader->SetUniform("Projection", Projection);
	m_shader->SetUniform("ModelView", ModelView);
	//m_shader->SetUniform("lightPosition", glm::vec4(1.0, 0.0, 0.0, 1.0));

	glEnableVertexAttribArray((*m_shader)["vertexPosition"]); // Enable vertex attribute.
	glEnableVertexAttribArray((*m_shader)["vertexNormal"]); // Enable normal attribute.

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Bind VBO.
	glVertexAttribPointer((*m_shader)["vertexPosition"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); // Attribute pointer.
	glVertexAttribPointer((*m_shader)["vertexNormal"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO); // Bind IBO.
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0); // Draw.

	glDisableVertexAttribArray((*m_shader)["vertexPosition"]); // Disable vertex attribute.
	glDisableVertexAttribArray((*m_shader)["vertexNormal"]); // Disable normal attribute.

	m_shader->DeActivate(); // Unbind shader.
}

void Mesh::changeNormals()
{
	if (UsingDefault) { // Toggles manually generated normals.
		recalculateVertexNormals();
		UsingDefault = false;
	}

	else { // Toggles normals from file.
		CreateData();
		UsingDefault = true;
	}
}

void Mesh::recalculateVertexNormals() {

	Vertices.clear(); // Clear all data from vertex list.
	Indices.clear(); // Clear all data from index list.
	Vertices.resize(m_vCount);
	Indices.resize(m_iCount);

	// Store position data from file in vertex list.
	for (GLuint i = 0; i < Vertices.size(); i++)
		Vertices[i].Position = position[i];

	// Store position indices in index list.
	for (GLuint i = 0; i < Indices.size(); i++)
		Indices[i] = pIndices[i];

	// Generate normals
	for (GLuint i = 0; i < Indices.size(); i += 3)
	{
		// Indices for triangle
		GLuint index0 = Indices[i];
		GLuint index1 = Indices[i + 1];
		GLuint index2 = Indices[i + 2];

		// Generate the normal for that triangle.
		glm::vec3 v1 = Vertices[index1].Position - Vertices[index0].Position;
		glm::vec3 v2 = Vertices[index2].Position - Vertices[index0].Position;

		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		// Add that normal for each vertex.
		Vertices[index0].Normal += normal;
		Vertices[index1].Normal += normal;
		Vertices[index2].Normal += normal;
	}

	// Loop through all normals and normalize them (same as averaging).
	for (GLuint i = 0; i < m_vCount; i++)
		glm::vec3 temp = Vertices[i].Normal;

	CreateBuffers(); // Refill buffers.
}
void Mesh::reverseNormals() {

	// Multiply all normals by -1.
	for (GLuint i = 0; i < Vertices.size(); i++)
		Vertices[i].Normal *= -1;

	// Refill array buffer with new normal data.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
}

bool Mesh::LoadMesh()
{
	std::ifstream file(m_filename);
	std::vector<std::string*> text;
	int vIndex = 0, fIndex = 0, nIndex = 0;
	char buf[1024];
	float x, y, z;
	float nx = 0, ny = 0, nz = 0;
	int ix = 0, iy = 0, iz = 0, inx = 0, iny = 0, inz = 0;

	// Load obj file.
	if (!file.is_open()) {
		std::cerr << "Unable to open file: " << m_filename << std::endl;
		return false;
	}

	// Read contents of file into string list.
	while (!file.eof()) {
		file.getline(buf, 1024);
		text.push_back(new std::string(buf));
	}

	// Get vertex counts to initialize arrays at the right size.
	if (!Count(text)) {
		std::cerr << "Error calculating vertex counts." << std::endl;
		return false;
	}

	// Initialize array of vertices and indices.
	position = new glm::vec3[m_vCount];
	normals = new glm::vec3[m_nCount];
	pIndices = new GLuint[m_iCount];
	nIndices = new GLuint[m_iCount];

	for (unsigned int i = 0; i < text.size(); i++)
	{
		// Line is blank, skip.
		if (text[i]->empty())
			continue;

		// Line is a comment, skip.
		if ((*text[i])[0] == '#')
			continue;

		// Line is a vertex.
		else if ((sscanf_s(text[i]->c_str(), "v %f %f %f", &x, &y, &z) == 3))
			position[vIndex++] = glm::vec3(x, y, z);

		// Line is a normal.
		else if ((sscanf_s(text[i]->c_str(), "vn %f %f %f", &nx, &ny, &nz) == 3))
			normals[nIndex++] = glm::vec3(nx, ny, nz);

		// Line is a face.
		else if ((sscanf_s(text[i]->c_str(), "f %d//%d %d//%d %d//%d", &ix, &inx, &iy, &iny, &iz, &inz) == 6))
		{
			pIndices[fIndex] = ix - 1;
			pIndices[fIndex + 1] = iy - 1;
			pIndices[fIndex + 2] = iz - 1;

			nIndices[fIndex] = inx - 1;
			nIndices[fIndex + 1] = iny - 1;
			nIndices[fIndex + 2] = inz - 1;
			fIndex += 3;
		}
	}

	// delete file strings.
	for (unsigned int i = 0; i < text.size(); i++)
		delete text[i];
	text.clear();

	CreateData(); // Generate vertices and normals using data in the .obj file.

	return true;
}

bool Mesh::CreateData()
{
	Vertices.clear(); // Clear all data from the vertex list.
	Indices.clear(); // Clear all data from the index list
	vMap.clear();  // Clear the previous hash.

	for (GLuint i = 0; i < m_iCount; i++)
	{
		Vertex temp; // temporary vertex
		int pIndex = pIndices[i]; // get index of position for current vertex.
		int nIndex = nIndices[i]; // get index of normal for current vertex.

		GLuint key = pIndex; // initial key is the position index.
		temp.Position = position[pIndex]; //store position in temporary variable.
		temp.Normal = normals[nIndex]; // store normal in temporary variable.

		key += nIndex * 1000; // update key with normal index * the a constant value.

		// is vertex/normal combo unique?
		if (vMap.count(key) == 0)
		{
			// add index for current key (vertex/normal pair).
			vMap[key] = Vertices.size();
			// add new index into index list.
			Indices.push_back(Vertices.size());
			// add new vertex into vertex list.
			Vertices.push_back(temp);
		}

		else
			// pair already exists so just get index for it using key.
			Indices.push_back(vMap[key]);
	}

	CreateBuffers(); // Fill the buffers with new data.

	return true;
}

bool Mesh::CreateBuffers()
{
	// Bind vbo to the array buffer.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// Fill array buffer with vertex data.
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

	// Bind ibo to the index buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	// Fill index buffer with index data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), &Indices[0], GL_STATIC_DRAW);

	return true;
}

// Gets vertex, normal, and face counts to dynamically create arrays at the correct size.
bool Mesh::Count(std::vector<std::string*> text)
{
	GLuint face = 0;

	for (unsigned int i = 0; i < text.size(); i++)
	{
		// Line is blank, skip.
		if (text[i]->empty())
			continue;

		// Line is a comment, skip.
		if ((*text[i])[0] == '#')
			continue;

		// Line is a vertex, increment count.
		else if ((*text[i])[0] == 'v' && ((*text[i])[1] == ' '))
			m_vCount++;

		// Line is a normal, increment count.
		else if ((*text[i])[0] == 'v' && ((*text[i])[1] == 'n'))
			m_nCount++;

		// Line is a face, increment count.
		else if ((*text[i])[0] == 'f')
			face++;
	}

	// For each face there are 3 indices.
	m_iCount = face * 3;

	return true;
}