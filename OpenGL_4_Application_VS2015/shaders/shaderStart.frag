#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;
in vec3 fragPos;

out vec4 fColor;

//lighting
uniform	mat3 normalMatrix;
uniform mat3 lightDirMatrix;
uniform	vec3 lightColor1;
uniform vec3 lightColor2;
uniform vec3 lightColor3;
uniform	vec3 lightDir;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform int day;
uniform int directional;
uniform int fog;
uniform vec3 pointLightPosition1;
uniform vec3 pointLightPosition3;
uniform int broken;

vec3 ambient;
float ambientStrength ;
vec3 diffuse;
vec3 specular;
float specularStrength; 
float shininess = 64.0f;

vec3 lightColor;

float pointLightConstant=1.0f;
float pointLightLinear=0.35f;
float pointLightQuadratic=0.44f;


vec3 CalcPointLight(vec3 position)
{
    vec3 lightDir = normalize(position - fragPos);
	
    // attenuation
    float distance = length(position - fragPos);
    float attenuation = 1.0 / (pointLightConstant + pointLightLinear * distance + pointLightQuadratic * (distance * distance));    

	ambient *= attenuation * vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= attenuation * vec3(texture(diffuseTexture, fragTexCoords));
	specular *= attenuation * vec3(texture(specularTexture, fragTexCoords));
	
    return (ambient + diffuse + specular);
}

vec3 CalcDirLight()
{
		//modulate with diffuse map
		ambient *= vec3(texture(diffuseTexture, fragTexCoords));
		diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
		//modulate woth specular map
		specular *= vec3(texture(specularTexture, fragTexCoords));

		return (ambient + diffuse + specular);
}
void computeLightComponents()
{	
	if(day==1)
	{
		lightColor=lightColor1;
		ambientStrength=0.2f;
		specularStrength=0.5f;
	}
	else
	{
		if(directional==0 || directional==2 || directional==3)
		{
			lightColor=lightColor3;
			ambientStrength=0.2f;
			specularStrength=0.5f;
			
		}
		else
		{
			lightColor=lightColor2;
			ambientStrength=0.1f;
			specularStrength=0.1f;
		}
		
	}
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);	

	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeShadow()
{	
	// perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    // Transform to [0,1] range
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    
    // Get depth of current fragment from light's perspective
    float currentDepth = normalizedCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.005f;
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f;

    return shadow;	
}
float computeFog()
{
	float fogDensity = 0.1f;
	float fragmentDistance = length(fragPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f); 
}




void main() 
{
	
	
	float shadow = computeShadow();
	vec3 color=vec3(0,0,0);

	if(day==1)
	{
		computeLightComponents();
		color+=CalcDirLight();
		//modulate with shadow
		color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	}
	else
	{
		if(directional==1)
		{
			computeLightComponents();
			color+=CalcDirLight();
			//modulate with shadow
			color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
			
		}
		if(directional==0)
		{
			computeLightComponents();
			color+=CalcPointLight(pointLightPosition1);
		}
		if(directional==2)
		{
			computeLightComponents();
			if(broken==0)
			{
				color+=CalcPointLight(pointLightPosition3);
			}

		}
		if(directional==3)
		{
			computeLightComponents();
			color+=CalcPointLight(pointLightPosition1);

			if(broken==0)
			{
				computeLightComponents();
				color+=CalcPointLight(pointLightPosition3);
			}
			
		}
	}


	
	
    
   if( fog == 1)
	{
		float fogFactor = computeFog();
		
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		
		fColor = fogColor * (1 - fogFactor) + vec4(color,1.0f) * fogFactor;
	}
	else fColor = vec4(color, 1.0f);


    
}
