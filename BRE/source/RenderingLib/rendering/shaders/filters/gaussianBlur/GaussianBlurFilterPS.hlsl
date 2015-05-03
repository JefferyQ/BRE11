/******************* Constants **********************/
#define SAMPLE_COUNT 9

/*******************  Data  *************************/
struct VS_OUTPUT {
	float4 PositionH : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

/*******************  Resources  *************************/
cbuffer CBufferSampleOffsets : register (b0) {
	float2 SampleOffsets0;
	float2 SampleOffsets1;
	float2 SampleOffsets2;
	float2 SampleOffsets3;
	float2 SampleOffsets4;
	float2 SampleOffsets5;
	float2 SampleOffsets6;
	float2 SampleOffsets7;
	float2 SampleOffsets8;
};

cbuffer CBufferSampleWeights : register (b1) {
	float SampleWeights0;
	float SampleWeights1;
	float SampleWeights2;
	float SampleWeights3;
	float SampleWeights4;
	float SampleWeights5;
	float SampleWeights6;
	float SampleWeights7;
	float SampleWeights8;
};

SamplerState TexSampler : register (s0);

Texture2D Texture : register (t0);

/*******************  Shader  *************************/
float4 main(const in VS_OUTPUT IN) : SV_TARGET{
	float4 color = (float4)0;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets0) * SampleWeights0;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets1) * SampleWeights1;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets2) * SampleWeights2;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets3) * SampleWeights3;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets4) * SampleWeights4;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets5) * SampleWeights5;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets6) * SampleWeights6;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets7) * SampleWeights7;
	color += Texture.Sample(TexSampler, IN.TexCoord + SampleOffsets8) * SampleWeights8;

	return color;
}