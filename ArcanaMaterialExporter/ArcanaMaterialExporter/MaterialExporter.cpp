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

void MaterialExporter::run(const char* name, const char* outFile, const char* materialDir, const char* materialSpec)
{
	cleanup();

	rapidxml::file<> xmlFile(materialSpec);
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	rapidxml::xml_node<>* node = doc.first_node();
	for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
	{
		if (strcmp(child->name(), "texture_type") == 0)
		{
			_textureType = child->value();
		}
		else if(strcmp(child->name(), "separator") == 0)
		{
			_separator = child->value();
		}
		else if (strcmp(child->name(), "prefix") == 0)
		{
			_prefixes.emplace(std::string(child->value()), std::string(child->first_attribute("attribute")->value()));
		}
		else if (strcmp(child->name(), "suffix") == 0)
		{
			_suffixes.emplace(std::string(child->value()), std::string(child->first_attribute("attribute")->value()));
		}
	}

	std::vector<std::pair<std::string, std::string>> textures;

	for (boost::filesystem::directory_iterator itr(materialDir); itr != boost::filesystem::directory_iterator(); ++itr)
	{
		if (is_regular_file(itr->status()))
		{
			std::string path = itr->path().filename().string();
			std::vector<std::string> words;
			boost::split(words, path, boost::is_any_of(_separator), boost::token_compress_on);

			for (auto iter = words.begin(); iter != words.end(); iter++)
			{
				eraseSubStr(*iter, "." + std::string(_textureType));

				if (strcmp((*iter).c_str(), name) != 0)
				{
					std::map<std::string, std::string>::iterator mapIter = _prefixes.find(*iter);
					if (mapIter != _prefixes.end())
					{
						textures.push_back(std::make_pair(std::string(mapIter->second), path));
					}
					mapIter = _suffixes.find(*iter);
					if (mapIter != _suffixes.end())
					{
						textures.push_back(std::make_pair(std::string(mapIter->second), path));
					}
				}
			}
		}
	}

	
}

void MaterialExporter::cleanup()
{
	_textureType = nullptr;
	_separator = nullptr;
	_prefixes.clear();
	_suffixes.clear();
}
