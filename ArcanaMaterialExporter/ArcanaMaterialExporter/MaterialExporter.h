#ifndef MATERIAL_EXPORTER
#define MATERIAL_EXPORTER

#include <vector>
#include <map>

struct Material
{
	std::string name;
	std::vector<std::pair<std::string, std::string>> textures;
};

class MaterialExporter
{
public:

	MaterialExporter();

	~MaterialExporter();

	void run(std::string name, std::string outFile, std::string materialDir, std::string materialSpec);

private:

	Material loadMaterial(std::string path);

	void cleanup();

private:

	std::map<std::string, std::string> _types;
};

#endif // !MATERIAL_EXPORTER

