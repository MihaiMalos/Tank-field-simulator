#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float hue;

void main()
{   

    vec3 customColor = vec3(0.004,0.008,0.071);
    vec4 skyboxColor = texture(skybox, TexCoords);
    vec3 blendedColor = mix(skyboxColor.rgb, customColor, 1-hue);
    FragColor = vec4(blendedColor, skyboxColor.a);
}