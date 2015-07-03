#define NUM_LIGHTS 512

struct Input {
	uint VertexId : SV_VertexID;
};

struct Output {
	nointerpolation float3 LightCenterPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 View;
	float4 LightPosWAndRadius[NUM_LIGHTS];
	float4 LightColor[NUM_LIGHTS];
};

Output main(in const Input input) {
	Output output = (Output)0;
	output.LightCenterPosVS = mul(float4(LightPosWAndRadius[input.VertexId].xyz, 1.0f), View).xyz;
	output.LightColorAndRadius.xyz = LightColor[input.VertexId].xyz;
	output.LightColorAndRadius.w = LightPosWAndRadius[input.VertexId].w;
	return output;
}

