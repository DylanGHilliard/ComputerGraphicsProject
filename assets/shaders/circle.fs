#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform vec4 COLOR;

void main()
{
    float dist = length(uv - vec2(0.5, 0.5));
    if (dist > 0.5)
        discard; // Discard fragments outside the circle

    FragColor = COLOR;
}