#pragma once
#include <GL/glew.h>
#include <GLM.hpp>
#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"

class SkyBox
{
public:
	SkyBox(float x, float y, float z, Texture texture);
	void Render(Shader& shader);
private:
	static std::vector<Vertex> GetCubeVertices(float x, float y, float z);
	std::vector<Vertex> vertices;
	Texture texture;
	unsigned int VAO, VBO;
};

