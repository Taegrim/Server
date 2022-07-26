#include <iostream>
#include <string_view>
#include <fstream>
#include <vector>
#include "save.h"

void save(std::string_view filename)
{
	std::ifstream in{ filename.data() };
	std::ofstream out{ ".\\..\\..\\Server 내용 저장.txt", std::ios::app };

	std::vector<char> v{ std::istreambuf_iterator<char>{in}, {} };

	copy(v.begin(), v.end(), std::ostream_iterator<char>{out});
	out << std::endl;
}