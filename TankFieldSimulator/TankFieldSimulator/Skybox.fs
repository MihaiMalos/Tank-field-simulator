#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float hue;

void main()
{    
    FragColor = hue * texture(skybox, TexCoords);
}