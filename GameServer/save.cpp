#include <iostream>
#include <string_view>
#include <fstream>
#include <vector>
#include "save.h"

void save(std::string_view filename)
{
	std::ifstream in{ filename.data() };
	std::ofstream out{ ".\\..\\Server ���� ����.txt", std::ios::app };
	
	out << "------------------------------------------------------------" << std::endl;

	char c;
	in >> std::noskipws;

	while (in >> c)
		out << c;

	out << "------------------------------------------------------------" << std::endl << std::endl;
}