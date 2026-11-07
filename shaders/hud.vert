#version 330 core
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec4 aColor;

uniform vec2 uScreenSize;
out vec4 fragColor;

void main() {
    // pixels -> -1..1, y up
    vec2 p = aPosition / uScreenSize * 2.0 - 1.0;
    gl_Position = vec4(p.x, -p.y, 0.0, 1.0);
    fragColor = aColor;
}
