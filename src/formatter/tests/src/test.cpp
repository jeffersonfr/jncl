#include "nclformatter.h"
#include "nclexception.h"

#include <iostream>

using namespace jncl;

int main(int argc, char **argv)
{
	if (argc < 2) {
		return 1;
	}

	try {
		jncl::NCLFormatter formatter(argv[1], 1920, 1080);

		formatter.LoadPresentation();
		formatter.StartPresentation();

		formatter.Wait();
		formatter.Release();
	} catch (jncl::NCLException &e) {
		std::cout << "[jncl::NCLException] -> " << e.what() << "" << std::endl;
	}

	return 0;
}

