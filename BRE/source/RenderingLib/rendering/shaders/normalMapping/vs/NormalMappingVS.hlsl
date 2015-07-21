struct Input {
	float4 PosOS : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalOS : NORMAL;
	float3 TangentOS : TANGENT;
};

struct Output {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentVS : TANGENT;
	float3 BinormalVS : BINORMAL;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 WorldViewProj;
	float4x4 WorldView;
	float TextureScaleFactor;
}

Output main(const Input input) {
	Output output = (Output)0;
	output.PosCS = mul(input.PosOS, WorldViewProj);
	output.NormalVS = normalize(mul(float4(normalize(input.NormalOS), 0.0f), WorldView).xyz);
	output.TexCoord = input.TexCoord * TextureScaleFactor;
	output.TangentVS = normalize(mul(float4(normalize(input.TangentOS), 0.0f), WorldView).xyz);
	output.BinormalVS = normalize(cross(output.NormalVS, output.TangentVS));
	return output;
}