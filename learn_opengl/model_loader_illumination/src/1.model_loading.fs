#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

in vec3 Normal;
in vec3 FragPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;

void main()
{    
    // ambient lighting
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

    //diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));

    // specular lighting
    vec3 viewDir = normalize( viewPos- FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords));

    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
    // FragColor = texture(texture_diffuse1, TexCoords);
}