#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 normal_vec;
uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    normal_vec = vec3(aPos.x, aPos.y, aPos.z);
}