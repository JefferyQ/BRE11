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
	const float3 grayScaleFactors = float3(0.299f, 0.587f, 0.114);
	const float intensity = dot(texColor.rgb, grayScaleFactors);

	return float4 (intensity.rrr, texColor.a);
}