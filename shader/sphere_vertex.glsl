#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 normal_vec;
uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    normal_vec = gl_Position.xyz;
}