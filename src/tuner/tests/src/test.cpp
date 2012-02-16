#include "tunermanager.h"

#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
	tuner::TunerManager *manager = NULL;
	tuner::Tuner *tuner = NULL;

	manager = tuner::TunerManager::GetInstance();
	tuner = manager->GetTuner();

	if ((void *)tuner == NULL) {
		std::cout << "Cannot create a new Tuner to the Locator" << std::endl;

		exit(1);
	}

	tuner->Scan();

	return EXIT_SUCCESS;
}
