// InterpretorProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

#include "Interpreter5.h"

int main()
{
	std::string line;
	while (true)
	{
		std::cout << "calc> ";
		if (std::getline(std::cin, line))
		{
			Interpreter inter = Interpreter(line);
			std::string output = inter.interpretIntOpStatement();
			std::cout << output << std::endl;
		}
		else
		{
			break;
		}
	}	

	return 0;
}