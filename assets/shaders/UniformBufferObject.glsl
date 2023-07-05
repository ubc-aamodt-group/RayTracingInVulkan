
struct UniformBufferObject
{
	mat4 ModelView;
	mat4 Projection;
	mat4 ModelViewInverse;
	mat4 ProjectionInverse;
	vec3 LightPosition;
	float LightRadius;
	float Aperture;
	float FocusDistance;
	float HeatmapScale;
	uint TotalNumberOfSamples;
	uint NumberOfSamples;
	uint NumberOfBounces;
	uint NumberOfShadows;
	uint RandomSeed;
	uint Width;
	uint Height;
	bool HasSky;
	bool ShowHeatmap;
};
