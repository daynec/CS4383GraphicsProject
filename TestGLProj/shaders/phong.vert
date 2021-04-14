#version 440 core 
out vec3 N;
out vec3 L;
out vec3 E;
out vec3 H;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

uniform vec4 lightPosition;
uniform mat4 Projection;
uniform mat4 ModelView;

void main()
{
    gl_Position = Projection * ModelView * vec4(vertexPosition, 1.0);

    vec4 eyePosition = ModelView * vec4(vertexPosition, 1.0);
    vec4 eyeLightPos = lightPosition;

    N = normalize(ModelView * vec4(vertexNormal,0.0)).xyz;
    L = normalize(eyeLightPos.xyz - eyePosition.xyz);
    E = -normalize(eyePosition.xyz);
    H = normalize(L + E);
}

