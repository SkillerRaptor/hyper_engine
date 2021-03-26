#version 330 core

out vec4 a_FragColor;

in vec4 v_SpriteColor;

void main()
{
	if (v_SpriteColor.w < 0.1)
		discard;

    a_FragColor = v_SpriteColor;
}
