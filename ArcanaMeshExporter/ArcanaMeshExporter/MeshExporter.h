#ifndef MESH_EXPORTER_H_
#define MESH_EXPORTER_H_

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include <iostream>

#define MAX_TEXCOORDS 8

class MeshExporter
{
public:

	MeshExporter();

	~MeshExporter();

	void run(std::string inFile, std::string outFile, bool writeMaterials, std::string materialSpec);

private:

	struct VertexFormat
	{
		bool hasPosition;
		bool hasNormal;
		bool hasColor;
		bool hasTangent;
		bool hasBinormal;
		bool hasTexCoords[MAX_TEXCOORDS];

		VertexFormat() : hasPosition(false), hasNormal(false), hasColor(false), hasTangent(false), hasBinormal(false)
		{
			for (unsigned int i = 0; i < MAX_TEXCOORDS; i++)
			{
				hasTexCoords[i] = false;
			}
		}
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 color;
		glm::vec3 tangent;
		glm::vec3 binormal;
		glm::vec2 texCoords[MAX_TEXCOORDS];

		VertexFormat format;

		Vertex() : position(glm::vec3(0.0)), normal(glm::vec3(0.0)), color(glm::vec4(0.0)), tangent(glm::vec3(0.0)), binormal(glm::vec3(0.0))
		{
			for (unsigned int i = 0; i < MAX_TEXCOORDS; i++)
			{
				texCoords[i] = glm::vec2(0.0);
			}
		}
	};

	struct Mesh
	{
		std::vector<float> vertexData;
		std::vector<unsigned int> indexData;
		int materialIndex;
		VertexFormat format;
	};

	struct Texture
	{
		std::vector<unsigned char> data;
		unsigned int length;
		aiTextureType type;
		std::string path;
		glm::ivec3 size;

		Texture() : data(), length(0), type(aiTextureType_NONE), path("")
		{

		}

		~Texture()
		{

		}

		Texture(const Texture& texture) : data(texture.data), length(texture.length), type(texture.type), path(texture.path), size(texture.size)
		{

		}

		Texture& operator=(const Texture& texture)
		{
			data = texture.data;
			length = texture.length;
			type = texture.type;
			path = texture.path;
			size = texture.size;

			return *this;
		}
	};

	struct MaterialProperty
	{
		std::string name;
		int type;
		Texture textureValue;

		union
		{
			float floatValue;
			aiColor3D vec3Value;
			aiColor4D vec4Value;
		};

		MaterialProperty() : floatValue(0.0f), type(0)
		{

		}

		~MaterialProperty()
		{

		}

		MaterialProperty(const MaterialProperty& copy) : name(copy.name), type(copy.type)
		{
			if (type == 0)
			{
				floatValue = copy.floatValue;
			}
			else if (type == 1)
			{
				vec3Value = copy.vec3Value;
			}
			else if (type == 2)
			{
				vec4Value = copy.vec4Value;
			}
			else if (type == 3)
			{
				textureValue = copy.textureValue;
			}
		}
	};

private:

	aiTextureType getTextureType(std::string name);

	std::vector<unsigned char> loadTextureFromFile(const char *path, const std::string& directory);

	void processNode(aiNode *node, const aiScene *scene, std::vector<aiMaterial*>& materials);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene, std::vector<aiMaterial*>& materials);

private:

	std::vector<Mesh> _meshes;

	std::string _directory;

	std::vector<Texture> _loadedTextures;
};

#endif // !MESH_EXPORTER_H_

