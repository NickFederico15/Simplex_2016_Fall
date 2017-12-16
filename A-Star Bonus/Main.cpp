#include "Main.h"

int main()
{
	Appclass application;
	if (application.init() != 0) {
		return -1;
	}
	if (application.run() != 0) {
		return -1;
	}
	if (application.end() != 0) {
		return -1;
	}
	
	return 0;
}