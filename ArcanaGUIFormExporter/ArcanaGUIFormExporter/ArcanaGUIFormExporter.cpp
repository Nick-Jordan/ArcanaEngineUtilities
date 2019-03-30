
#include "pch.h"
#include <iostream>

int main(int argc, char* argv[])
{
	//if(argc < 2)
	{
		std::cout << "Usage: ./gui_form_exporter.exe name specification.xml" << std::endl;
		std::cout << "name - The name of the form." << std::endl;
		std::cout << "specification.xml - Specifies the form components (buttons, sliders, etc.)" << std::endl;
	}

	const char* name = "form1"; //argv[0];
	const char* spec = "form1.xml";//argv[1];
}
