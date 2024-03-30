#version 460 core
layout (location = 0) in dvec2 aPos;
layout (location = 1) in dvec2 aTexCoord;

out dvec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
    TexCoord = dvec2(aTexCoord.x, aTexCoord.y);
}