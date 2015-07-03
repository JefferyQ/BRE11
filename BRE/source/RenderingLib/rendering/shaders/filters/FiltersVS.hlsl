struct Input {
	float4 PositionH : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct Output {
	float4 PositionH : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

Output main(const Input input) {
	Output output = (Output)0;
	output.PositionH = input.PositionH;
	output.TexCoord = input.TexCoord;
	return output;
}