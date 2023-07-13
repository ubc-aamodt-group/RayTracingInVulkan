
struct RayPayload
{
	float valid;
	int bounce;
	vec4 ColorAndDistance; // rgb + t
	vec4 ScatterDirection; // xyz + w (is scatter needed)
	vec3 SurfaceNormal;
	uint RandomSeed;
};
