#define ICSNEOC_DYNAMICLOAD

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "icsneo/icsneoc.h"

size_t numDevices = 0;
neodevice_t devices[99];
neodevice_t* selectedDevice = NULL;

void printAllDevices() {
	for (int i = 0; i < numDevices; i++) {
		char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
		size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;

		if (icsneo_describeDevice(devices + i, productDescription, &descriptionLength)) {
			printf("[%d] %s\n", i + 1, productDescription);
		}
		else {
			printf("Device %d not found!\n", i + 1);
		}
	}
	printf("\n");
}

// Scans for any new devices
// Adds them to devices and updates numDevices accordingly
// Does not shrink devices or numDevices
// Appends new devices to the end of the current array, if too many are found (total lifetime over 99) undefined behavior ensues
size_t scanNewDevices() {
	neodevice_t newDevices[99];
	size_t numNewDevices = 99;
	icsneo_findAllDevices(newDevices, &numNewDevices);

	for (int i = 0; i < numNewDevices; ++i) {
		devices[numDevices + i] = newDevices[i];
	}
	numDevices += numNewDevices;
	return numNewDevices;
}

void printMainMenu() {
	printf("Press the letter next to the function you want to use\n");
	printf("A - Scan for new devices\n");
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

neodevice_t* selectDevice() {
	printf("\nPlease select a device:\n");
	printAllDevices();

	int selectedDeviceNum = 10;

	while (selectedDeviceNum > numDevices) {
		char deviceSelection = getCharInput(9, '1', '2', '3', '4', '5', '6', '7', '8', '9');
		selectedDeviceNum = deviceSelection - '0';
		if (selectedDeviceNum > numDevices) {
			printf("Selected device out of range!\n");
		}
	}

	return devices + selectedDeviceNum - 1;
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
			size_t numNewDevices = scanNewDevices();
			if (numNewDevices == 1) {
				printf("1 new device found!\n");
			}
			else {
				printf("%d new devices found!\n", (int) numNewDevices);
			}
			printAllDevices();
			break;
		case 'B':
		case 'b':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			if (icsneo_isOpen(selectedDevice)) {
				printf("\n%s already open!\n", productDescription);
			}
			else {
				if (icsneo_openDevice(selectedDevice)) {
					printf("\n%s successfully opened!\n\n", productDescription);
				}
				else {
					printf("\n%s failed to open!\n\n", productDescription);
				}
			}
		}
			break;
		case 'C':
		case 'c':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			int selectedDeviceNum = selectDevice();
		}
			
			break;
		case 'D':
		case 'd':
			break;
		case 'E':
		case 'e':
			break;
		case 'F':
		case 'f':
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			if (icsneo_isOnline(selectedDevice)) {
				icsneo_setBaudrate(selectedDevice, ICSNEO_NETID_HSCAN, 250000);
				printf("Successfully set HS CAN baudrate to 250k!");
			}
			else {
				printf("Device not online!\n");
			}
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
