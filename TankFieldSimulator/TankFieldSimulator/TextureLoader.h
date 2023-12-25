#pragma once

#include <string>
#include <iostream>
#include <vector>

class TextureLoader
{
public:
	static unsigned int CreateTexture(const std::string& strTexturePath);
	static unsigned int LoadSkybox(std::vector<std::string> faces);
};

