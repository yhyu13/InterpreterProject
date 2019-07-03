// InterpretorProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Visual Studio c++ memory leak
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <iostream>
#include <string>

#include "interpreter6.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::string line;
	Interpreter interp = Interpreter();
	while (true)
	{
		std::cout << "calc> ";
		if (std::getline(std::cin, line))
		{
			if (line != "")
			{
				interp.Reset();
				interp.SetText(line);
				std::string output = interp.InterpretIntBinOp();
				if (output != "\0")
					std::cout << output << std::endl;
			}
		}
		else
		{
			break;
		}
	}	
	return 0;
}