////////////////////////////////////////////////////////////////////////////////
// Filename: light.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
matrix worldInverseTranspose;

Texture2D diffuseTexture;
Texture2D normalTexture;

float4 diffuseColor;
float3 lightDirection;

float4 ambientColor = {0.07f, 0.07f, 0.07f, 0.07f};
float specular = 0.4f;
float specularPower = 55.0f;
float bumpiness = 1.0f;
float reflectionStrength = 0.5f;

float3 eyePosition;


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState Diffuse
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//////////////
// TYPEDEFS //
//////////////
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
	float4 homogeniousPosition : POSITION;
	float2 tex : TEXCOORD0;
	float3 lightVector : TEXCOORD1;
	float3 worldNormal : TEXCOORD2;
	float3 WorldTangent : TEXCOORD3;
	float3 worldBinormal : TEXCOORD4;
	float3 worldView : TEXCOORD5;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output = (PixelInputType(0));
	output.worldNormal = mul(input.normal, worldInverseTranspose).xyz;
	output.WorldTangent = mul(input.tangent, worldInverseTranspose).xyz;
	output.worldBinormal = mul(input.binormal, worldInverseTranspose).xyz;

	float4 position = float4(input.position.xyz, 1.0f);
	float3 worldPosition = mul(position, mul(worldView * worldProjection)).xyz;

	output.lightVector = (lightDirection - worldPosition);

	output.tex = input.tex;

	output.worldView = normalize(viewInverse[3].xyz - Pw);
	output.homogeniousPosition = mul(Po, worldViewProjectionInverse);

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_Target
{

}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 tech0
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, LightVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, LightPixelShader()));
        SetGeometryShader(NULL);
    }
}