#version 400 core

layout (location = 0) in dvec2 aPos;
layout (location = 1) in dvec2 aTexCoord;

out dvec2 TexCoord;

void main()
{
    gl_Position = dvec4(aPos.x, aPos.y, 0.0, 1.0); // Use 'dvec4' if needed
    TexCoord = aTexCoord;  
}