#version 330 core
in vec3 fragPosition;
in vec3 fragNormal;

out vec4 outColor;

uniform vec3 uColor;
uniform bool uGlow;

uniform vec3 uViewPosition;
uniform vec3 uAmbient;
uniform vec3 uSunDirection;    // points towards the sun
uniform vec3 uSunColor;
uniform vec3 uLampPosition;
uniform vec3 uLampColor;
uniform vec3 uLanternPosition;
uniform vec3 uLanternColor;

vec3 blinnPhong(vec3 N, vec3 V, vec3 L, vec3 lightColor, vec3 baseColor) {
    float diffuse = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float specular = 0.0;
    if (diffuse > 0.0)
        specular = pow(max(dot(N, H), 0.0), 32.0) * 0.3;
    return lightColor * (diffuse * baseColor + specular);
}

vec3 pointLight(vec3 N, vec3 V, vec3 lightPosition, vec3 lightColor, vec3 baseColor) {
    vec3 toLight = lightPosition - fragPosition;
    float distance = length(toLight);
    float attenuation = 1.0 / (1.0 + 0.3 * distance + 0.2 * distance * distance);
    return blinnPhong(N, V, toLight / distance, lightColor * attenuation, baseColor);
}

void main() {
    vec3 baseColor = uColor;
    if (uGlow) {
        outColor = vec4(baseColor, 1.0);
        return;
    }

    vec3 N = normalize(fragNormal);
    vec3 V = normalize(uViewPosition - fragPosition);

    vec3 color = uAmbient * baseColor;
    color += blinnPhong(N, V, normalize(uSunDirection), uSunColor, baseColor);
    color += pointLight(N, V, uLampPosition, uLampColor, baseColor);
    color += pointLight(N, V, uLanternPosition, uLanternColor, baseColor);

    outColor = vec4(color, 1.0);
}
