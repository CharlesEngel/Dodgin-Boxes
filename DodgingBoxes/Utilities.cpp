#include "Utilities.h"

int random_int(int low, int high)
{
	static std::mt19937 generator(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
	std::uniform_int_distribution<int> distribution(low, high);

	return distribution(generator);
}