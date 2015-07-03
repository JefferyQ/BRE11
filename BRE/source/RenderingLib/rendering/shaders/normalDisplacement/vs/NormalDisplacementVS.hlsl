struct Input {
	float4 PosOS : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalOS : NORMAL;
	float3 TangentOS : TANGENT;
};

struct Output {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentOS : TANGENT;
};

cbuffer CBufferPerFrame : register (b0) {
	float TextureScaleFactor;
}

Output main(const Input input) {
	Output output = (Output)0;
	output.PosOS = input.PosOS;
	output.NormalOS = input.NormalOS;
	output.TexCoord = input.TexCoord * TextureScaleFactor;
	output.TangentOS = input.TangentOS;
	return output;
}