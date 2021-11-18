#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
# define PATH_DELIMITER "\\"
#else
# include <unistd.h>
# define PATH_DELIMITER "/"
#endif
#pragma once
// Changes process current directory.
void changeCurrentDir(const char* path) {
#ifdef _WIN32
	SetCurrentDirectoryA(path);
#else
	chdir(path);
#endif
}
#include "vraysdk.hpp"
void logMessage(VRay::VRayRenderer &renderer, const char* message, VRay::MessageLevel level, double instant, void* userData) {
	switch (level) {
	case VRay::MessageError:
		printf("[ERROR] %s\n", message);
		break;
	case VRay::MessageWarning:
		printf("[Warning] %s\n", message);
		break;
	case VRay::MessageInfo:
		printf("[info] %s\n", message);
		break;
	case VRay::MessageDebug:
		// Uncomment for testing, but you might want to ignore these in real code
		printf("[debug] %s\n", message);
		break;
	}
}

void printProgress(VRay::VRayRenderer &renderer, const char* message, int progress, int total, double instant, void* userData) {
	printf("Progress: %s %.2f%%\n", message, 100.0 * progress / total);
}
