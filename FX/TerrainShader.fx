#include "Lights.fx"

uniform extern float4x4 gWorld;
uniform extern float4x4 gView;
uniform extern float4x4 gProjection;

uniform extern float4x4 gPosition;

uniform extern float4 gLightDirection;

Texture2D gBlend0;
Texture2D gBlend1;
Texture2D gBlend2;
Texture2D gBlendMaster;

SamplerState TriLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_IN
{
	float3 posL : POSITION;
	float3 normalL : NORMAL;
	float2 texCoords : TEXCOORD;
};

struct VS_OUT
{
	float4 posH	: SV_POSITION;
	float shade : SHADE;
	float2 tiledUV : TEXCOORD0;
	float2 stretchedUV : TEXCOORD1;
};

VS_OUT VS(VS_IN VSin)
{
	VS_OUT VSout;
	
	VSout.posH = mul(float4(VSin.posL, 1.0f), gWorld);
	VSout.posH = mul(VSout.posH, gView);
	VSout.posH = mul(VSout.posH, gProjection);
	
	float4 normalW = mul(float4(VSin.normalL, 0.0f), gWorld);
	
	VSout.shade = saturate(max(dot(normalW, gLightDirection), 0.0f) + 0.1f);
	
	VSout.tiledUV = 16.0f * VSin.texCoords;
	VSout.stretchedUV = VSin.texCoords;
	
	return VSout;
}

float4 PS(VS_OUT PSin) : SV_Target
{
	float4 c0 = gBlend0.Sample(TriLinear, PSin.tiledUV); 
	float4 c1 = gBlend1.Sample(TriLinear, PSin.tiledUV);
	float4 c2 = gBlend2.Sample(TriLinear, PSin.tiledUV);
	
	float4 master = gBlendMaster.Sample(TriLinear, PSin.stretchedUV);

	//can also use alpha channel of the 'master' blendtexture, and also use a whole texture as the main texture.
	//this could allow for a potential 5 textures on one blend map.
	c0 *= master.r;
	c1 *= master.g;
	c2 *= master.b;

	float4 finalColor = normalize((c0 + c1 + c2));
	finalColor.a = 1.0f;
	
	//finalColor *= PSin.shade;
	return finalColor;
}

technique10 tech0
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}