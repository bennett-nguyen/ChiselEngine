#version 460 core
out vec4 frag_color;

uniform sampler2DArray texture_array0;

in vec2 fs_uv_coords;
in float fs_shades;

flat in uint fs_face_id;
flat in uint fs_voxel_id;

vec4 texture2DArrayAA(sampler2DArray tex, vec2 uv) {
    vec2 texsize = textureSize(tex, 0).xy;
    vec2 uv_texspace = uv * texsize;
    vec2 seam = floor(uv_texspace + .5f);
    uv_texspace = (uv_texspace-seam) / fwidth(uv_texspace) + seam;
    uv_texspace = clamp(uv_texspace, seam - .5f, seam + .5f);
    return texture(tex, vec3(uv_texspace / texsize, fs_voxel_id));
}

void main() {
    vec2 face_uv = fs_uv_coords;
    face_uv.x = (fs_uv_coords.x / 6) + (float(fs_face_id) / 6);

    vec3 tex_color = texture2DArrayAA(texture_array0, face_uv).rgb;
    vec4 final_color = vec4(vec3(1.0f) * fs_shades, 1.0f);
    frag_color = final_color;
}