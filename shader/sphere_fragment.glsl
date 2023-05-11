#version 330 core
out vec4 FragColor;
in vec3 normal_vec;
uniform vec3 lightSource;
uniform float radius;

#define M_PI 3.1415926535897932384626433832795

float DotProductVec3(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Vec3Sum(vec3 a) {
    return sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2));
}

float Vec3Degree(vec3 a, vec3 b) {
    return acos((DotProductVec3(a, b))/(Vec3Sum(a) * Vec3Sum(b)));
}
void main() {
    float brightness = 1.0f;
    vec3 normal_vec2 = vec3(normal_vec.x, normal_vec.y, sqrt(-pow(normal_vec.x, 2.0f) - pow(normal_vec.y, 2.0f) + pow(radius, 2.0f)));
    float degree = Vec3Degree(normal_vec2, lightSource);
    brightness = pow(degree / (M_PI), 4);
    FragColor=vec4(brightness, brightness, brightness, 1.0f);
}