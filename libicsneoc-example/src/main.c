#define ICSNEOC_DYNAMICLOAD

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "icsneo/icsneoc.h"

size_t numDevices = 99;
neodevice_t devices[99];

void printAllDevices() {
	icsneo_findAllDevices(devices, &numDevices);
	for (int i = 0; i < numDevices; i++) {
		char productName[ICSNEO_DEVICETYPE_LONGEST_NAME];
		size_t maxNameLength = sizeof(productName) / sizeof(char);
		icsneo_getProductName(devices + i, productName, &maxNameLength);
		printf("[%d] %s - %s\n", i + 1, productName, devices[i].serial);
	}
	printf("\n");
}

void printMainMenu() {
	printf("Press the letter next to the function you want to use\n");
	printf("A - List attached devices\n");
	printf("B - Connect to a device\n");
	printf("C - Get messages\n");
	printf("D - Send message\n");
	printf("E - Get errors\n");
	printf("F - Set HS CAN to 250K\n");
	printf("G - Set HS CAN to 500K\n");
	printf("H - Disconnect from device\n");
	printf("X - Exit\n\n");
}

/**
 * \brief Used to check character inputs for correctness (if they are found in an expected list)
 * \param[in] numArgs the number of possible options for the expected character
 * \param[in] ... the possible options for the expected character
 * \returns the entered character
 *
 * This function repeatedly prompts the user for input until a matching input is entered.
 * Example usage: char input = getCharInput(5, 'F', 'u', 'b', 'a', 'r');
 */
char getCharInput(int numArgs, ...) {
	// 99 chars shold be more than enough to catch any typos
	char input[99];
	bool found = false;

	va_list vaList;
	va_start(vaList, numArgs);

	char* list = (char*) calloc(numArgs, sizeof(char));
	for (int i = 0; i < numArgs; ++i) {
		*(list + i) = va_arg(vaList, int);
	}
	
	va_end(vaList);

	while (!found) {
		fgets(input, 99, stdin);
		if (strlen(input) == 2) {
			for (int i = 0; i < numArgs; ++i) {
				if (input[0] == *(list + i)) {
					found = true;
					break;
				}
			}
		}

		if (!found) {
			printf("Input did not match expected options. Please try again.\n");
		}
	}

	free(list);

	return input[0];
}

int main() {
	bool running = true;

	if (icsneo_init() != 0) {
		printf("ICS Neo init failed!\n");
		return 1;
	}

	neoversion_t ver = icsneo_getVersion();

	// TODO: If .dll file changes, update this as well
	printf("ICS icsneoc.dll version %u.%u.%u\n\n", ver.major, ver.minor, ver.patch);
	
	while (running) {
		printMainMenu();
		char input = getCharInput(18, 'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f', 'G', 'g', 'H', 'h', 'X', 'x');
		switch (input) {
		case 'A':
		case 'a':
			printf("\n");
			printAllDevices();
			break;
		case 'B':
		case 'b':
		{
			printf("Which device would you like to connect to?\n");
			printAllDevices();

			char deviceSelection = getCharInput(1, 'g');

			int selectedDeviceNum = deviceSelection - '0' - 1;
		}
			break;
		case 'C':
		case 'c':
			break;
		case 'D':
		case 'd':
			break;
		case 'E':
		case 'e':
			break;
		case 'F':
		case 'f':
			break;
		case 'G':
		case 'g':
			break;
		case 'H':
		case 'h':
			break;
		case 'X':
		case 'x':
			break;
		default:
			printf("Unexpected input, exiting!\n");
			break;
		}
	}

	return 0;
}
