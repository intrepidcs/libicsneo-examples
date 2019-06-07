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
	if (numDevices == 0) {
		printf("No devices found! Please scan for new devices.\n");
	}
	for (int i = 0; i < numDevices; i++) {
		char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
		size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;

		if (icsneo_describeDevice(devices + i, productDescription, &descriptionLength)) {

			printf("[%d] %s\tConnected: ", i + 1, productDescription);
			if (icsneo_isOpen(devices + i)) {
				printf("Yes\t");
			}
			else printf("No\t");

			printf("Online: ");
			if (icsneo_isOnline(devices + i)) {
				printf("Yes\n");
			}
			else printf("No\n");

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
	printf("A - List all devices\n");
	printf("B - Scan for new devices\n");
	printf("C - Connect to a device\n");
	printf("D - Set a device to online\n");
	printf("E - Get messages\n");
	printf("F - Send message\n");
	printf("G - Get errors\n");
	printf("H - Set HS CAN to 250K\n");
	printf("I - Set HS CAN to 500K\n");
	printf("J - Disconnect from device\n");
	printf("K - Set a device to offline\n");
	printf("X - Exit\n\n");
}

void printAPIErrors() {
	neoerror_t errors[99];
	size_t errorCount = 99;
	if (icsneo_getErrors(errors, &errorCount)) {
		if (errorCount == 1) {
			printf("1 API error found!\n");
			printf("Error 0x%x: %s\n", errors[0].errorNumber, errors[0].description);
		}
		else {
			printf("%d API errors found!\n", (int)errorCount);
			for (int i = 0; i < errorCount; ++i) {
				printf("Error 0x%x: %s\n", errors[i].errorNumber, errors[i].description);
			}
		}
	}
	else {
		printf("Failed to get API errors!\n");
	}
}

void printDeviceErrors(neodevice_t* device) {
	neoerror_t errors[99];
	size_t errorCount = 99;
	if (icsneo_getDeviceErrors(selectedDevice, errors, &errorCount)) {
		if (errorCount == 1) {
			printf("1 device error found!\n");
			printf("Error 0x%x: %s\n", errors[0].errorNumber, errors[0].description);
		}
		else {
			printf("%d device errors found!\n", (int)errorCount);
			for (int i = 0; i < errorCount; ++i) {
				printf("Error 0x%x: %s\n", errors[i].errorNumber, errors[i].description);
			}
		}
	}
	else {
		printf("Failed to get device errors!\nSearching for API errors...\n");
		printAPIErrors();
	}
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
		printf("An error occurred when initializing the library!\n");
		return 1;
	}

	neoversion_t ver = icsneo_getVersion();

	// TODO: If .dll file changes, update this as well
	printf("ICS icsneoc.dll version %u.%u.%u\n\n", ver.major, ver.minor, ver.patch);
	
	while (running) {
		printMainMenu();
		char input = getCharInput(22, 'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f', 'G', 'g', 'H', 'h', 'I', 'i', 'J', 'j', 'X', 'x');
		switch (input) {
			// List current devices
		case 'A':
		case 'a':
			printAllDevices();
			break;
			// Scan for new devices
		case 'B':
		case 'b':
		{
			printf("\n");
			size_t numNewDevices = scanNewDevices();
			if (numNewDevices == 1) {
				printf("1 new device found!\n");
			}
			else {
				printf("%d new devices found!\n", (int)numNewDevices);
			}
			printAllDevices();
			break;
		}
		// Connect to (open) a device
		case 'C':
		case 'c':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);


			if (icsneo_openDevice(selectedDevice)) {
				printf("\n%s successfully opened!\n\n", productDescription);
			}
			else {
				printf("\n%s failed to open!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
			}
		}
		break;
		// Go online
		case 'D':
		case 'd':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			if (icsneo_goOnline(selectedDevice)) {
				printf("\n%s successfully went online!\n\n", productDescription);
			}
			else {
				printf("\n%s failed to go online!\n", productDescription);
				printDeviceErrors(selectedDevice);
			}
		}
		break;
		// Get messages
		case 'E':
		case 'e':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			icsneo_enableMessagePolling(selectedDevice);

			// default is 20k, we want to use 50k
			size_t msgLimit = 10000;
			neomessage_t msgs[10000];
			icsneo_setPollingMessageLimit(selectedDevice, msgLimit);

			// Get messages
			size_t msgCount;
			icsneo_getMessages(selectedDevice, msgs, &msgCount, 0);

			if (msgCount == 1) {
				printf("1 message received!\n");
			}
			else {
				printf("%d messages received!\n", (int)msgCount);
			}	
		}

		break;
		// Send message
		case 'F':
		case 'f':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			icsneo_enableMessagePolling(selectedDevice);

			// start generating sample msg
			uint8_t sendMessageData[8];
			sendMessageData[0] = 0xaa;
			sendMessageData[1] = 0xbb;
			sendMessageData[2] = 0xcc;
			sendMessageData[3] = 0xdd;
			sendMessageData[4] = 0xee;
			sendMessageData[5] = 0xff;

			neomessage_can_t msg;
			msg.arbid = 0x120;
			msg.length = 6;
			msg.netid = ICSNEO_NETID_HSCAN;
			msg.data = sendMessageData;
			msg.status.canfdFDF = false;
			msg.status.extendedFrame = false;
			msg.status.canfdBRS = false;
			// end generating sample msg

			icsneo_transmit(selectedDevice, &msg);
		}
		break;
		// Get errors
		case 'G':
		case 'g':
		{
			printAPIErrors();
		}
			break;
			// Set HS CAN to 250k
		case 'H':
		case 'h':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			if (icsneo_setBaudrate(selectedDevice, ICSNEO_NETID_HSCAN, 250000) && icsneo_settingsApply(selectedDevice)) {
				printf("Successfully set HS CAN baudrate to 250k!\n");
			}
			else {
				printf("Failed to set HS CAN to 250k!\n");
				printDeviceErrors(selectedDevice);
			}
		}
			break;
		// Set HS CAN to 500k
		case 'I':
		case 'i':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			if (icsneo_setBaudrate(selectedDevice, ICSNEO_NETID_HSCAN, 500000) && icsneo_settingsApply(selectedDevice)) {
				printf("Successfully set HS CAN baudrate to 500k!\n");
			}
			else {
				printf("Failed to set HS CAN to 500k!\n");
				printDeviceErrors(selectedDevice);
			}
		}
			break;
		// Disconnect
		case 'J':
		case 'j':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			if (icsneo_closeDevice(selectedDevice)) {
				printf("Successfully closed device!\n");
			}
			else {
				printf("Failed to close device!\n");
				printDeviceErrors(selectedDevice);
			}
		}
			break;
		// Go offline
		case 'K':
		case 'k':
		{
			if (numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n");
				break;
			}
			selectedDevice = selectDevice();

			if (icsneo_goOffline(selectedDevice)) {
				printf("Successfully went offline!\n");
			}
			else {
				printf("Failed to go offline!\n");
				printDeviceErrors(selectedDevice);
			}
		}
			break;
		// Exit
		case 'X':
		case 'x':
		{
			printf("Exiting program\n");
			return !icsneo_close();
		}
		default:
		{
			printf("Unexpected input, exiting!\n");
			return 1;
		}
		}
	}

	return 0;
}
