struct VS_INPUT {
	float4 PosCS : POSITION;
};

struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 ViewRayVS : VIEW_RAY;
};

cbuffer CBufferPerFrame : register (b0) {
	float ScreenWidth;
	float ScreenHeight;
	float FarClipPlaneDistance;
};

VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	OUT.PosCS = IN.PosCS;
	OUT.ViewRayVS = float3(ScreenHeight * OUT.PosCS.x, ScreenWidth * OUT.PosCS.y, FarClipPlaneDistance);
	return OUT;
}