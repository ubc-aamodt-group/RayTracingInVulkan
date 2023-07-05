#pragma once

struct UserSettings final
{
	// Application
	bool Benchmark;

	// Benchmark
	bool BenchmarkNextScenes{};
	uint32_t BenchmarkMaxTime{};
	
	// Scene
	int SceneIndex;

	uint32_t Width;
	uint32_t Height;

	// Renderer
	bool IsRayTraced;
	bool AccumulateRays;
	uint32_t NumberOfSamples;
	uint32_t NumberOfBounces;
	uint32_t NumberOfShadows;
	uint32_t MaxNumberOfSamples;
	uint32_t ShaderType;

	// Camera
	float FieldOfView;
	float Aperture;
	float FocusDistance;

	// Profiler
	bool ShowHeatmap;
	float HeatmapScale;

	// UI
	bool ShowSettings;
	bool ShowOverlay;

	inline const static float FieldOfViewMinValue = 10.0f;
	inline const static float FieldOfViewMaxValue = 90.0f;

	bool RequiresAccumulationReset(const UserSettings& prev) const
	{
		return
			IsRayTraced != prev.IsRayTraced ||
			AccumulateRays != prev.AccumulateRays ||
			NumberOfBounces != prev.NumberOfBounces ||
			FieldOfView != prev.FieldOfView ||
			Aperture != prev.Aperture ||
			FocusDistance != prev.FocusDistance;
	}
};
