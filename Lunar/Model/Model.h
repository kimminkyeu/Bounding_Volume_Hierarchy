//
// Created by Minkyeu Kim on 7/25/23.
//

#ifndef SCOOP_MODEL_H
#define SCOOP_MODEL_H

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Lunar/Mesh/Mesh.h"
#include "Lunar/Texture/Texture.h"
#include "Lunar/AABB/AABB.h"
#include "Lunar/Material/Material.h"

namespace Lunar {

	class Model
	{
	public: // NOTE: temporary data for AABB creation. delete later!
		glm::mat4 ModelMatrix {1.0f}; // init unit matrix
		std::vector<float> Vertices;
		std::vector<unsigned int> Indices;
        Material _Material;

	public:
		Model();
		~Model();
		void LoadModel(const std::string& filePath);
		void Render(GLenum mode, const Shader* shader = nullptr);
		void ClearModel();
//		void SetTexture();

	private:
		void LoadNode(aiNode* node, const aiScene* scene);
		void LoadMesh(aiMesh* mesh, const aiScene* scene);
		void LoadMaterials(const aiScene* scene);

		std::vector<Mesh*> m_MeshList;
		std::vector<Texture*> m_TextureList; // model에서 여러개의 텍스쳐 사용.
//		std::vector<Material*> m_MaterialList;

		std::vector<unsigned int> m_MeshToTexture; // mesh-텍스쳐 매칭 인덱스를 배열로 저장.
	};
}



#endif //SCOOP_MODEL_H
