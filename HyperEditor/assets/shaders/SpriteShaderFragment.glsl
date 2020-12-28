#version 330 core
out vec4 a_FragColor;

in vec4 v_SpriteColor;

void main()
{
    a_FragColor = v_SpriteColor;
}
