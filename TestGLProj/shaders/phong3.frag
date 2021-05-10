#version 440 core
 in vec3 N;
 in vec3 L;
 in vec3 E;
 in vec3 H;

 uniform vec4 lightDiffuse;
 uniform vec4 lightSpecular;
 uniform vec4 lightAmbient;

 uniform vec4 surfaceDiffuse;
 uniform vec4 surfaceSpecular;
 uniform float shininess;
 uniform vec4 surfaceAmbient;
 uniform vec4  surfaceEmissive;

 out vec4 color;
void main()
{
     vec3 Normal = normalize(N);
     vec3 Light  = normalize(L);
     vec3 Eye    = normalize(E);
     vec3 Half   = normalize(H);
  

    float Kd = max(dot(Normal, Light), 0.0);
    float Ks = pow(max(dot(Half, Normal), 0.0), shininess);
    float Ka = 1.0;

    vec4 diffuse  = Kd * lightDiffuse*surfaceDiffuse;
    vec4 specular = Ks * lightSpecular*surfaceSpecular;
    vec4 ambient  = Ka * lightAmbient*surfaceAmbient;

    color = surfaceEmissive + ambient + diffuse + specular;
}
