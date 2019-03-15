#include "pch.h"
#include "MeshExporter.h"

#include <iostream>
#include <fstream>
#include <map>
#include <boost/algorithm/string.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

MeshExporter::MeshExporter()
{
}


MeshExporter::~MeshExporter()
{
}

aiTextureType MeshExporter::getTextureType(std::string name) //string of the form texture_typename
{
	std::vector<std::string> strs;
	boost::split(strs, name, boost::is_any_of("_"));

	if (strs.size() == 2 && strs[0].compare("texture") == 0)
	{
		if (strs[1].compare("diffuse") == 0)
			return aiTextureType_DIFFUSE;
		if (strs[1].compare("specular") == 0)
			return aiTextureType_SPECULAR;
		if (strs[1].compare("ambient") == 0)
			return aiTextureType_AMBIENT;
		if (strs[1].compare("emissive") == 0)
			return aiTextureType_EMISSIVE;
		if (strs[1].compare("height") == 0)
			return aiTextureType_HEIGHT;
		if (strs[1].compare("normals") == 0)
			return aiTextureType_NORMALS;
		if (strs[1].compare("shininess") == 0)
			return aiTextureType_SHININESS;
		if (strs[1].compare("opacity") == 0)
			return aiTextureType_OPACITY;
		if (strs[1].compare("displacement") == 0)
			return aiTextureType_DISPLACEMENT;
		if (strs[1].compare("lightmap") == 0)
			return aiTextureType_LIGHTMAP;
		if (strs[1].compare("reflection") == 0)
			return aiTextureType_REFLECTION;
		if (strs[1].compare("default") == 0 || strs[1].compare("unknown") == 0)
			return aiTextureType_UNKNOWN;
	}

	return aiTextureType_NONE;
}

std::vector<unsigned char> MeshExporter::loadTextureFromFile(const char *path, const std::string& directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	//return std::vector<unsigned char>(data, data + sizeof(data));
	return std::vector<unsigned char>();
}

void MeshExporter::run(std::string inFile, std::string outFile, bool writeMaterials, std::string materialSpec)
{
	/* AI_MATKEY_NAME "?mat.name",0,0
AI_MATKEY_TWOSIDED "$mat.twosided",0,0
 AI_MATKEY_SHADING_MODEL "$mat.shadingm",0,0
 AI_MATKEY_ENABLE_WIREFRAME "$mat.wireframe",0,0
 AI_MATKEY_BLEND_FUNC "$mat.blend",0,0
 AI_MATKEY_OPACITY "$mat.opacity",0,0
 AI_MATKEY_BUMPSCALING "$mat.bumpscaling",0,0
 AI_MATKEY_SHININESS "$mat.shininess",0,0
 AI_MATKEY_REFLECTIVITY "$mat.reflectivity",0,0
 AI_MATKEY_SHININESS_STRENGTH "$mat.shinpercent",0,0
 AI_MATKEY_REFRACTI "$mat.refracti",0,0
 AI_MATKEY_COLOR_DIFFUSE "$clr.diffuse",0,0
 AI_MATKEY_COLOR_AMBIENT "$clr.ambient",0,0
 AI_MATKEY_COLOR_SPECULAR "$clr.specular",0,0
 AI_MATKEY_COLOR_EMISSIVE "$clr.emissive",0,0
 AI_MATKEY_COLOR_TRANSPARENT "$clr.transparent",0,0
 AI_MATKEY_COLOR_REFLECTIVE "$clr.reflective",0,0
 AI_MATKEY_GLOBAL_BACKGROUND_IMAGE "?bg.global",0,0*/

	std::map<std::string, std::string> materialProperties;

	std::ifstream file(materialSpec);
	std::string str;
	while (std::getline(file, str))
	{
		std::vector<std::string> strs;
		boost::split(strs, str, boost::is_any_of("\t "));

		if (strs.size() == 2)
		{
			materialProperties.emplace(strs[0], strs[1]);
		}
	}


	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(inFile, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	_directory = inFile.substr(0, inFile.find_last_of('/'));

	std::vector<aiMaterial*> materials;

	processNode(scene->mRootNode, scene, materials);

	//what to write to binary
	//num meshes
	//num materials
	//for each mesh
	//	num vertices
	//  num indices
	//  hasPosition
	//  hasNormal
	//  hasColor
	//	hasTangent
	//	hasBinormal
	//  hasTexCoords(0), ..., hasTexCoords(7)
	//	vertices
	//	indices
	//	material index	-- is has materials
	//for each material
	//	num attributes
	//  for each attribute
	//		name length
	//		name
	//		type
	//		data

	std::ofstream outData(outFile, std::ios::binary);
	int numMeshes = _meshes.size();
	outData.write(reinterpret_cast<const char*>(&numMeshes), sizeof(numMeshes));
	int numMaterials = materials.size();
	outData.write(reinterpret_cast<const char*>(&numMaterials), sizeof(numMaterials));
	for (unsigned int i = 0; i < _meshes.size(); i++)
	{
		int numVertices = _meshes[i].vertexData.size();
		int numIndices = _meshes[i].indexData.size();
		outData.write(reinterpret_cast<const char*>(&numVertices), sizeof(numVertices));
		outData.write(reinterpret_cast<const char*>(&numIndices), sizeof(numIndices));

		outData.write(reinterpret_cast<const char*>(&_meshes[i].format.hasPosition), sizeof(_meshes[i].format.hasPosition));
		outData.write(reinterpret_cast<const char*>(&_meshes[i].format.hasNormal), sizeof(_meshes[i].format.hasNormal));
		outData.write(reinterpret_cast<const char*>(&_meshes[i].format.hasColor), sizeof(_meshes[i].format.hasColor));
		outData.write(reinterpret_cast<const char*>(&_meshes[i].format.hasTangent), sizeof(_meshes[i].format.hasTangent));
		outData.write(reinterpret_cast<const char*>(&_meshes[i].format.hasBinormal), sizeof(_meshes[i].format.hasBinormal));

		for (unsigned int j = 0; j < MAX_TEXCOORDS; j++)
		{
			outData.write(reinterpret_cast<const char*>(&_meshes[i].format.hasTexCoords[j]), sizeof(_meshes[i].format.hasTexCoords[j]));
		}

		for (unsigned int j = 0; j < numVertices; j++)
		{
			outData.write(reinterpret_cast<const char*>(&_meshes[i].vertexData[j]), sizeof(_meshes[i].vertexData[j]));
		}

		for (unsigned int j = 0; j < numIndices; j++)
		{
			outData.write(reinterpret_cast<const char*>(&_meshes[i].indexData[j]), sizeof(_meshes[i].indexData[j]));
		}

		if (writeMaterials)
		{
			int materialIndex = _meshes[i].materialIndex;//get material index

			outData.write(reinterpret_cast<const char*>(&materialIndex), sizeof(materialIndex));
		}
	}

	if (writeMaterials)
	{
		for (unsigned int i = 0; i < numMaterials; i++)
		{
			aiMaterial* material = materials[i];

			std::map<std::string, int> sizes;
			for (int j = 0; j < material->mNumProperties; j++)
			{
				sizes.emplace(std::string(material->mProperties[j]->mKey.C_Str()), material->mProperties[j]->mDataLength);
			}

			std::vector<MaterialProperty> properties;

			std::map<std::string, std::string>::iterator iter;
			for (iter = materialProperties.begin(); iter != materialProperties.end(); iter++)
			{
				float floatValue = 0.0;
				aiColor3D vec3Value = aiColor3D(0, 0, 0);
				aiColor4D vec4Value = aiColor4D(0, 0, 0, 0);

				MaterialProperty property;
				property.name = (*iter).second;

				aiTextureType type = getTextureType((*iter).first);

				if (type != aiTextureType_NONE)
				{
					aiString str;
					material->GetTexture(type, 0, &str);

					Texture attributeTexture;

					bool skip = false;
					for (unsigned int j = 0; j < _loadedTextures.size(); j++)
					{
						if (std::strcmp(_loadedTextures[j].path.c_str(), str.C_Str()) == 0)
						{
							attributeTexture = _loadedTextures[j];
							skip = true;
							break;
						}
					}

					if (!skip)
					{
						std::string filename = std::string(str.C_Str());
						//filename = this->_directory + '/' + filename;

						std::cout << "Loading texture: " << filename.c_str() << std::endl;

						int width, height, nrComponents;
						unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

						std::cout << "data: " << (data == nullptr) << " " << (width * height * nrComponents) << std::endl;

						if (data != nullptr)
						{
							attributeTexture.length = width * height * nrComponents;
							attributeTexture.data = std::vector<unsigned char>(data, data + attributeTexture.length);//loadTextureFromFile(str.C_Str(), this->_directory);
							attributeTexture.type = type;
							attributeTexture.size = glm::vec3(width, height, nrComponents);
							attributeTexture.path = std::string(str.C_Str());
							_loadedTextures.push_back(attributeTexture);
						}

						STBI_FREE(data);
					}

					if (attributeTexture.data.empty())
					{
						property.type = 0;
						property.floatValue = 0.0f;
					}
					else
					{
						property.type = 3;
						property.textureValue = attributeTexture;
					}
				}
				else
				{
					if (sizes.at((*iter).first) == sizeof(float) && material->Get((*iter).first.c_str(), 0, 0, floatValue) == aiReturn_SUCCESS)
					{
						property.type = 0;
						property.floatValue = floatValue;
						std::cout << (*iter).first.c_str() << " is float: " << floatValue << std::endl;
					}
					else if (sizes.at((*iter).first) == sizeof(aiColor3D) && material->Get((*iter).first.c_str(), 0, 0, vec3Value) == aiReturn_SUCCESS)
					{
						property.type = 1;
						property.vec3Value = vec3Value;
						std::cout << (*iter).first.c_str() << " is vec3: " << vec3Value.r << ", " << vec3Value.g << ", " << vec3Value.b << std::endl;
					}
					else if (sizes.at((*iter).first) == sizeof(aiColor4D) && material->Get((*iter).first.c_str(), 0, 0, vec4Value) == aiReturn_SUCCESS)
					{
						property.type = 2;
						property.vec4Value = vec4Value;
						std::cout << (*iter).first.c_str() << " is vec4: " << vec4Value.r << ", " << vec4Value.g << ", " << vec4Value.b << ", " << vec4Value.a << std::endl;
					}
				}

				properties.push_back(property);
			}

			int numAttributes = properties.size();
			outData.write(reinterpret_cast<const char*>(&numAttributes), sizeof(numAttributes));

			std::vector<MaterialProperty>::iterator j;
			for (j = properties.begin(); j != properties.end(); j++)
			{
				MaterialProperty& property = *j;

				std::string name = property.name;
				size_t nameLength = name.size();
				std::cout << "writing property: " << name.c_str() << std::endl;
				outData.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
				outData.write(name.c_str(), nameLength);

				int type = property.type;//0 is float, 1 is vec3, 2 is vec4, 3 is texture
				outData.write(reinterpret_cast<const char*>(&type), sizeof(type));
				if (type == 0)
				{
					outData.write(reinterpret_cast<const char*>(&property.floatValue), sizeof(property.floatValue));
				}
				else if (type == 1)
				{
					outData.write(reinterpret_cast<const char*>(&property.vec3Value.r), sizeof(property.vec3Value.r));
					outData.write(reinterpret_cast<const char*>(&property.vec3Value.g), sizeof(property.vec3Value.g));
					outData.write(reinterpret_cast<const char*>(&property.vec3Value.b), sizeof(property.vec3Value.b));
				}
				else if (type == 2)
				{
					outData.write(reinterpret_cast<const char*>(&property.vec4Value.r), sizeof(property.vec4Value.r));
					outData.write(reinterpret_cast<const char*>(&property.vec4Value.g), sizeof(property.vec4Value.g));
					outData.write(reinterpret_cast<const char*>(&property.vec4Value.b), sizeof(property.vec4Value.b));
					outData.write(reinterpret_cast<const char*>(&property.vec4Value.a), sizeof(property.vec4Value.a));
				}
				else if (type == 3)
				{
					outData.write(reinterpret_cast<const char*>(&property.textureValue.length), sizeof(property.textureValue.length));
					outData.write(reinterpret_cast<const char*>(&property.textureValue.size.x), sizeof(property.textureValue.size.x));
					outData.write(reinterpret_cast<const char*>(&property.textureValue.size.y), sizeof(property.textureValue.size.y));
					outData.write(reinterpret_cast<const char*>(&property.textureValue.size.z), sizeof(property.textureValue.size.z));
					outData.write(reinterpret_cast<const char*>(property.textureValue.data.data()), property.textureValue.length);
				}
			}

			/*for (unsigned int j = 0; j < numAttributes; j++)
			{

				aiMaterialProperty* property = material->mProperties[j];
				std::string name = std::string(property->mKey.C_Str());
				std::cout << name << std::endl;
				//std::string name = "baseColor";
				size_t nameLength = name.size();
				outData.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
				outData.write(name.c_str(), nameLength);

				/*int type = 1;//1 is vec3
				//test
				float x = 0.5f;
				float y = 0.25f;
				float z = 1.0f;
				outData.write(reinterpret_cast<const char*>(&type), sizeof(type));

				outData.write(reinterpret_cast<const char*>(&x), sizeof(x));
				outData.write(reinterpret_cast<const char*>(&y), sizeof(y));
				outData.write(reinterpret_cast<const char*>(&z), sizeof(z));
			}*/
		}
	}

	outData.close();
}

void MeshExporter::processNode(aiNode *node, const aiScene *scene, std::vector<aiMaterial*>& materials)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(processMesh(mesh, scene, materials));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, materials);
	}

}

MeshExporter::Mesh MeshExporter::processMesh(aiMesh *mesh, const aiScene *scene, std::vector<aiMaterial*>& materials)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		if (mesh->HasPositions())
		{
			vertex.format.hasPosition = true;
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;
		}

		if (mesh->HasNormals())
		{
			vertex.format.hasNormal = true;
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasVertexColors(0))//increase color attribute number???
		{
			vertex.format.hasColor = true;
			vertex.color.x = mesh->mColors[0][i].r;
			vertex.color.y = mesh->mColors[0][i].g;
			vertex.color.z = mesh->mColors[0][i].b;
			vertex.color.w = mesh->mColors[0][i].a;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			vertex.format.hasTangent = true;
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;

			vertex.format.hasBinormal = true;
			vertex.binormal.x = mesh->mBitangents[i].x;
			vertex.binormal.y = mesh->mBitangents[i].y;
			vertex.binormal.z = mesh->mBitangents[i].z;
		}

		for (unsigned int j = 0; j < MAX_TEXCOORDS; j++)
		{
			if (mesh->HasTextureCoords(j))
			{
				vertex.format.hasTexCoords[j] = true;
				vertex.texCoords[j].x = mesh->mTextureCoords[j][i].x;
				vertex.texCoords[j].y = mesh->mTextureCoords[j][i].y;
			}
		}
		vertices.push_back(vertex);
	}

	if (mesh->HasFaces())
	{
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
	}

	if (mesh->mMaterialIndex < scene->mNumMaterials)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		if (std::find(materials.begin(), materials.end(), material) == materials.end())
		{
			materials.push_back(material);
		}
	}

	Mesh exportMesh;
	exportMesh.indexData = indices;
	exportMesh.format = vertices[0].format;
	exportMesh.materialIndex = mesh->mMaterialIndex;

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].format.hasPosition)
		{
			exportMesh.vertexData.push_back(vertices[i].position.x);
			exportMesh.vertexData.push_back(vertices[i].position.y);
			exportMesh.vertexData.push_back(vertices[i].position.z);
		}

		if (vertices[i].format.hasNormal)
		{
			exportMesh.vertexData.push_back(vertices[i].normal.x);
			exportMesh.vertexData.push_back(vertices[i].normal.y);
			exportMesh.vertexData.push_back(vertices[i].normal.z);
		}

		if (vertices[i].format.hasColor)
		{
			exportMesh.vertexData.push_back(vertices[i].color.x);
			exportMesh.vertexData.push_back(vertices[i].color.y);
			exportMesh.vertexData.push_back(vertices[i].color.z);
			exportMesh.vertexData.push_back(vertices[i].color.w);
		}

		if (vertices[i].format.hasTangent)
		{
			exportMesh.vertexData.push_back(vertices[i].tangent.x);
			exportMesh.vertexData.push_back(vertices[i].tangent.y);
			exportMesh.vertexData.push_back(vertices[i].tangent.z);
		}

		if (vertices[i].format.hasBinormal)
		{
			exportMesh.vertexData.push_back(vertices[i].tangent.x);
			exportMesh.vertexData.push_back(vertices[i].tangent.y);
			exportMesh.vertexData.push_back(vertices[i].tangent.z);
		}

		for (unsigned int j = 0; j < MAX_TEXCOORDS; j++)
		{
			if (vertices[i].format.hasTexCoords[j])
			{
				exportMesh.vertexData.push_back(vertices[i].texCoords[j].x);
				exportMesh.vertexData.push_back(vertices[i].texCoords[j].y);
			}
		}
	}

	return exportMesh;
}
