/*******************  Data  *************************/
struct VS_OUTPUT {
	float4 PositionH : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

static const float3x3 gSepiaFilter = { 0.393f, 0.349f, 0.272f,
									  0.769f, 0.686f, 0.534f,
									  0.189f, 0.168f, 0.131f };

/*******************  Resources  *************************/
SamplerState TexSampler : register (s0);

Texture2D Texture : register (t0);

/*******************  Shader  *************************/
float4 main(const in VS_OUTPUT IN) : SV_TARGET{
	const float4 texColor = Texture.Sample(TexSampler, IN.TexCoord);
	return float4(mul(texColor.rgb, gSepiaFilter), texColor.a);
}