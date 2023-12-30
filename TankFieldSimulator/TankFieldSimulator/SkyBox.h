#pragma once
#include <GL/glew.h>
#include <GLM.hpp>
#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"

class SkyBox
{
public:
	SkyBox(Texture texture);
	void Render(Shader& shader);
private:
	static std::vector<Vertex> GetCubeVertices();
	std::vector<Vertex> vertices;
	Texture texture;
	unsigned int VAO, VBO;
};

