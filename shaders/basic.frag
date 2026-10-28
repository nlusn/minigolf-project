#version 330 core
in vec3 fragNormal;
out vec4 outColor;

uniform vec3 uColor;

void main() {
    // TODO: replace with Blinn-Phong 
    vec3 lightDir = normalize(vec3(0.4, 1.0, 0.6));
    float brightness = 0.4 + 0.6 * max(dot(normalize(fragNormal), lightDir), 0.0);
    outColor = vec4(uColor * brightness, 1.0);
}
