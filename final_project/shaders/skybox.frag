// skybox.frag
// source: https://ogldev.org/www/tutorial25/tutorial25.html

#version 330

in vec3 TexCoord0;

out vec4 FragColor;

uniform samplerCube gCubemapTexture;

void main()
{
    FragColor = texture(gCubemapTexture, TexCoord0);
}