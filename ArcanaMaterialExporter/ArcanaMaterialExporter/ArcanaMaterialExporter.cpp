
#include "pch.h"
#include "MaterialExporter.h"
#include <iostream>

int main(int argc, char* argv[])
{
	//if(argc < 4)
	{
		std::cout << "Usage: ./material_exporter.exe name out_file.xml material_dir material_spec.xml" << std::endl;
		std::cout << "name - The name of the material." << std::endl;
		std::cout << "out_file.xml - The output material specification, stored as XML." << std::endl;
		std::cout << "material_dir - The directory holding material textures." << std::endl;
		std::cout << "material_spec.xml - Specifies how the material textures are named, so they can be automatically read." << std::endl;
	}

	const char* name = "test_material";
	const char* outFile = "out_file.xml";//argv[1];
	const char* materialDir = "material_dir";//argv[2];
	const char* materialSpec = "default_spec.xml";//argv[3];

	MaterialExporter exporter;
	exporter.run(std::string(name), std::string(outFile), std::string(materialDir), std::string(materialSpec));
}
