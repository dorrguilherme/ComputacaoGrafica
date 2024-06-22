#version 410

in vec2 tex_coord_shader;
in vec3 final_color;
in vec3 frag_pos;
in vec3 scaled_normal;

out vec4 color;

uniform sampler2D tex_buffer;

// Propriedades da superfície
uniform float ka;
uniform float kd;
uniform float ks;
uniform float q;

// Propriedades da fonte de luz
uniform vec3 light_pos;
uniform vec3 light_color;

// Posição da Câmera
uniform vec3 camera_pos;

void main()
{
    // Cálculo da parcela de iluminação ambiente
    vec3 ambient = ka * light_color;

    // Cálculo da parcela de iluminação difusa
    vec3 N = normalize(scaled_normal);
    vec3 L = normalize(light_pos - frag_pos);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = kd * diff * light_color;

    // Cálculo da parcela de iluminação especular
    vec3 V = normalize(camera_pos - frag_pos);
    vec3 R = reflect(-L, N);
    float spec = max(dot(R, V), 0.0);
    spec = pow(spec, q);
    vec3 specular = ks * spec * light_color;

    // Combinação das parcelas de iluminação com a textura
    vec3 tex_color = texture(tex_buffer, tex_coord_shader).rgb;
    vec3 result = (ambient + diffuse) * tex_color + specular;

    color = vec4(result, 1.0);
}
