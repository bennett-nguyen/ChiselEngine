#version 410 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D tex; 

void main() {
    vec4 tex_color = texture(tex, TexCoords);

    if (tex_color.a < 0.1f) {
        discard;
    }

    color = tex_color;
}