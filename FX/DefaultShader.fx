#include "Lights.fx"

//globals
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;

Texture2D diffuseTexture;
Texture2D normalTexture;
Texture2D specularTexture;

float4 diffuseColor;
float3 lightDirection;

float3 cameraPosition;
float4 specularColor;
float specularPower;

float4 ambientColor = {0.1f, 0.1f, 0.1f, 0.0f};

//samplers
SamplerState Diffuse
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//types
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
};

//vertex shader
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
        
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

	//both the input tangent and binormal are calculated against the world matrix and then normalizzed the same as the input normal vector
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	//specular mapping
	worldPosition = mul(input.position, worldMatrix);

	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	return output;
}


//pixel shader
float4 LightPixelShader(PixelInputType input) : SV_Target
{
	float4 textureColor;
	float4 bumpMap;
	float3 bumpNormal;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float4 specularIntensity;
	float3 reflection;
	float4 specular;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = diffuseTexture.Sample(Diffuse, input.tex);

	//Sample bumpmap pixel
	bumpMap = normalTexture.Sample(Diffuse, input.tex);
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	bumpNormal = (input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal);
	bumpNormal = normalize(bumpNormal);

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(bumpNormal, lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = saturate(diffuseColor * lightIntensity);

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	
    color = color * textureColor;

	//specular pixels
	if(lightIntensity > 0.0f)
	{
		specularIntensity = specularTexture.Sample(Diffuse, input.tex);
		reflection = normalize(2 * lightIntensity * bumpNormal - lightDir);
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
		specular = specular * specularIntensity;
		color = saturate(color + specular);
	}
	
    return color + ambientColor;
}

technique10 tech0
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, LightVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, LightPixelShader()));
        SetGeometryShader(NULL);
    }
}