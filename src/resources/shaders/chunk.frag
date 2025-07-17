#version 460 core
out vec4 frag_color;

uniform sampler2DArray texture_array0;

in vec2 fs_uv_coords;
in float fs_shades;

flat in uint fs_face_id;
flat in uint fs_voxel_id;

void main() {
    vec2 face_uv = fs_uv_coords;
    face_uv.x = (fs_uv_coords.x / 6) + (float(fs_face_id) / 6);

    frag_color = vec4(texture(texture_array0, vec3(face_uv, fs_voxel_id)).rgb * fs_shades, 1.0f);
}