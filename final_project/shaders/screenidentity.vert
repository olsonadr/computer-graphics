#version 330

layout (location = 0)  in vec3 vp;
uniform mat4 camera;
uniform mat4 projection;

void main() {
  gl_Position = camera * projection * vec4(vp.x, vp.y, vp.z, 1.0);
};