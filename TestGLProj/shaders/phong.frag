 #version 440 core
 in vec3 N;
 in vec3 L;
 in vec3 E;
 in vec3 H;

 out vec4 color2;
void main()
{
     vec3 Normal = normalize(N);
     vec3 Light  = normalize(L);
     vec3 Eye    = normalize(E);
     vec3 Half   = normalize(H);
     

    float Kd = max(dot(Normal, Light), 0.0);
    float Ks = pow(max(dot(Half, Normal), 0.0), 80.0);
    float Ka = 0.0;

    vec4 diffuse  = Kd * vec4(1,1,1,1); // white
    vec4 specular = Ks * vec4(0,0,0,1); // black


    color2 = diffuse + specular;
}
