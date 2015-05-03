#define UTILS_HEADER

// Helper functions to compress/decompress a normal vector
// using octahedron-normal vectors
float2 OctWrap(float2 v) {
	return (1.0f - abs(v.yx)) * (v.xy >= 0.0f ? 1.0f : -1.0f);
}

float2 Encode(float3 n) {
	n /= (abs(n.x) + abs(n.y) + abs(n.z));
	n.xy = n.z >= 0.0f ? n.xy : OctWrap(n.xy);
	n.xy = n.xy * 0.5f + 0.5f;
	return n.xy;
}

float3 Decode(float2 encN) {
	encN = encN * 2.0f - 1.0f;

	float3 n;
	n.z = 1.0f - abs(encN.x) - abs(encN.y);
	n.xy = n.z >= 0.0f ? encN.xy : OctWrap(encN.xy);
	n = normalize(n);
	return n;
}