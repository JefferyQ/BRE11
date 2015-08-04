#define NUM_LIGHTS 512

struct Input {
	uint VertexId : SV_VertexID;
};

struct Output {
	nointerpolation float4 LightPosVSAndRadius : POSITION_AND_RADIUS;
	nointerpolation float4 LightColorAndPower : LIGHT_COLOR_AND_POWER;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 View;
	float4 LightPosWAndRadius[NUM_LIGHTS];
	float4 LightColorAndPower[NUM_LIGHTS];
};

Output main(in const Input input) {
	Output output = (Output)0;
	output.LightPosVSAndRadius.xyz = mul(float4(LightPosWAndRadius[input.VertexId].xyz, 1.0f), View).xyz;
	output.LightPosVSAndRadius.w = LightPosWAndRadius[input.VertexId].w;
	output.LightColorAndPower.xyz = LightColorAndPower[input.VertexId].xyz;
	output.LightColorAndPower.w = LightColorAndPower[input.VertexId].w;
	return output;
}

