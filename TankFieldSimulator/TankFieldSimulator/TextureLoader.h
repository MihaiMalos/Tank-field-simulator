#pragma once

#include <string>
#include <iostream>

class TextureLoader
{
public:
	static unsigned int CreateTexture(const std::string& strTexturePath);
};

