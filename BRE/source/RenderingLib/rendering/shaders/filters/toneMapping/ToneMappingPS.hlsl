#include <rendering/shaders/Lighting.hlsli>

struct Input {
	float4 PositionH : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

SamplerState TexSampler : register (s0);

Texture2D Texture : register (t0);

float3 HableToneMap(float3 color) {
	float A = 0.22;//Shoulder Strength
	float B = 0.30;//Linear Strength
	float C = 0.10;//Linear Angle
	float D = 0.20;//Toe Strength
	float E = 0.01;//Toe Numerator
	float F = 0.30;//Toe Denominator

	color = max(0, color - 0.004f);
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - (E / F);
	return color;
}

float4 main(const in Input input) : SV_TARGET{
	const float3 color = Texture.Sample(TexSampler, input.TexCoord).rgb;
	return float4(accurateLinearToSRGB(HableToneMap(color)), 1.0f);
}