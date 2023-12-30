#include "SkyBox.h"


SkyBox::SkyBox(float x, float y, float z, Texture texture)
{
	this->texture = texture;
	vertices = GetCubeVertices(x, y, z);

	// plane VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

std::vector<Vertex> SkyBox::GetCubeVertices(float x, float y, float z)
{
	return std::vector<Vertex>{
		{ -x, -y, -z, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f }, // bottom-left
		{ x, y,-z, 0.0f,  0.0f,-1.0f, 1.0f, 1.0f, }, // top-lright
		{ x,-y,-z, 0.0f,  0.0f,-1.0f, 1.0f, 0.0f, }, // bottom-lright         
		{ x, y,-z, 0.0f,  0.0f,-1.0f, 1.0f, 1.0f, }, // top-lright
		{ -x,-y,-z, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f, }, // bottom-left
		{ -x, y,-z, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f, }, // top-
		{ -x,-y, z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, }, // bottom-left
		{ x,-y, z, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f, }, // bottom-lright
		{ x, y, z, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, }, // top-lright
		{ x, y, z, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, }, // top-lright
		{ -x, y, z, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, }, // top-left
		{ -x,-y, z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, }, // bottom-
		{ -x, y, z,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f, }, // top-lright
		{ -x, y,-z,-1.0f, 0.0f, 0.0f, 1.0f, 1.0f, }, // top-left
		{ -x,-y,-z,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, }, // bottom-left
		{ -x,-y,-z,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, }, // bottom-left
		{ -x,-y, z,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, }, // bottom-lright
		{ -x, y, z,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f, }, // top-
		{ x, y, z, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, }, // top-left
		{ x,-y,-z, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, }, // bottom-lright
		{ x, y,-z, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, }, // top-lright         
		{ x,-y,-z, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, }, // bottom-lright
		{ x, y, z, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, }, // top-left
		{ x,-y, z, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f, }, // bottom-
		{ -x,-y,-z, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, }, // top-lright
		{ x,-y,-z, 0.0f,  -1.0f, 0.0f, 1.0f, 1.0f, }, // top-left
		{ x,-y, z, 0.0f,  -1.0f, 0.0f, 1.0f, 0.0f, }, // bottom-left
		{ x,-y, z, 0.0f,  -1.0f, 0.0f, 1.0f, 0.0f, }, // bottom-left
		{ -x,-y, z, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, }, // bottom-lright
		{ -x,-y,-z, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, }, // top-
		{ -x, y,-z, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, }, // top-left
		{ x, y, z, 0.0f,   1.0f, 0.0f, 1.0f, 0.0f, }, // bottom-lright
		{ x, y,-z, 0.0f,   1.0f, 0.0f, 1.0f, 1.0f, }, // top-lright     
		{ x, y, z, 0.0f,   1.0f, 0.0f, 1.0f, 0.0f, }, // bottom-lright
		{ -x, y,-z, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, }, // top-left
		{ -x, y, z, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f } // bott
	};
}

void SkyBox::Render(Shader& shader)
{
	glDisable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);
	shader.Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

