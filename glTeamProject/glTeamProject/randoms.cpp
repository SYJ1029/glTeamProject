#include "randoms.h"


float GetRandomNumber(int seed) {
	// ������� ������ [-seed/2, seed/2]
	float result = (float)((float)rand() / RAND_MAX) * seed - ((float)seed / 2.0f);

	return result;
}