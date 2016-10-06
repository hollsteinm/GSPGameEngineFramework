uniform extern float4x4 gWorld;
uniform extern float4x4 gView;
uniform extern float4x4 gProjection;

uniform extern float specularIntensity;
uniform extern float specularPower;

Texture2D gGBufferTexture;
Texture2D gSpecularMap;
Texture2D gNormalMap;

SamplerState texSamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VSInput
{
	float4 position : POSITION0;
	float3 normal : NORMAL0;
	float2 texCoord : TEXCOORD0;
	float3 binormal : BINORMAL0;
	float3 tangent : TANGENT0;
};

struct VSOutput
{
	float4 position : POSITION0;
	float2 texCoord : TEXCOORD0;
	float2 depth : TEXCOORD1;
	float3x3 tangentToWorld : TEXCOORD2;
};

struct PSOutput
{
	half4 color : COLOR0;
	half4 normal : COLOR1;
	half4 depth : COLOR2;
};

VSOutput VS(VSInput input)
{
	VSOutput output;

	float4 wpos = mul(float4(input.position.xyz, 1.0f), gWorld);
	float4 vpos = mul(wpos, gView);

	output.position = mul(vpos, gProjection);

	output.texCoord = input.texCoord;
	output.depth.x = output.position.z;
	output.depth.y = output.position.w;

	output.tangentToWorld[0] = mul(input.tangent, gWorld);
	output.tangentToWorld[1] = mul(input.binormal, gWorld);
	output.tangentToWorld[2] = mul(input.normal, gWorld);

	return output;
}

PSOutput PS(VSOutput input)
{
	PSOutput output;
	
	output.color = gGBuffer.Sample(texSamp, input.texCoord);
	
	float4 specAtt = gSpecularMap.Sample(texSamp, input.texCoord);
	output.color.a = specAtt.r;

	float3 normalFromMap = gNormalMap.Sample(texSamp, input.texCoord);
	normalFromMap = 2.0f * normalFromMap - 1.0f;
	normalFromMap = mul(normalFromMap, input.tangentToWorld);
	normalFromMap = normalize(normalFromMap);

	output.normal.rgb = 0.5f * (normalFromMap + 1.0f);
	output.normal.a = specAtt.a;
	output.depth = input.depth.x / input.depth.y;

	return output;
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
