#include <version_numbers>
#include <iostream>

int main()
{
	std::cout
		<< CompWolf::compwolf_version.major << '.'
		<< CompWolf::compwolf_version.minor << '.'
		<< CompWolf::compwolf_version.patch << "\n"; 
}
