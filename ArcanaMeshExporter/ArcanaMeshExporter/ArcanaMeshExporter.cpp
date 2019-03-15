// ArcanaMeshExporter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "MeshExporter.h"
#include <iostream>

int main(int argc, char* argv[])
{
	//if (argc < 4)
	{
		std::cout << "Usage: ./mesh_exporter.exe infile outfile write_materials material_spec.txt" << std::endl;
		std::cout << "infile - The input mesh file (fbx, obj, 3ds, etc.)" << std::endl;
		std::cout << "outfile - Mesh output file name" << std::endl;
		std::cout << "write_materials - Should write materials to the outfile?" << std::endl;
		std::cout << "material_spec.txt - Specifies what material properties should be exported" << std::endl;
	}

	const char* inFile = "textured_cube.obj";//argv[0];
	const char* outFile = "textured_cube.mesh";//argv[1];
	bool writeMaterials = strcmp("true", "true") == 0;//argv[2];
	const char* materialSpec = "default.txt";//argv[3]

	MeshExporter exporter;
	exporter.run(std::string(inFile), std::string(outFile), writeMaterials, std::string(materialSpec));
}
