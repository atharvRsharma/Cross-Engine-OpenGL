#version 330 core
out vec4 FragColor;

in vec3 FragPos; 
in vec3 Normal;  

uniform float energy;
uniform vec3 viewPos;

void main() {
    vec3 lowEnergyColor = vec3(0.0, 0.1, 0.5);
    vec3 highEnergyColor = vec3(1.0, 1.0, 0.0);
    vec3 baseColor = mix(lowEnergyColor, highEnergyColor, energy);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    float centerFactor = dot(viewDir, norm);
    centerFactor = pow(centerFactor, 2.5); 

    vec3 finalColor = mix(baseColor, vec3(1.0, 1.0, 1.0), centerFactor);
    FragColor = vec4(finalColor * (0.5 + energy), 1.0);
}