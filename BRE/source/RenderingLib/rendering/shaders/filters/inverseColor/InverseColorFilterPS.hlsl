/*******************  Data  *************************/
struct VS_OUTPUT {
	float4 PositionH : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

/*******************  Resources  *************************/
SamplerState TexSampler : register (s0);

Texture2D Texture : register (t0);

/*******************  Shader  *************************/
float4 main(const in VS_OUTPUT IN) : SV_TARGET{
	const float4 texColor = Texture.Sample(TexSampler, IN.TexCoord);
	return float4(1 - texColor.rgb, texColor.a);
}