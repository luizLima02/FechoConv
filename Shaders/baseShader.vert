#version 460 compatibility

layout (location = 0) in vec2 aPos;


void main()
{ 
    gl_Position = vec4(aPos, 0.0f, 1.0f);

}