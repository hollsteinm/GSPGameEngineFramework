uniform extern float3 gDirection;
uniform extern float3 gColor;

uniform extern float3 gCameraPosition;

float4x4 invertViewProj;

Texture2D colorMap;
Texture2D normalMap;
Texture2D depthMap;

SamplerState LinearClamp
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState PointClamp
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct VSInput
{
	float3 position : POSITION0;
	float2 texCoord : TEXCOORD0;
};

struct VSOutput
{
	float4 position : POSITION0;
	float2 texCoord0 : TEXCOORD0;
};

float2 halfPixel;

VSOutput VS(VSInput input)
{
	VSOutput output;

	output.position = float4(input.position, 1.0f);
	output.texCoord = input.texCoord - halfPixel;

	return output;
}

float4 PS(VSOutput input) : COLOR0
{
	float4 normaldata = NormalMap.Sample(PointClamp, input.texCoord);

	float3 normal = 2.0f * normalData.xyz - 1.0f;

	float specPower = normalData.a * 255.0f;

	float specIntensity = SpecularMap.Sample(LinearWrap, input.texCoord);

	float depthVal = DepthMap.Sample(PointClamp, input.texCoord).r;

	float4 pos;
	pos.x = input.texCoord.x * 2.0f - 1.0f;
	pos.y = -(input.texCoord.y * 2.0f - 1.0f);
	pos.z = depthVal;
	pos.w = 1.0f;

	pos = mul(pos, InvertViewProjection);
	pos /= pos.w;

	//surface to light vector
	float3 lightvector = -normalize(gDirection);

	//diffuse color
	float ndl = max(0.0f, dot(normal, lightvector));
	float3 diffuseLight = ndl * gColor.rgb;

	//reflection
	float3 reflectionvector = normalize(reflect(lightvector, normal));

	float3 dirTocamera = normalize(gCameraPosition - pos);

	//specular
	float specLight = specIntensity * pow(saturate(dot(reflectionvector, dirTocamera)), specPower);

	return float4(diffuseLight.rgb, specLight);

}

technique tech0
{
	pass p0
	{
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetPixelShader(CompileShader(ps_4_0, PS()));
        SetGeometryShader(NULL);
	}
}