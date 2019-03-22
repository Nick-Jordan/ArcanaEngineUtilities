#ifndef MATERIAL_EXPORTER
#define MATERIAL_EXPORTER

#include <vector>
#include <map>

class MaterialExporter
{
public:

	MaterialExporter();

	~MaterialExporter();

	void run(const char* name, const char* outFile, const char* materialDir, const char* materialSpec);

private:

	void cleanup();

private:

	const char* _textureType;
	const char* _separator;

	std::map<std::string, std::string> _prefixes;
	std::map<std::string, std::string> _suffixes;
};

#endif // !MATERIAL_EXPORTER

