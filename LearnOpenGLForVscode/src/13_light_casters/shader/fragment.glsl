#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    //vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // 点光源衰减
    float constant;
    float linear;
    float quadratic;
    // 聚光（手电筒）（聚光位置、切光角、聚光方向向量
    vec3 direction;
    float cutOff; // 内圆锥余弦值
    float outerCutOff;//外圆锥余弦值

};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    // 环境光
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    // 反射光
    vec3 norm = normalize(Normal); // 标准化法向量
    vec3 lightDir = normalize(light.position - FragPos); // 物体到光源的方向向量
    float diff = max(dot(norm, lightDir), 0.0); // 法线与光线的余弦值
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    // 镜面光(观察者和反射光线的夹角余弦值)
    vec3 viewDir = normalize(viewPos - FragPos); // 物体到观察者的方向向量
    vec3 reflectDir = reflect(-lightDir, norm); // 反射光线
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    // 聚光
    float theta = dot(lightDir, normalize(-light.direction)); // 光到物体的方向与光线的方向夹角余弦值
    float epsilon = (light.cutOff - light.outerCutOff); // 内角与外角的差
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    //衰减
    float distance = length(light.position - FragPos); // 光的位置到物体位置的距离
    float attenuation =  1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
