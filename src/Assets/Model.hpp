#pragma once

#include "Material.hpp"
#include "Procedural.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Assets
{
	class Model final
	{
	public:

		static Model LoadModel(const std::string& filename);
		static Model LoadModel(const std::string& filename, std::vector<Texture>& sceneTextures);
		static Model CreateCornellBox(const float scale);
		static Model CreateSquare(const float scale);
		static Model CreateBox(const glm::vec3& p0, const glm::vec3& p1, const Material& material);
		static Model CreateSphere(const glm::vec3& center, float radius, const Material& material, bool isProcedural);
		static Model CreateCube(const glm::vec3& center, float radius, const Material& material, bool isProcedural);
		static Model CreateCylinder(const glm::vec3& center, float radius, const Material& material, bool isProcedural);
		static Model CreateMandelbulb(const glm::vec3& center, float radius, const Material& material, bool isProcedural);
		
		Model& operator = (const Model&) = delete;
		Model& operator = (Model&&) = delete;

		Model() = default;
		Model(const Model&) = default;
		Model(Model&&) = default;
		~Model() = default;

		void SetMaterial(const Material& material);
		void SetMaterial(const Material& material, int index);
		void SetAllMaterial(const Material& material);
		void Transform(const glm::mat4& transform);

		const std::vector<Vertex>& Vertices() const { return vertices_; }
		const std::vector<uint32_t>& Indices() const { return indices_; }
		const std::vector<Material>& Materials() const { return materials_; }

		const class Procedural* Procedural() const { return procedural_.get(); }
		const class Procedural* ProceduralCube() const { return proceduralCube_.get(); }
		const class Procedural* ProceduralCylinder() const { return proceduralCylinder_.get(); }
		const class Procedural* ProceduralMandelbulb() const { return proceduralMandelbulb_.get(); }

		uint32_t NumberOfVertices() const { return static_cast<uint32_t>(vertices_.size()); }
		uint32_t NumberOfIndices() const { return static_cast<uint32_t>(indices_.size()); }
		uint32_t NumberOfMaterials() const { return static_cast<uint32_t>(materials_.size()); }

	private:

		Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Material>&& materials, const class Procedural* procedural);
		Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Material>&& materials, const class Procedural* procedural, const class Procedural* proceduralCube);
		Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Material>&& materials, const class Procedural* procedural, const class Procedural* proceduralCube, const class Procedural* proceduralCylinder);
		Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Material>&& materials, const class Procedural* procedural, const class Procedural* proceduralCube, const class Procedural* proceduralCylinder, const class Procedural* proceduralMandelbulb);

		std::vector<Vertex> vertices_;
		std::vector<uint32_t> indices_;
		std::vector<Material> materials_;
		std::shared_ptr<const class Procedural> procedural_;
		std::shared_ptr<const class Procedural> proceduralCube_;
		std::shared_ptr<const class Procedural> proceduralCylinder_;
		std::shared_ptr<const class Procedural> proceduralMandelbulb_;
	};

}
