#version 330 core

uniform sampler2D uTex0;
smooth in vec2 vTexCoord;
out vec4 fColor;

void main()
{
    fColor = texture( uTex0, vTexCoord );
}
