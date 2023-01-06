#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aAmbient;
layout(location = 5) in vec3 aDiffuse;
layout(location = 6) in vec3 aSpecular;

out vec3 vs_FragPos;
out vec3 vs_Color;
out vec2 vs_TexCoord;
out vec3 vs_Normal;
out vec3 vs_Ambient;
out vec3 vs_Diffuse;
out vec3 vs_Specular;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_FragPos = vec4(model * vec4(aPos, 1.0f)).xyz;
	vs_Color = aColor;
	vs_TexCoord = vec2(aTexCoord.x, aTexCoord.y * -1.f);
	vs_Normal = mat3(model) * aNormal;
	vs_Ambient = aAmbient;
	vs_Diffuse = aDiffuse;
	vs_Specular = aSpecular;

	gl_Position = projection * view * vec4(vs_FragPos, 1.0f);
}

#shader fragment
#version 330 core

in vec3 vs_FragPos;
in vec3 vs_Color;
in vec2 vs_TexCoord;
in vec3 vs_Normal;
in vec3 vs_Ambient;
in vec3 vs_Diffuse;
in vec3 vs_Specular;
out vec4 FragColor;


uniform vec3 lightPos = vec3(2000.0f, 1002600.0f, 2000.0f);;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform int n = 1;

void main()
{
	vec3 ambiental = (vec4(lightColor, 1.f) * vec4(vs_Ambient, 1.f)).xyz;

	vec3 lightDir = normalize(vs_FragPos - lightPos);
	vec3 viewDir = normalize(viewPos - vs_FragPos);
	vec3 reflectDir = reflect(lightDir, vs_Normal);
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0f), n);
	vec3 diffuse = lightColor * vs_Diffuse * clamp(dot(lightDir, vs_Normal), 0, 1);
	vec3 specular = vs_Specular.x * specFactor * (lightColor - vec3(0.1f, 0.1f, 0.1f));

	FragColor = (vec4((diffuse + ambiental), 1.0f) + 0.2f * vec4(specular, 1.0f)) * vec4(vs_Color, 1.0f) * 0.8f + 0.2f * vec4(specular, 1.0f);
}