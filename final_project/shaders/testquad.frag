// GLSL version
#version 420 compatibility

uniform sampler2D texture;

varying vec2 textureCoord;
void main() {
   vec4 color1 = texture2D(texture,textureCoord);
   gl_FragColor = color1;
}