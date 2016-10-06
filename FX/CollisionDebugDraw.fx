float4 gColor = { 1.0f, 0.0f, 0.0f, 1.0f};
uniform extern float4x4 worldMatrix;
uniform extern float4x4 projectionMatrix;
uniform extern float4x4 viewMatrix;

float4 VertexShaderPosition(float4 position: POSITION) : POSITION
{
	return mul(float4(position.xyz, 1.0f), worldMatrix * viewMatrix * projectionMatrix);
}

float4 PixelShaderColor() : COLOR
{
	return gColor;
}

technique CollisionDebugDraw
{
	pass p0
	{
		VertexShader = compile vs_3_0 VertexShaderPosition();
		PixelShader = compile ps_3_0 PixelShaderColor();
	}
}
	