#include "SceneList.hpp"
#include "Assets/Material.hpp"
#include "Assets/Model.hpp"
#include "Assets/Texture.hpp"
#include <functional>
#include <random>
#include <filesystem>
#include <iostream>
#include <fstream>

using namespace glm;
using Assets::Material;
using Assets::Model;
using Assets::Texture;

namespace fs = std::filesystem;

namespace
{

	void AddRayTracingInOneWeekendCommonScene(std::vector<Assets::Model>& models, const bool& isProc, std::function<float ()>& random)
	{
		// Common models from the final scene from Ray Tracing In One Weekend book. Only the three central spheres are missing.
		// Calls to random() are always explicit and non-inlined to avoid C++ undefined evaluation order of function arguments,
		// this guarantees consistent and reproducible behaviour across different platforms and compilers.

		models.push_back(Model::CreateSphere(vec3(0, -1000, 0), 1000, Material::Lambertian(vec3(0.5f, 0.5f, 0.5f)), isProc));

		for (int i = -11; i < 11; ++i)
		{
			for (int j = -11; j < 11; ++j)
			{
				const float chooseMat = random();
				const float center_y = static_cast<float>(j) + 0.9f * random();
				const float center_x = static_cast<float>(i) + 0.9f * random();
				const vec3 center(center_x, 0.2f, center_y);

				if (length(center - vec3(4, 0.2f, 0)) > 0.9f)
				{
					if (chooseMat < 0.8f) // Diffuse
					{
						const float b = random() * random();
						const float g = random() * random();
						const float r = random() * random();

						models.push_back(Model::CreateSphere(center, 0.2f, Material::Lambertian(vec3(r, g, b)), isProc));
					}
					else if (chooseMat < 0.95f) // Metal
					{
						const float fuzziness = 0.5f * random();
						const float b = 0.5f * (1 + random());
						const float g = 0.5f * (1 + random());
						const float r = 0.5f * (1 + random());

						models.push_back(Model::CreateSphere(center, 0.2f, Material::Metallic(vec3(r, g, b), fuzziness), isProc));
					}
					else // Glass
					{
						models.push_back(Model::CreateSphere(center, 0.2f, Material::Dielectric(1.5f), isProc));
					}
				}
			}
		}
	}

}

const std::vector<std::pair<std::string, std::function<SceneAssets (SceneList::CameraInitialSate&)>>> SceneList::AllScenes =
{
	{"Cube And Spheres", CubeAndSpheres},
	{"Ray Tracing In One Weekend", RayTracingInOneWeekend},
	{"Planets In One Weekend", PlanetsInOneWeekend},
	{"Lucy In One Weekend", LucyInOneWeekend},
	{"Cornell Box", CornellBox},
	{"Cornell Box & Lucy", CornellBoxLucy},
<<<<<<< HEAD
	{"Cubes and Common Scene", CubesAndCommonScene},
	{"Cylinder and Common Scene", CylinderCubesCommonScene},
	{"TreesAndGrass", TreesAndGrass},
	{"blender_2_77 Racing Car", blender_2_77},
	{"blender_2_78 Procedural", blender_2_78},
	{"blender_2_80 Spring", blender_2_80},
	{"blender_2_83 PartyTug", blender_2_83},
	{"blender_2_90 Splash Fox", blender_2_90},
	{"blender_2_91 Red Autumn Forest", blender_2_91},
	{"blender_3_2 White Lands", blender_3_2},
	{"TestScene", TestScene},
=======
	{"Simple Test", SimpleTest},
	{"Bunny", Bunny},
	{"Carnival", Carnival},
	{"Ship", Ship},
>>>>>>> lavapipe-investigation
};

SceneAssets SceneList::CubeAndSpheres(CameraInitialSate& camera)
{
	// Basic test scene.
	
	camera.ModelView = translate(mat4(1), vec3(0, 0, -2));
	camera.FieldOfView = 90;
	camera.Aperture = 0.05f;
	camera.FocusDistance = 2.0f;
	camera.ControlSpeed = 2.0f;
	camera.GammaCorrection = false;
	camera.HasSky = true;

	std::vector<Model> models;
	std::vector<Texture> textures;

	models.push_back(Model::LoadModel("../assets/models/cube_multi.obj"));
	models.push_back(Model::CreateSphere(vec3(1, 0, 0), 0.5, Material::Metallic(vec3(0.7f, 0.5f, 0.8f), 0.2f), true));
	models.push_back(Model::CreateSphere(vec3(-1, 0, 0), 0.5, Material::Dielectric(1.5f), true));
	models.push_back(Model::CreateSphere(vec3(0, 1, 0), 0.5, Material::Lambertian(vec3(1.0f), 0), true));

	textures.push_back(Texture::LoadTexture("../assets/textures/land_ocean_ice_cloud_2048.png", Vulkan::SamplerConfig()));

	return std::forward_as_tuple(std::move(models), std::move(textures));
}

SceneAssets SceneList::RayTracingInOneWeekend(CameraInitialSate& camera)
{
	// Final scene from Ray Tracing In One Weekend book.
	
	camera.ModelView = lookAt(vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.FieldOfView = 20;
	camera.Aperture = 0.1f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	AddRayTracingInOneWeekendCommonScene(models, isProc, random);

	models.push_back(Model::CreateSphere(vec3(0, 1, 0), 1.0f, Material::Dielectric(1.5f), isProc));
	models.push_back(Model::CreateSphere(vec3(-4, 1, 0), 1.0f, Material::Lambertian(vec3(0.4f, 0.2f, 0.1f)), isProc));
	models.push_back(Model::CreateSphere(vec3(4, 1, 0), 1.0f, Material::Metallic(vec3(0.7f, 0.6f, 0.5f), 0.0f), isProc));

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::PlanetsInOneWeekend(CameraInitialSate& camera)
{
	// Same as RayTracingInOneWeekend but using textures.
	
	camera.ModelView = lookAt(vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.FieldOfView = 20;
	camera.Aperture = 0.1f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;
	std::vector<Texture> textures;

	AddRayTracingInOneWeekendCommonScene(models, isProc, random);

	models.push_back(Model::CreateSphere(vec3(0, 1, 0), 1.0f, Material::Metallic(vec3(1.0f), 0.1f, 2), isProc));
	models.push_back(Model::CreateSphere(vec3(-4, 1, 0), 1.0f, Material::Lambertian(vec3(1.0f), 0), isProc));
	models.push_back(Model::CreateSphere(vec3(4, 1, 0), 1.0f, Material::Metallic(vec3(1.0f), 0.0f, 1), isProc));

	textures.push_back(Texture::LoadTexture("../assets/textures/2k_mars.jpg", Vulkan::SamplerConfig()));
	textures.push_back(Texture::LoadTexture("../assets/textures/2k_moon.jpg", Vulkan::SamplerConfig()));
	textures.push_back(Texture::LoadTexture("../assets/textures/land_ocean_ice_cloud_2048.png", Vulkan::SamplerConfig()));

	return std::forward_as_tuple(std::move(models), std::move(textures));
}

SceneAssets SceneList::LucyInOneWeekend(CameraInitialSate& camera)
{
	// Same as RayTracingInOneWeekend but using the Lucy 3D model.
	
	camera.ModelView = lookAt(vec3(13, 2, 3), vec3(0, 1.0, 0), vec3(0, 1, 0));
	camera.FieldOfView = 20;
	camera.Aperture = 0.05f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;
	
	AddRayTracingInOneWeekendCommonScene(models, isProc, random);

	auto lucy0 = Model::LoadModel("../assets/models/lucy.obj");
	auto lucy1 = lucy0;
	auto lucy2 = lucy0;

	const auto i = mat4(1);
	const float scaleFactor = 0.0035f;

	lucy0.Transform(
		rotate(
			scale(
				translate(i, vec3(0, -0.08f, 0)), 
				vec3(scaleFactor)),
			radians(90.0f), vec3(0, 1, 0)));

	lucy1.Transform(
		rotate(
			scale(
				translate(i, vec3(-4, -0.08f, 0)),
				vec3(scaleFactor)),
			radians(90.0f), vec3(0, 1, 0)));

	lucy2.Transform(
		rotate(
			scale(
				translate(i, vec3(4, -0.08f, 0)),
				vec3(scaleFactor)),
			radians(90.0f), vec3(0, 1, 0)));

	lucy0.SetMaterial(Material::Dielectric(1.5f));
	lucy1.SetMaterial(Material::Lambertian(vec3(0.4f, 0.2f, 0.1f)));
	lucy2.SetMaterial(Material::Metallic(vec3(0.7f, 0.6f, 0.5f), 0.05f));

	models.push_back(std::move(lucy0));
	models.push_back(std::move(lucy1));
	models.push_back(std::move(lucy2));

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::CornellBox(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(278, 278, 800), vec3(278, 278, 0), vec3(0, 1, 0));
	camera.FieldOfView = 40;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 500.0f;
	camera.GammaCorrection = true;
	camera.HasSky = false;

	const auto i = mat4(1);
	const auto white = Material::Lambertian(vec3(0.73f, 0.73f, 0.73f));

	auto box0 = Model::CreateBox(vec3(0, 0, -165), vec3(165, 165, 0), white);
	auto box1 = Model::CreateBox(vec3(0, 0, -165), vec3(165, 330, 0), white);

	box0.Transform(rotate(translate(i, vec3(555 - 130 - 165, 0, -65)), radians(-18.0f), vec3(0, 1, 0)));
	box1.Transform(rotate(translate(i, vec3(555 - 265 - 165, 0, -295)), radians(15.0f), vec3(0, 1, 0)));

	std::vector<Model> models;
	models.push_back(Model::CreateCornellBox(555));
	models.push_back(box0);
	models.push_back(box1);

	return std::make_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::Carnival(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(275, 60, -700), vec3(225, 120, -1000), vec3(0, 1, 0));
	camera.FieldOfView = 50;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 500.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const auto i = mat4(1);

	auto carnival0 = Model::LoadModel("../assets/models/TheCarnival.obj");
	carnival0.Transform(
		rotate(
			scale(
				translate(i, vec3(400, 20, 150)),
				vec3(0.2f)),
			radians(0.0f), vec3(0, 1, 0)));
	std::vector<Model> models;
	models.push_back(carnival0);

	return std::make_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::Ship(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(378, 278, 500), vec3(178, 278, 0), vec3(0, 1, 0));
	camera.FieldOfView = 50;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 500.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const auto i = mat4(1);

	auto ship0 = Model::LoadModel("../assets/models/karimSchooner.obj");
	printf("Loading ship...\n");
	ship0.Transform(
		rotate(
			scale(
				translate(i, vec3(555 - 300 - 165/2, -9, -295 - 165/2)),
				vec3(100.0f)),
			radians(75.0f), vec3(0, 1, 0)));
	std::vector<Model> models;
	models.push_back(ship0);

	return std::make_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::Bunny(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(278, 278, 800), vec3(278, 278, 0), vec3(0, 1, 0));
	camera.FieldOfView = 40;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 500.0f;
	camera.GammaCorrection = true;
	camera.HasSky = false;

	const auto i = mat4(1);

	auto bunny0 = Model::LoadModel("../assets/models/bunny.obj");
	bunny0.Transform(
		rotate(
			scale(
				translate(i, vec3(555 - 300 - 165/2, 5, -225 - 165/2)),
				vec3(200.0f)),
			radians(75.0f), vec3(0, 1, 0)));
	std::vector<Model> models;
	models.push_back(Model::CreateCornellBox(555));
	models.push_back(bunny0);

	return std::make_tuple(std::move(models), std::vector<Texture>());
}


SceneAssets SceneList::SimpleTest(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(278, 278, 800), vec3(278, 278, 0), vec3(0, 1, 0));
	camera.FieldOfView = 40;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 500.0f;
	camera.GammaCorrection = true;
	camera.HasSky = false;

	const auto i = mat4(1);

	std::vector<Model> models;
	models.push_back(Model::CreateSquare(555));

	return std::make_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::CornellBoxLucy(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(278, 278, 800), vec3(278, 278, 0), vec3(0, 1, 0));
	camera.FieldOfView = 40;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 500.0f;
	camera.GammaCorrection = true;
	camera.HasSky = false;

	const auto i = mat4(1);
	const auto sphere = Model::CreateSphere(vec3(555 - 130, 165.0f, -165.0f / 2 - 65), 80.0f, Material::Dielectric(1.5f), true);
	auto lucy0 = Model::LoadModel("../assets/models/lucy.obj");

	lucy0.Transform(
		rotate(
			scale(
				translate(i, vec3(555 - 300 - 165/2, -9, -295 - 165/2)),
				vec3(0.6f)),
			radians(75.0f), vec3(0, 1, 0)));

	std::vector<Model> models;
	models.push_back(Model::CreateCornellBox(555));
	models.push_back(sphere);
	models.push_back(lucy0);

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::CubesAndCommonScene(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.FieldOfView = 20;
	camera.Aperture = 0.1f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	AddRayTracingInOneWeekendCommonScene(models, isProc, random);

	// Procedural Cubes
	for (int i = -30; i < 30; ++i)
	{
		for (int j = -30; j < 30; ++j)
		{
			// if (abs(i) <= 11 && abs(j) <= 11)
			// 	continue;

			const float chooseMat = random();
			const float center_y = static_cast<float>(j) + 0.9f * random();
			const float center_x = static_cast<float>(i) + 0.9f * random();
			const vec3 center(center_x, 0.2f, center_y);

			if (length(center - vec3(4, 0.2f, 0)) > 0.9f)
			{
				if (chooseMat < 0.8f) // Diffuse
				{
					const float b = random() * random();
					const float g = random() * random();
					const float r = random() * random();

					models.push_back(Model::CreateCube(center, 0.2f, Material::Lambertian(vec3(r, g, b)), isProc));
				}
				else if (chooseMat < 0.95f) // Metal
				{
					const float fuzziness = 0.5f * random();
					const float b = 0.5f * (1 + random());
					const float g = 0.5f * (1 + random());
					const float r = 0.5f * (1 + random());

					models.push_back(Model::CreateCube(center, 0.2f, Material::Metallic(vec3(r, g, b), fuzziness), isProc));
				}
				else // Glass
				{
					models.push_back(Model::CreateCube(center, 0.2f, Material::Dielectric(1.5f), isProc));
				}
			}
		}
	}

	// models.push_back(Model::CreateSphere(vec3(0, 1, 0), 1.0f, Material::Dielectric(1.5f), isProc));
	// models.push_back(Model::CreateSphere(vec3(-4, 1, 0), 1.0f, Material::Lambertian(vec3(0.4f, 0.2f, 0.1f)), isProc));
	// models.push_back(Model::CreateSphere(vec3(4, 1, 0), 1.0f, Material::Metallic(vec3(0.7f, 0.6f, 0.5f), 0.0f), isProc));

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}


SceneAssets SceneList::CylinderCubesCommonScene(CameraInitialSate& camera)
{
	camera.ModelView = lookAt(vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.FieldOfView = 20;
	camera.Aperture = 0.1f;
	camera.FocusDistance = 10.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	AddRayTracingInOneWeekendCommonScene(models, isProc, random);

	// Procedural Cylinder and Cubes
	for (int i = -30; i < 30; ++i)
	{
		for (int j = -30; j < 30; ++j)
		{
			// if (abs(i) <= 11 && abs(j) <= 11)
			// 	continue;

			const float type = random();
			const float chooseMat = random();
			const float center_y = static_cast<float>(j) + 0.9f * random();
			const float center_x = static_cast<float>(i) + 0.9f * random();
			const vec3 center(center_x, 0.2f, center_y);

			if(type <= 0.5)
			{
				if (length(center - vec3(4, 0.2f, 0)) > 0.9f)
				{
					if (chooseMat < 0.8f) // Diffuse
					{
						const float b = random() * random();
						const float g = random() * random();
						const float r = random() * random();

						models.push_back(Model::CreateCube(center, 0.2f, Material::Lambertian(vec3(r, g, b)), isProc));
					}
					else if (chooseMat < 0.95f) // Metal
					{
						const float fuzziness = 0.5f * random();
						const float b = 0.5f * (1 + random());
						const float g = 0.5f * (1 + random());
						const float r = 0.5f * (1 + random());

						models.push_back(Model::CreateCube(center, 0.2f, Material::Metallic(vec3(r, g, b), fuzziness), isProc));
					}
					else // Glass
					{
						models.push_back(Model::CreateCube(center, 0.2f, Material::Dielectric(1.5f), isProc));
					}
				}
			}
			else
			{
				if (length(center - vec3(4, 0.2f, 0)) > 0.9f)
				{
					if (chooseMat < 0.8f)
					{
						const float b = random() * random();
						const float g = random() * random();
						const float r = random() * random();

						models.push_back(Model::CreateCylinder(center, 0.2f, Material::DiffuseLight(vec3(r, g, b)), isProc));
					}
					else
					{
						const float b = 0.5f * (1 + random());
						const float g = 0.5f * (1 + random());
						const float r = 0.5f * (1 + random());

						models.push_back(Model::CreateCylinder(center, 0.2f, Material::DiffuseLight(vec3(r, g, b)), isProc));
					}
				}
			}
		}
	}

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

// SceneAssets SceneList::TestScene(CameraInitialSate& camera)
// {
// 	// Final scene from Ray Tracing In One Weekend book.
	
// 	camera.ModelView = lookAt(vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0));
// 	camera.FieldOfView = 20;
// 	camera.Aperture = 0.1f;
// 	camera.FocusDistance = 10.0f;
// 	camera.ControlSpeed = 5.0f;
// 	camera.GammaCorrection = true;
// 	camera.HasSky = true;

// 	const bool isProc = true;

// 	std::mt19937 engine(42);
// 	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

// 	std::vector<Model> models;

// 	models.push_back(Model::CreateSphere(vec3(0, -1000, 0), 1000, Material::Lambertian(vec3(0.5f, 0.5f, 0.5f)), isProc));


// 	// stuff I added
// 	const auto i = mat4(1);

// 	auto human = Model::LoadModel("/home/mrs/vulkan-samples/obj-files/human-model/FinalBaseMesh.obj");
// 	human.Transform(
// 			rotate(
// 				scale(
// 					translate(i, vec3(2, 0, 0))
// 				, vec3(0.05)),
// 			radians(90.0f), vec3(0, 1, 0)));
// 	models.push_back(human);
	
// 	auto Tree1 = Model::LoadModel("/home/mrs/vulkan-samples/obj-files/three-tree/Tree-1.obj");
// 	Tree1.Transform(
// 			rotate(
// 				scale(
// 					translate(i, vec3(-2, 0, -3))
// 				, vec3(0.25)),
// 			radians(180.0f), vec3(0, 1, 0)));
// 	models.push_back(Tree1);

// 	auto Tree2 = Model::LoadModel("/home/mrs/vulkan-samples/obj-files/three-tree/Tree-2.obj");
// 	Tree2.Transform(
// 			rotate(
// 				scale(
// 					translate(i, vec3(0, 0, 0))
// 				, vec3(0.25)),
// 			radians(180.0f), vec3(0, 1, 0)));
// 	models.push_back(Tree2);

// 	auto Tree3 = Model::LoadModel("/home/mrs/vulkan-samples/obj-files/three-tree/Tree-3.obj");
// 	Tree3.Transform(
// 			rotate(
// 				scale(
// 					translate(i, vec3(2, 0, 2))
// 				, vec3(0.25)),
// 			radians(180.0f), vec3(0, 1, 0)));
// 	models.push_back(Tree3);

// 	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
// }

SceneAssets SceneList::TreesAndGrass(CameraInitialSate& camera)
{
	// Final scene from Ray Tracing In One Weekend book.
	
	camera.ModelView = lookAt(vec3(2, 0.5, 0.75), vec3(0, 0.3, 0), vec3(0, 1, 0));
	camera.FieldOfView = 30;
	camera.Aperture = 0.0001f;
	camera.FocusDistance = 2.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	models.push_back(Model::CreateSphere(vec3(0, -1000, 0), 1000, Material::Lambertian(vec3(0.5f, 0.5f, 0.5f)), isProc));


	// stuff I added
	const auto i = mat4(1);

	std::string path = "../../../Scenes/TreesAndGrass";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());
			model.Transform(scale(i, vec3(0.1)));

			if(entry.path().string().find("leaves") != std::string::npos || entry.path().string().find("grass") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(124.0 / 256, 252.0 / 256, 0.0)));
			}
			else if(entry.path().string().find("mountain") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(136.0 / 256, 140.0 / 256, 141.0 / 256)));
			}
			else if(entry.path().string().find("ground") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(155.0 / 256, 118.0 / 256, 83.0 / 256)));
			}
			else if(entry.path().string().find("tree") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(114.0 / 256, 92.0 / 256, 66.0 / 256)));
			}
			else if(entry.path().string().find("human") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(197.0 / 256, 140.0 / 256, 133.0 / 256)));
			}
			else if(entry.path().string().find("Benz") != std::string::npos)
			{
				model.SetAllMaterial(Material::Metallic(vec3(200.0 / 256, 200.0 / 256, 200.0 / 256), 0.2));
			}

			models.push_back(model);
		}
	}

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::blender_2_77(CameraInitialSate& camera)
{
	camera.FieldOfView = 30;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 7.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;


	// models.push_back(Model::CreateSphere(vec3(0.0, 10.0, 0.0), 1.0f, Material::Lambertian(vec3(1.0, 0.0, 0.0)), isProc));
	// models.push_back(Model::CreateSphere(vec3(3.0, 10.0, 0.0), 1.0f, Material::Lambertian(vec3(0.0, 1.0, 0.0)), isProc));
	// models.push_back(Model::CreateSphere(vec3(0.0, 10.0, 3.0), 1.0f, Material::Lambertian(vec3(0.0, 0.0, 1.0)), isProc));

	// stuff I added
	const auto i = mat4(1);

	std::string path = "../../../Scenes/Blender_2.77";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());

			if(model.Vertices().size() == 0)
				continue;

			models.push_back(model);
		}
		else if(entry.path().extension() == ".camera")
		{
			std::ifstream fin(entry.path().string());
			
			float eye[3], center[3];
			fin >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2];
			camera.ModelView = lookAt(vec3(eye[0], eye[1], eye[2]), vec3(center[0], center[1], center[2]), vec3(0, 1, 0));

			fin.close();
		}
	}

	std::cout << "done loading" << std::endl;

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::blender_2_78(CameraInitialSate& camera)
{
	camera.FieldOfView = 30;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 7.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	const auto i = mat4(1);

	std::string path = "../../../Scenes/Blender_2.78";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());
			models.push_back(model);
		}
		else if(entry.path().extension() == ".camera")
		{
			std::ifstream fin(entry.path().string());
			
			float eye[3], center[3];
			fin >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2];
			camera.ModelView = lookAt(vec3(eye[0], eye[1], eye[2]), vec3(center[0], center[1], center[2]), vec3(0, 1, 0));

			fin.close();
		}
	}

	std::cout << "done loading" << std::endl;

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::blender_2_80(CameraInitialSate& camera)
{
	camera.FieldOfView = 20;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 7.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	const auto i = mat4(1);

	std::string path = "../../../Scenes/Blender_2.80";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());

			if(model.Vertices().size() == 0)
				continue;
			
			if(entry.path().string().find("Dirt_grass") != std::string::npos)
				continue;

			if(entry.path().string().find("spring_body") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(223.0 / 256, 175.0 / 256, 171.0 / 256)));
			}
			else if(entry.path().string().find("stitches") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(92.0 / 256, 64.0 / 256, 51.0 / 256)));
			}
			else if(entry.path().string().find("spring_jacket") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(163.0 / 256, 67.0 / 256, 42.0 / 256)));
			}
			else if(entry.path().string().find("spring_pants") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(92.0 / 256, 74.0 / 256, 101.0 / 256)));
			}
			else if(entry.path().string().find("spring_boots") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(150.0 / 256, 106.0 / 256, 86.0 / 256)));
			}
			else if(entry.path().string().find("spring_hairband") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(69.0 / 256, 23.0 / 256, 8.0 / 256)));
			}
			else if(entry.path().string().find("spring_hair") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(108.0 / 256, 86.0 / 256, 99.0 / 256)));
			}
			else if(entry.path().string().find("spring_scarf") != std::string::npos || entry.path().string().find("spring_pullover") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(114.0 / 256, 76.0 / 256, 64.0 / 256)));
			}

			

			models.push_back(model);
		}
		else if(entry.path().extension() == ".camera")
		{
			std::ifstream fin(entry.path().string());
			
			float eye[3], center[3];
			fin >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2];
			camera.ModelView = lookAt(vec3(eye[0], eye[1], eye[2]), vec3(center[0], center[1], center[2]), vec3(0, 1, 0));

			fin.close();
		}
	}

	std::cout << "done loading" << std::endl;

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::blender_2_83(CameraInitialSate& camera)
{
	// camera.ModelView = lookAt(vec3(-63.8804, 4.68381, 59.8617), vec3(16.7533, 7.33571, -15.7229), vec3(0, 1, 0));
	camera.FieldOfView = 20;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 7.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	const auto i = mat4(1);

	std::string path = "../../../Scenes/Blender_2.83";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());
			models.push_back(model);
		}
		else if(entry.path().extension() == ".camera")
		{
			std::ifstream fin(entry.path().string());
			
			float eye[3], center[3];
			fin >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2];
			camera.ModelView = lookAt(vec3(eye[0], eye[1], eye[2]), vec3(center[0], center[1], center[2]), vec3(0, 1, 0));

			fin.close();
		}
	}

	std::cout << "done loading" << std::endl;

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::blender_2_90(CameraInitialSate& camera)
{
	camera.FieldOfView = 30;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 7.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	const auto i = mat4(1);

	std::string path = "../../../Scenes/Blender_2.90";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());
			models.push_back(model);
		}
		else if(entry.path().extension() == ".camera")
		{
			std::ifstream fin(entry.path().string());
			
			float eye[3], center[3];
			fin >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2];
			camera.ModelView = lookAt(vec3(eye[0], eye[1], eye[2]), vec3(center[0], center[1], center[2]), vec3(0, 1, 0));

			fin.close();
		}
	}

	std::cout << "done loading" << std::endl;

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::blender_2_91(CameraInitialSate& camera)
{
	camera.FieldOfView = 30;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 7.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	const auto i = mat4(1);

	std::string path = "../../../Scenes/Blender_2.91";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());

			if(entry.path().string().find("Character2_Shirt") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(156.0 / 256, 74.0 / 256, 61.0 / 256)));
			}
			else if(entry.path().string().find("Character_Hair") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(119.0 / 256, 74.0 / 256, 55.0 / 256)));
			}
			else if(entry.path().string().find("Character_Pants") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(131.0 / 256, 115.0 / 256, 160.0 / 256)));
			}
			else if(entry.path().string().find("Character_Hair") != std::string::npos || entry.path().string().find("Character_hair_2") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(121.0 / 256, 74.0 / 256, 56.0 / 256)));
			}
			else if(entry.path().string().find("Character_watch") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(163.0 / 256, 73.0 / 256, 78.0 / 256)));
			}
			else if(entry.path().string().find("Character") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(223.0 / 256, 187.0 / 256, 161.0 / 256)));
			}

			else if(entry.path().string().find("backpackStraps") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(104.0 / 256, 66.0 / 256, 64.0 / 256)));
			}
			else if(entry.path().string().find("BackPackDec") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(108.0 / 256, 191.0 / 256, 210.0 / 256)));
			}
			else if(entry.path().string().find("BackPack") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(97.0 / 256, 85.0 / 256, 85.0 / 256)));
			}

			else if(entry.path().string().find("ForeGround_terrain") != std::string::npos)
			{
				model.SetMaterial(Material::Lambertian(vec3(212.0 / 256, 207.0 / 256, 207.0 / 256)), 0);
				model.SetMaterial(Material::Lambertian(vec3(212.0 / 256, 207.0 / 256, 207.0 / 256)), 1);
				model.SetMaterial(Material::Lambertian(vec3(220.0 / 256, 139.0 / 256, 27.0 / 256)), 2);
			}
			else if(entry.path().string().find("MidGround") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(232.0 / 256, 189.0 / 256, 26.0 / 256)));
			}

			else if(entry.path().string().find("FlowerParticles") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(212.0 / 256, 207.0 / 256, 207.0 / 256)));
			}

			else if(entry.path().string().find("Mesh") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(142.0 / 256, 85.0 / 256, 47.0 / 256)));
			}
			else if(entry.path().string().find("bush") != std::string::npos)
			{
				float chooseColor = random();
				if(chooseColor < 0.3)
					model.SetAllMaterial(Material::Lambertian(vec3(175.0 / 256, 195.0 / 256, 62.0 / 256)));
				else if(chooseColor < 0.6)
					model.SetAllMaterial(Material::Lambertian(vec3(224.0 / 256, 70.0 / 256, 45.0 / 256)));
				else
					model.SetAllMaterial(Material::Lambertian(vec3(236.0 / 256, 186.0 / 256, 85.0 / 256)));
			}

			models.push_back(model);
		}
		else if(entry.path().extension() == ".camera")
		{
			std::ifstream fin(entry.path().string());
			
			float eye[3], center[3];
			fin >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2];
			camera.ModelView = lookAt(vec3(eye[0], eye[1], eye[2]), vec3(center[0], center[1], center[2]), vec3(0, 1, 0));

			fin.close();
		}
	}

	std::cout << "done loading" << std::endl;

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::blender_3_2(CameraInitialSate& camera)
{
	// camera.ModelView = lookAt(vec3(1.1334, -1.3, 13.2851), vec3(-4.44416, -2.71126, 12.7306), vec3(0, 1, 0));
	camera.FieldOfView = 25;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 7.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	const auto i = mat4(1);

	std::string path = "../../../Scenes/Blender_3.2";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());

			if(entry.path().string().find("boat") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(150.0 / 256, 111.0 / 256, 51.0 / 256)));
			}
			else if(entry.path().string().find("water") != std::string::npos)
			{
				// model.SetAllMaterial(Material::Lambertian(vec3(18.0 / 256, 109.0 / 256, 105.0 / 256)));
				model.SetAllMaterial(Material::Metallic(vec3(18.0 / 256, 109.0 / 256, 105.0 / 256), 0.6));
			}
			else if(entry.path().string().find("Landscape") != std::string::npos)
			{
				model.SetAllMaterial(Material::Lambertian(vec3(250.0 / 256, 250.0 / 256, 245.0 / 256)));
			}

			models.push_back(model);
		}
		else if(entry.path().extension() == ".camera")
		{
			std::ifstream fin(entry.path().string());
			
			float eye[3], center[3];
			fin >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2];
			camera.ModelView = lookAt(vec3(eye[0], eye[1], eye[2]), vec3(center[0], center[1], center[2]), vec3(0, 1, 0));

			fin.close();
		}
	}

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

SceneAssets SceneList::TestScene(CameraInitialSate& camera)
{
	// camera.ModelView = lookAt(vec3(-0.0, 1.6, 1.2), vec3(0.0, 1.24307, -5.23752), vec3(0, 1, 0));
	// camera.ModelView = lookAt(vec3(-0.35024330019950867, -1.0811127424240112, 1.1617968082427979), vec3(0.0, 1.24307, -5.23752), vec3(0, 1, 0));
	// camera.ModelView = lookAt(vec3(-0.35024330019950867, 1.1617968082427979, 1.0811127424240112), vec3(-0.06039896607398987, 1.5020394325256348, 0.18655967712402344), vec3(0, 1, 0));
	// camera.ModelView = lookAt(vec3(-25.9075, 2.54455, 61.7621), vec3(-16.8204, 3.49817, 29.8268), vec3(0, 1, 0));
	camera.FieldOfView = 30;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 7.0f;
	camera.ControlSpeed = 5.0f;
	camera.GammaCorrection = true;
	camera.HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Model> models;

	// models.push_back(Model::CreateSphere(vec3(0, -1010, 0), 1000, Material::Lambertian(vec3(0.5f, 0.5f, 0.5f)), isProc));

	// stuff I added
	const auto i = mat4(1);

	std::string path = "../../../Scenes/TestScene";
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.path().extension() == ".obj")
		{
			auto model = Model::LoadModel(entry.path());
			models.push_back(model);
		}
		else if(entry.path().extension() == ".camera")
		{
			std::ifstream fin(entry.path().string());
			
			float eye[3], center[3];
			fin >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2];
			camera.ModelView = lookAt(vec3(eye[0], eye[1], eye[2]), vec3(center[0], center[1], center[2]), vec3(0, 1, 0));

			fin.close();
		}
	}

	std::cout << "done loading" << std::endl;

	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
}

// SceneAssets SceneList::blender_3_3(CameraInitialSate& camera)
// {
// 	// Final scene from Ray Tracing In One Weekend book.
	
// 	camera.ModelView = lookAt(vec3(563.448, 697.095, -556.87), vec3(361.606, 110.776, 117.695), vec3(0, 1, 0));
// 	camera.FieldOfView = 30;
// 	camera.Aperture = 0.0f;
// 	camera.FocusDistance = 700.0f;
// 	camera.ControlSpeed = 5.0f;
// 	camera.GammaCorrection = true;
// 	camera.HasSky = true;

// 	const bool isProc = true;

// 	std::mt19937 engine(42);
// 	std::function<float ()> random = std::bind(std::uniform_real_distribution<float>(), engine);

// 	std::vector<Model> models;

// 	// models.push_back(Model::CreateSphere(vec3(0, -1000, 0), 1000, Material::Lambertian(vec3(0.5f, 0.5f, 0.5f)), isProc));


// 	// stuff I added
// 	const auto i = mat4(1);

// 	std::string path = "/home/mrs/vulkan-samples/RayTracingInVulkan-cube/Scenes/blender_3_3_lts_splash_by_piotr_krynski";
// 	for (const auto & entry : fs::directory_iterator(path))
// 	{
// 		if(entry.path().extension() == ".obj")
// 		{
// 			auto model = Model::LoadModel(entry.path());
// 			// model.Transform(scale(i, vec3(0.1)));

// 			models.push_back(model);
// 		}
// 	}

// 	return std::forward_as_tuple(std::move(models), std::vector<Texture>());
// }