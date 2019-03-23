#include "pch.h"
#include "MaterialExporter.h"

#include <iterator>
#include "rapidxml/include.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <sstream>
#include <iostream>

MaterialExporter::MaterialExporter()
{
}


MaterialExporter::~MaterialExporter()
{
}

void eraseSubStr(std::string& mainStr, const std::string& toErase)
{
	size_t pos = mainStr.find(toErase);

	if (pos != std::string::npos)
	{
		mainStr.erase(pos, toErase.length());
	}
}

void MaterialExporter::run(std::string name, std::string outFile, std::string materialDir, std::string materialSpec)
{
	cleanup();

	rapidxml::file<> xmlFile(materialSpec.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	rapidxml::xml_node<>* node = doc.first_node();
	for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
	{
		if (strcmp(child->name(), "type") == 0)
		{
			_types.emplace(std::string(child->value()), std::string(child->first_attribute("attribute")->value()));
		}
	}

	std::vector<Material> materials;

	for (boost::filesystem::directory_iterator itr(materialDir); itr != boost::filesystem::directory_iterator(); ++itr)
	{
		if (is_directory(itr->status()))
		{
			materials.push_back(loadMaterial(itr->path().string()));
		}
	}	

	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* databaseNode = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
	databaseNode->name("resource_database");
	rapidxml::xml_node<>* rootNode = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
	rootNode->name("resource");
	rapidxml::xml_node<>* nameNode = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
	nameNode->name("name");
	nameNode->value(name.c_str());

	rapidxml::xml_node<>* typeNode = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
	typeNode->name("resource_type");
	typeNode->value("material");

	rapidxml::xml_node<>* data = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
	data->name("resource_data");

	rootNode->append_node(nameNode);
	rootNode->append_node(typeNode);
	rootNode->append_node(data);

	databaseNode->append_node(rootNode);

	std::vector<std::string> textureNames;//workaround

	for (std::vector<Material>::iterator i = materials.begin(); i != materials.end(); i++)
	{
		Material& m = *i;

		rapidxml::xml_node<>* technique = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
		technique->name("technique");

		rapidxml::xml_node<>* techniqueName = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
		techniqueName->name("name");
		techniqueName->value(m.name.c_str());

		technique->append_node(techniqueName);

		for (std::vector<std::pair<std::string, std::string>>::iterator i = m.textures.begin(); i != m.textures.end(); i++)
		{
			rapidxml::xml_node<>* texture = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			texture->name((*i).first.c_str());
			textureNames.push_back(m.name + "_" + (*i).first);
			texture->value(textureNames.back().c_str());
			rapidxml::xml_attribute<>* type = new rapidxml::xml_attribute<>();
			type->name("type");
			type->value("resource");
			texture->append_attribute(type);
			technique->append_node(texture);

			//texture resource
			rapidxml::xml_node<>* textureNode = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			textureNode->name("resource");
			rapidxml::xml_node<>* textureNameNode = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			textureNameNode->name("name");
			textureNameNode->value(textureNames.back().c_str());

			rapidxml::xml_node<>* textureTypeNode = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			textureTypeNode->name("resource_type");
			textureTypeNode->value("texture2D");

			rapidxml::xml_node<>* textureData = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			textureData->name("resource_data");

			textureNode->append_node(textureNameNode);
			textureNode->append_node(textureTypeNode);
			textureNode->append_node(textureData);

			rapidxml::xml_node<>* format = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			format->name("format");
			format->value("RGBA");
			rapidxml::xml_attribute<>* formatType = new rapidxml::xml_attribute<>();
			formatType->name("type");
			formatType->value("string");
			format->append_attribute(formatType);
			rapidxml::xml_node<>* internalFormat = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			internalFormat->name("internal_format");
			internalFormat->value("RGBA8");
			rapidxml::xml_attribute<>* internalFormatType = new rapidxml::xml_attribute<>();
			internalFormatType->name("type");
			internalFormatType->value("string");
			internalFormat->append_attribute(internalFormatType);
			rapidxml::xml_node<>* pixelType = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			pixelType->name("pixel_type");
			pixelType->value("UnsignedByte");
			rapidxml::xml_attribute<>* pixelTypeType = new rapidxml::xml_attribute<>();
			pixelTypeType->name("type");
			pixelTypeType->value("string");
			pixelType->append_attribute(pixelTypeType);
			rapidxml::xml_node<>* mipmap = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			mipmap->name("mipmap");
			mipmap->value("true");
			rapidxml::xml_attribute<>* mipmapType = new rapidxml::xml_attribute<>();
			mipmapType->name("type");
			mipmapType->value("bool");
			mipmap->append_attribute(mipmapType);
			rapidxml::xml_node<>* textureStringData = new rapidxml::xml_node<>(rapidxml::node_type::node_element);
			textureStringData->name("data");
			textureStringData->value((*i).second.c_str());
			rapidxml::xml_attribute<>* textureStringDataType = new rapidxml::xml_attribute<>();
			textureStringDataType->name("type");
			textureStringDataType->value("string");
			textureStringData->append_attribute(textureStringDataType);

			textureData->append_node(format);
			textureData->append_node(internalFormat);
			textureData->append_node(pixelType);
			textureData->append_node(mipmap);
			textureData->append_node(textureStringData);

			databaseNode->append_node(textureNode);
		}
		
		data->append_node(technique);
	}

	for (int i = 0; i < textureNames.size(); i++)
	{
		std::cout << textureNames[i].c_str() << std::endl;
	}

	document.append_node(databaseNode);

	std::ofstream output(outFile);

	output << document;
}

Material MaterialExporter::loadMaterial(std::string path)
{
	Material material;
	material.name = path.substr(path.find_last_of('\\') + 1, path.size());

	for (boost::filesystem::directory_iterator itr(path); itr != boost::filesystem::directory_iterator(); ++itr)
	{
		if (is_regular_file(itr->status()))
		{
			std::string file = itr->path().string();

			std::vector<std::string> strs;
			boost::split(strs, file, boost::is_any_of("."));

			std::string type = strs[0].substr(strs[0].find_last_of('\\') + 1, strs[0].size());
			std::string finalPath = file;
			std::replace(finalPath.begin(), finalPath.end(), '\\', '/');

			bool emplaced = false;
			for (std::map<std::string, std::string>::iterator i = _types.begin(); i != _types.end(); i++)
			{
				if ((*i).first.compare(type) == 0)
				{
					emplaced = true;
					material.textures.push_back(std::make_pair((*i).second, finalPath));
				}
			}

			if (!emplaced)
			{
				material.textures.push_back(std::make_pair(type, finalPath));
			}
		}
	}

	return material;
}

void MaterialExporter::cleanup()
{
	_types.clear();
}
