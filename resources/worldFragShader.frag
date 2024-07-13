#version 330 core

in vec2 uv;
in float tileId;

uniform sampler2DArray tilesetArray;

out vec4 outColor;

void main()
{
        outColor = texture(tilesetArray, vec3(uv, tileId));
}
