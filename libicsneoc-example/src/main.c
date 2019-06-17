#define ICSNEOC_DYNAMICLOAD

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "icsneo/icsneoc.h"

size_t msgLimit = 50000;
size_t numDevices = 0;
neodevice_t devices[99];
neodevice_t* selectedDevice = NULL;

/**
 * \brief Prints all current known devices to output in the following format:
 * [num] DeviceType SerialNum    Connected: Yes/No    Online: Yes/No    Msg Polling: On/Off
 *
 * If any devices could not be described due to an error, they will appear in the following format:
 * Description for device num not available!
 */
void printAllDevices() {
	if(numDevices == 0) {
		printf("No devices found! Please scan for new devices.\n");
	}
	for(int i = 0; i < numDevices; i++) {
		char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
		size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;

		// Updates productDescription and descriptionLength for each device
		if(icsneo_describeDevice(devices + i, productDescription, &descriptionLength)) {

			printf("[%d] %s\tConnected: ", i + 1, productDescription);
			if(icsneo_isOpen(devices + i)) {
				printf("Yes\t");
			} else printf("No\t");

			printf("Online: ");
			if(icsneo_isOnline(devices + i)) {
				printf("Yes\t");
			} else printf("No\t");

			printf("Msg Polling: ");
			if(icsneo_isMessagePollingEnabled(devices + i)) {
				printf("On\n");
			} else printf("Off\n");

		} else {
			printf("Description for device %d not available!\n", i + 1);
		}
	}
}

/**
 * \brief Scans for any new devices, adding them to devices and updating numDevices accordingly
 * A total of 99 devices may be stored at once
 */
size_t scanNewDevices() {
	neodevice_t newDevices[99];
	size_t numNewDevices = 99;
	icsneo_findAllDevices(newDevices, &numNewDevices);

	for(int i = 0; i < numNewDevices; ++i) {
		devices[numDevices + i] = newDevices[i];
	}
	numDevices += numNewDevices;
	return numNewDevices;
}

// Prints the main menu options to output
void printMainMenu() {
	printf("Press the letter next to the function you want to use:\n");
	printf("A - List all devices\n");
	printf("B - Scan for new devices\n");
	printf("C - Connect to a device\n");
	printf("D - Set a device to online\n");
	printf("E - Enable message polling\n");
	printf("F - Get messages\n");
	printf("G - Send message\n");
	printf("H - Get errors\n");
	printf("I - Set HS CAN to 250K\n");
	printf("J - Set HS CAN to 500K\n");
	printf("K - Disconnect from device\n");
	printf("L - Set a device to offline\n");
	printf("X - Exit\n");
}

/**
 * \brief Gets all current API errors and prints them to output
 * Flushes the API error cache, meaning future calls (barring any new errors) will not detect any further API errors
 * Does not get any device errors
 */
void printAPIErrors() {
	neoerror_t errors[99];
	size_t errorCount = 99;
	if(icsneo_getErrors(errors, &errorCount)) {
		if(errorCount == 1) {
			printf("1 API error found!\n");
			printf("Error 0x%x: %s\n", errors[0].errorNumber, errors[0].description);
		} else {
			printf("%d API errors found!\n", (int) errorCount);
			for(int i = 0; i < errorCount; ++i) {
				printf("Error 0x%x: %s\n", errors[i].errorNumber, errors[i].description);
			}
		}
	} else {
		printf("Failed to get API errors!\n");
	}
}

/**
 * \brief Gets all current device errors and prints them to output. If no device errors were found, printAPIErrors() is called
 * Flushes the device error cache, meaning future calls (barring any new errors) will not detect any further device errors
 * If no device errors were found, the APi error cache will be flushed as well, since printAPIErrors() is called
 */
void printDeviceErrors(neodevice_t* device) {
	neoerror_t errors[99];
	size_t errorCount = 99;
	if(icsneo_getDeviceErrors(selectedDevice, errors, &errorCount)) {
		if(errorCount == 1) {
			printf("1 device error found!\n");
			printf("Error 0x%x: %s\n", errors[0].errorNumber, errors[0].description);
		} else {
			printf("%d device errors found!\n", (int) errorCount);
			for(int i = 0; i < errorCount; ++i) {
				printf("Error 0x%x: %s\n", errors[i].errorNumber, errors[i].description);
			}
		}
	} else {
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
 * This function repeatedly prompts the user for input until a matching input is entered
 * Example usage: char input = getCharInput(5, 'F', 'u', 'b', 'a', 'r');
 */
char getCharInput(int numArgs, ...) {
	// 99 chars shold be more than enough to catch any typos
	char input[99];
	bool found = false;

	va_list vaList;
	va_start(vaList, numArgs);

	char* list = (char*) calloc(numArgs, sizeof(char));
	for(int i = 0; i < numArgs; ++i) {
		*(list + i) = va_arg(vaList, int);
	}

	va_end(vaList);

	while(!found) {
		fgets(input, 99, stdin);
		if(strlen(input) == 2) {
			for(int i = 0; i < numArgs; ++i) {
				if(input[0] == *(list + i)) {
					found = true;
					break;
				}
			}
		}

		if(!found) {
			printf("Input did not match expected options. Please try again.\n");
		}
	}

	free(list);

	return input[0];
}

/**
 * \brief Prompts the user to select a device from the list of currently known devices
 * \returns a pointer to the device in devices[] selected by the user
 * Requires an input from 1-9, so a maximum of 9 devices are supported
 */
neodevice_t* selectDevice() {
	printf("Please select a device:\n");
	printAllDevices();
	printf("\n");

	int selectedDeviceNum = 10;

	while(selectedDeviceNum > numDevices) {
		char deviceSelection = getCharInput(9, '1', '2', '3', '4', '5', '6', '7', '8', '9');
		selectedDeviceNum = deviceSelection - '0';
		if(selectedDeviceNum > numDevices) {
			printf("Selected device out of range!\n");
		}
	}

	printf("\n");

	return devices + selectedDeviceNum - 1;
}

int main() {

	if(icsneo_init() != 0) {
		printf("An error occurred when initializing the library!\n");
		return 1;
	}

	neoversion_t ver = icsneo_getVersion();
	printf("ICS icsneoc.dll version %u.%u.%u\n\n", ver.major, ver.minor, ver.patch);

	while(true) {
		printMainMenu();
		printf("\n");
		char input = getCharInput(26, 'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f', 'G', 'g', 'H', 'h', 'I', 'i', 'J', 'j', 'K', 'k', 'L', 'l', 'X', 'x');
		printf("\n");
		switch(input) {
		// List current devices
		case 'A':
		case 'a':
			printAllDevices();
			printf("\n");
			break;
		// Scan for new devices
		case 'B':
		case 'b':
		{
			size_t numNewDevices = scanNewDevices();
			if(numNewDevices == 1) {
				printf("1 new device found!\n");
			} else {
				printf("%d new devices found!\n", (int) numNewDevices);
			}
			printAllDevices();
			printf("\n");
			break;
		}
		// Connect to (open) a device
		case 'C':
		case 'c':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Attempt to open the selected device
			if(icsneo_openDevice(selectedDevice)) {
				printf("%s successfully opened!\n\n", productDescription);
			} else {
				printf("%s failed to open!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
				printf("\n");
			}
		}
		break;
		// Go online
		case 'D':
		case 'd':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Attempt to have the selected device go online
			if(icsneo_goOnline(selectedDevice)) {
				printf("%s successfully went online!\n\n", productDescription);
			} else {
				printf("%s failed to go online!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
				printf("\n");
			}
		}
		break;
		// Enable message polling
		case 'E':
		case 'e':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Attempt to enable message polling
			if(icsneo_enableMessagePolling(selectedDevice)) {
				printf("Successfully enabled message polling for %s!\n", productDescription);
			} else {
				printf("Failed to enable message polling for %s!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
				printf("\n");
			}

			// Manually setting the polling message limit as done below is optional
			// It will default to 20k if not set
			// Attempt to set the polling message limit
			if(icsneo_setPollingMessageLimit(selectedDevice, msgLimit)) {
				printf("Successfully set message polling limit for %s!\n\n", productDescription);
			} else {
				printf("Failed to set polling message limit for %s!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
				printf("\n");
			}
		}
		break;
		// Get messages
		case 'F':
		case 'f':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Prepare the array of neomessage_t ptrs for reading in the messages
			neomessage_t* msgs = malloc(msgLimit * sizeof(neomessage_t));

			// Get messages
			size_t msgCount;

			// Attempt to get messages
			if(!icsneo_getMessages(selectedDevice, msgs, &msgCount, 0)) {
				printf("Failed to get messages for %s!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
				free(msgs);
				printf("\n");
				break;
			}

			if(msgCount == 1) {
				printf("1 message received from %s!\n", productDescription);
			} else {
				printf("%d messages received from %s!\n", (int) msgCount, productDescription);
			}

			// Print out the received messages
			for(size_t i = 0; i < msgCount; i++) {
				neomessage_t* msg = &msgs[i];
				switch(msg->type) {
				case ICSNEO_NETWORK_TYPE_CAN: // CAN
					printf("\t0x%03x [%lu] ", ((neomessage_can_t*) msg)->arbid, (int) msg->length);
					for(size_t i = 0; i < msg->length; i++) {
						printf("%02x ", msg->data[i]);
					}
					printf("(%lu)\n", (int) msg->timestamp);
					break;
				default:
					if(msg->netid != 0)
						printf("\tMessage on netid %d with length %lu\n", msg->netid, (int) msg->length);
				}
			}
			printf("\n");

			free(msgs);
		}

		break;
		// Send message
		case 'G':
		case 'g':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Start generating sample msg
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
			// End generating sample msg

			// Attempt to transmit the sample msg
			if(icsneo_transmit(selectedDevice, &msg)) {
				printf("Message transmit successful!\n");
			} else {
				printf("Failed to transmit message to %s!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
			}

			printf("\n");
		}
		break;
		// Get errors
		case 'H':
		case 'h':
		{
			// API errors only, no device specific ones
			printAPIErrors();
			printf("\n");
		}
		break;
		// Set HS CAN to 250k
		case 'I':
		case 'i':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Attempt to set baudrate and apply settings
			if(icsneo_setBaudrate(selectedDevice, ICSNEO_NETID_HSCAN, 250000) && icsneo_settingsApply(selectedDevice)) {
				printf("Successfully set HS CAN baudrate for %s to 250k!\n", productDescription);
			} else {
				printf("Failed to set HS CAN for %s to 250k!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
			}
			printf("\n");
		}
		break;
		// Set HS CAN to 500k
		case 'J':
		case 'j':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Attempt to set baudrate and apply settings
			if(icsneo_setBaudrate(selectedDevice, ICSNEO_NETID_HSCAN, 500000) && icsneo_settingsApply(selectedDevice)) {
				printf("Successfully set HS CAN baudrate for %s to 500k!\n", productDescription);
			} else {
				printf("Failed to set HS CAN for %s to 500k!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
			}
			printf("\n");
		}
		break;
		// Disconnect
		case 'K':
		case 'k':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Attempt to close the device
			if(icsneo_closeDevice(selectedDevice)) {
				numDevices--;
				printf("Successfully closed %s!\n", productDescription);

				// Shifts everything after the removed device 1 index to the left
				bool startResizing = false;
				for(int i = 0; i < numDevices; ++i) {
					if(selectedDevice == devices + i)
						startResizing = true;
					if(startResizing)
						devices[i] = devices[i + 1];
				}

				selectedDevice = NULL;
			} else {
				printf("Failed to close %s!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
			}
			printf("\n");
		}
		break;
		// Go offline
		case 'L':
		case 'l':
		{
			// Select a device and get its description
			if(numDevices == 0) {
				printf("No devices found! Please scan for new devices.\n\n");
				break;
			}
			selectedDevice = selectDevice();

			char productDescription[ICSNEO_DEVICETYPE_LONGEST_NAME];
			size_t descriptionLength = ICSNEO_DEVICETYPE_LONGEST_DESCRIPTION;
			icsneo_describeDevice(selectedDevice, productDescription, &descriptionLength);

			// Attempt to go offline
			if(icsneo_goOffline(selectedDevice)) {
				printf("%s successfully went offline!\n", productDescription);
			} else {
				printf("%s failed to go offline!\n\n", productDescription);
				printDeviceErrors(selectedDevice);
			}
			printf("\n");
		}
		break;
		// Exit
		case 'X':
		case 'x':
			printf("Exiting program\n");
			return !icsneo_close();
		default:
			printf("Unexpected input, exiting!\n");
			return 1;
		}
	}

	return 0;
}
