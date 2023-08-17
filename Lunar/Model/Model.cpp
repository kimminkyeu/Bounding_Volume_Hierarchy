//
// Created by Minkyeu Kim on 7/25/23.
//

#include "Model.h"

namespace Lunar {


	Model::Model()
	{

	}

	Model::~Model()
	{
		this->ClearModel();
	}

	void Model::LoadModel(const std::string& filePath)
	{
		Assimp::Importer importer;

		std::string fullTexturePath = std::string(PROJECT_ROOT_DIR) + "/" + std::string(filePath);
		const aiScene* scene = importer.ReadFile(fullTexturePath,
						 aiProcess_Triangulate | aiProcess_FlipUVs |
						        aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

		if (!scene)
		{
			LOG_ERROR("Model {0} import failed: {0}", fullTexturePath, importer.GetErrorString());
			return ;
		}
		LoadNode(scene->mRootNode, scene);
//		LoadMaterials(scene);
	}

	void Model::LoadNode(aiNode* node, const aiScene* scene)
	{
		for (size_t i=0; i<node->mNumMeshes; i++)
		{
			LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
		}
		for (size_t i=0; i<node->mNumChildren; i++)
		{
			LoadNode(node->mChildren[i], scene);
		}
	}

	void Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<GLfloat> vertices;
		std::vector<unsigned int> indices;

		for (size_t i=0; i<mesh->mNumVertices; i++)
		{
			vertices.insert(vertices.end(),
					{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });

			// Texture 가 있을 경우.
			if(mesh->mTextureCoords[0])
			{
				vertices.insert(vertices.end(),
					{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
			}
			else // 만약 Texture 가 없을 경우
			{
				vertices.insert(vertices.end(),
					{ 0.0f, 0.0f });
			}
			// Normals (근데 OBJ에 normal이 없는데...?)
			vertices.insert(vertices.end(),
					{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
		}

		// Update Indices
		for (size_t i=0; i<mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j=0; j<face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		Mesh* newMesh = new Mesh();

		// NOTE: Create VAO for each Mesh
		newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
		m_MeshList.push_back(newMesh);
		m_MeshToTexture.push_back(mesh->mMaterialIndex);
	}

	void Model::LoadMaterials(const aiScene* scene)
	{
		m_TextureList.resize(scene->mNumMaterials);

		for (size_t i=0; i<scene->mNumMaterials; i++)
		{
			aiMaterial* material = scene->mMaterials[i];
			m_TextureList[i] = nullptr;

			// if we have at least one diffuse texture inside the scene.
			if (material->GetTextureCount(aiTextureType_DIFFUSE))
			{
				aiString path;
				// if getting texture succeed
				if(material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
				{
					int idx = std::string(path.data).rfind('\\');
//					std::string filePath = std::string(path.data).substr(0, idx);
//					std::string fileName = std::string(path.data).substr(idx + 1);
//					std::string texPath = std::string("") + fileName;
					std::string texPath = path.data;
					LOG_TRACE("Model Texture Path: {0}", texPath);
					m_TextureList[i] = new Texture(texPath);
					if (!m_TextureList[i]->LoadTexture())
					{
						LOG_ERROR("Failed to load texture at {0}", texPath);
						delete m_TextureList[i];
						m_TextureList[i] = nullptr;
					}
				}
			}
			if (!m_TextureList[i]) // no texture
			{
				m_TextureList[i] = new Texture("LunarApp/assets/brick.png");
				m_TextureList[i]->LoadTexture();
			}
		}
	}

	// NOTE: !!!! MOST IMPORTANT
	void Model::RenderModel(GLenum mode)
	{
		for (size_t i=0; i<m_MeshList.size(); i++)
		{
			// if material is assigned to the mesh (check meshToTex list)
			unsigned int materialIndex = m_MeshToTexture[i];
			if ((materialIndex < m_TextureList.size()) && m_TextureList[materialIndex])
			{
				m_TextureList[materialIndex]->UseTexture();
			}
			m_MeshList[i]->RenderMesh(mode);
		}
	}

	void Model::ClearModel()
	{
		for (size_t i=0; i<m_MeshList.size(); i++)
		{
			if (m_MeshList[i])
			{
				delete m_MeshList[i];
				m_MeshList[i] = nullptr;
			}
		}
		for (size_t i=0; i<m_TextureList.size(); i++)
		{
			if (m_TextureList[i])
			{
				delete m_TextureList[i];
				m_TextureList[i] = nullptr;
			}
		}
	}
}