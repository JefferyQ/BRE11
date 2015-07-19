struct Input {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
};

struct Output {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 WorldViewProj;
	float4x4 WorldView;
}

Output main(const Input input) {
	Output output = (Output)0;
	output.PosCS = mul(input.PosOS, WorldViewProj);
	output.NormalVS = normalize(mul(float4(normalize(input.NormalOS), 0.0f), WorldView).xyz);
	return output;
}