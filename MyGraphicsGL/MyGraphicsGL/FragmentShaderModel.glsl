#version 430

in vec2 TexCoords;
in vec3 thePositionWorld;
in vec3 theNormalWorld;
in vec4 PositionLightSpace;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 eyePositionWorld;
uniform vec3 lightPositionWorld;
uniform vec4 ambientLightWorld;

uniform sampler2D shadowMap;

float ShadowCalculation(vec4 PositionLightSpace)
{
    // perform perspective divide
    vec3 projCoords = PositionLightSpace.xyz / PositionLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	vec3 normal = normalize(theNormalWorld);
    vec3 lightDir = normalize(lightPositionWorld - thePositionWorld);

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.015);

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}  

void main()
{    
	vec3 normal = normalize(theNormalWorld);
	
	//diffuse + ambient
	vec3 lightVectorWorld = normalize(lightPositionWorld - thePositionWorld);
	float bright = dot(lightVectorWorld, normal);
	vec3 diffusedLight = vec3(bright,bright,bright) * vec3(texture(texture_diffuse1, TexCoords));
	vec3 ambientLight = vec3(ambientLightWorld) * vec3(texture(texture_diffuse1, TexCoords));
	
	//specular
	vec3 reflectedlightvectorWorld = reflect(-lightVectorWorld, normal);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - thePositionWorld);
	float specs = clamp(dot(reflectedlightvectorWorld, eyeVectorWorld),0,1);
	vec3 specularLight = vec3(specs, specs, specs) * vec3(texture(texture_specular1, TexCoords));

	//shadows
	float shadow = ShadowCalculation(PositionLightSpace);
	shadow = min(shadow, 0.5);
	vec3 lighting = (ambientLight + (1.0 - shadow) * (diffusedLight + specularLight));
	color = vec4(lighting,1.0);

}