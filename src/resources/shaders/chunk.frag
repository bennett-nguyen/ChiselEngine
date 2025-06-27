#version 460 core
out vec4 frag_color;

uniform sampler2DArray texture_array0;

in vec2 tex_coords;
flat in uint voxel_id;
flat in uint face_id;

void main() {
    vec2 face_uv = tex_coords;
    face_uv.x = (tex_coords.x / 6) + (float(face_id) / 6);

    frag_color = texture(texture_array0, vec3(face_uv, 0.0));
}