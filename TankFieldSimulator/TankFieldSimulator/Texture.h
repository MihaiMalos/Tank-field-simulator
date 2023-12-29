#pragma once
#include <string>

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;

    void SetTextureId(unsigned int id)
    {
        this->id = id;
        this->type = "texture_diffuse";
    }
};