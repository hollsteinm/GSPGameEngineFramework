struct Light
{
	float3 position;
	float3 direction;
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float3 attentuation;
	float spotPower;
	float range;
};

struct Material
{
	float3 position;
	float3 normal;
	float4 diffuseColor;
	float4 specularColor;
};

float3 DirectionalLight(Material mat, Light light, float3 cameraPosition, float shadowFactor)
{
	float3 color = float3(0.0f, 0.0f, 0.0f);

	//light vector opposite direction light rays
	float3 lightVector = -light.direction;

	color += mat.diffuseColor * light.ambientColor;

	float diffuseFactor = dot(lightVector, mat.normal);
	[branch]
	if(diffuseFactor > 0.0f)
	{
		float specularPower = max(mat.specularColor.a, 1.0f);
		float3 toCamera = normalize(cameraPosition - mat.position);
		float3 reflectVector = reflect(-lightVector, mat.normal);
		float specularFactor = pow(max(dot(reflectVector, toCamera), 0.0f), specularPower);

		color += shadowFactor * diffuseFactor * mat.diffuseColor * light.diffuseColor;
		color += shadowFactor * specularFactor * mat.specularColor * light.specularColor;
	}

	return color;
}

float3 PointLight(Material mat, Light light, float3 cameraPosition, float shadowFactor)
{
	float3 color = float3(0.0f, 0.0f, 0.0f);

	float3 lightVector = light.position - mat.position;

	//distance
	float d = length(lightVector);

	if(d > light.range)
		return float3(0.0f, 0.0f, 0.0f);

	lightVector /= d;

	color += mat.diffuseColor * light.ambientColor;

	float diffuseFactor = dot(lightVector, mat.normal);
	[branch]
	if(diffuseFactor > 0.0f)
	{
		float specularPower = max(mat.specularColor.a, 1.0f);
		float3 toCamera = normalize(cameraPosition - mat.position);
		float3 reflectVector = reflect(-lightVector, mat.normal);
		float specularFactor = pow(max(dot(reflectVector, toCamera),0.0f), specularPower);

		color += shadowFactor * diffuseFactor * mat.diffuseColor * light.diffuseColor;
		color += shadowFactor * specularFactor * mat.specularColor * light.specularColor;
	}

	return color / dot(light.attentuation, float3(1.0f, d, d*d));
}

float3 Spotlight(Material mat, Light light, float3 cameraPosition, float shadowFactor)
{
	float3 color = PointLight(mat, light, cameraPosition, shadowFactor);
	float3 lightVector = normalize(light.position - mat.position);

	float s = pow(max(dot(-lightVector, light.direction), 0.0f), light.spotPower);
	return color * s;
}