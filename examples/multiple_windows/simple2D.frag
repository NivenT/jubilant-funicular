#version 150

uniform sampler2D sampler;

in vec4 fragColor;
in vec2 fragUV;
in float hasTexture;

out vec4 finalColor;

void main() {
    finalColor = hasTexture > 0.5 ? fragColor * texture(sampler, fragUV) : fragColor;
}