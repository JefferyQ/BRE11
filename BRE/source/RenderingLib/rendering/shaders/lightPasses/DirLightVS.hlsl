struct Input {
	float4 PosCS : POSITION;
};

struct Output {
	float4 PosCS : SV_Position;
	float3 ViewRay : VIEW_RAY;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 InvProj;
};

Output main(const Input input) {
	Output output = (Output)0;
	output.PosCS = input.PosCS;

	// Clamp the view space position to the plane at Z = 1
	const float3 posVS = mul(input.PosCS, InvProj).xyz;
	output.ViewRay = float3(posVS.xy / posVS.z, 1.0f);
	return output;
}