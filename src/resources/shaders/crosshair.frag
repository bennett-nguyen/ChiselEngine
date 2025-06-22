#version 460 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D texture0;

void main() {
    vec4 tex_color = texture(texture0, TexCoords);

    if (tex_color.a < 0.1f) {
        discard;
    }

    color = tex_color;
}