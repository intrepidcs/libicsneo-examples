#define ICSNEOC_DYNAMICLOAD

#include <iostream>
#include <string>
#include <ctype.h>
#include <vector>
#include "icsneo/icsneocpp.h"

size_t msgLimit = 50000;
std::vector<std::shared_ptr<icsneo::Device>> devices;
std::shared_ptr<icsneo::Device> selectedDevice;

/**
 * \brief Prints all current known devices to output in the following format:
 * [num] DeviceType SerialNum    Connected: Yes/No    Online: Yes/No    Msg Polling: On/Off
 *
 * If any devices could not be described due to an error, they will appear in the following format:
 * Description for device num not available!
 */
void printAllDevices() {
	if(devices.size() == 0) {
		std::cout << "No devices found! Please scan for new devices." << std::endl;
	}

	int index = 1;
	for(auto device : devices) {
		std::cout << "[" << index << "] " << device->describe() << "\tConnected: " << (device->isOpen() ? "Yes\t" : "No\t");
		std::cout << "Online: " << (device->isOnline() ? "Yes\t" : "No\t");
		std::cout << "Msg Polling: " << (device->isMessagePollingEnabled() ? "On" : "Off") << std::endl;
		index++;
	}
}

// Prints the main menu options to output
void printMainMenu() {
	std::cout << "Press the letter next to the function you want to use:" << std::endl;
	std::cout << "A - List all devices" << std::endl;
	std::cout << "B - Find all devices" << std::endl;
	std::cout << "C - Open/close" << std::endl;
	std::cout << "D - Go online/offline" << std::endl;
	std::cout << "E - Enable/disable message polling" << std::endl;
	std::cout << "F - Get messages" << std::endl;
	std::cout << "G - Send message" << std::endl;
	std::cout << "H - Get errors" << std::endl;
	std::cout << "I - Set HS CAN to 250K" << std::endl;
	std::cout << "J - Set LSFT CAN to 250K" << std::endl;
	std::cout << "X - Exit" << std::endl;
}

/**
 * \brief Gets all current API errors and prints them to output
 * Flushes all current API errors, meaning future calls (barring any new errors) will not detect any further API errors
 * Does not get any device errors
 */
void printAPIErrors() {
	auto errors = icsneo::GetErrors(icsneo::ErrorFilter(nullptr, icsneo::APIError::Severity::Error));

	if(errors.size() == 1) {
		std::cout << "1 API error found!" << std::endl;
	} else {
		std::cout << errors.size() << " API errors found!" << std::endl;
	}

	for(auto error : errors) {
		std::cout << error.describe() << std::endl;
	}
}

/**
 * \brief Gets all current API warnings and prints them to output
 * Flushes all current API warnings, meaning future calls (barring any new warnings) will not detect any further API warnings
 * Does not get any device warnings
 */
void printAPIWarnings() {
	auto errors = icsneo::GetErrors(icsneo::ErrorFilter(nullptr, icsneo::APIError::Severity::Warning));

	if(errors.size() == 1) {
		std::cout << "1 API warning found!" << std::endl;
	} else {
		std::cout << errors.size() << " API warnings found!" << std::endl;
	}

	for(auto error : errors) {
		std::cout << error.describe() << std::endl;
	}
}

/**
 * \brief Gets all current device errors and prints them to output.
 * Flushes all current device errors, meaning future calls (barring any new errors) will not detect any further device errors
 */
void printDeviceErrors(std::shared_ptr<icsneo::Device> device) {
	auto errors = icsneo::GetErrors(icsneo::ErrorFilter(device.get(), icsneo::APIError::Severity::Error));

	if(errors.size() == 1) {
		std::cout << "1 device error found!" << std::endl;
	} else {
		std::cout << errors.size() << " device errors found!" << std::endl;
	}

	for(auto error : errors) {
		std::cout << error.describe() << std::endl;
	}
}

/**
 * \brief Gets all current device warnings and prints them to output.
 * Flushes all current device warnings, meaning future calls (barring any new warnings) will not detect any further device warnings
 */
void printDeviceWarnings(std::shared_ptr<icsneo::Device> device) {
	auto errors = icsneo::GetErrors(icsneo::ErrorFilter(device.get(), icsneo::APIError::Severity::Warning));

	if(errors.size() == 1) {
		std::cout << "1 device warning found!" << std::endl;
	} else {
		std::cout << errors.size() << " device warnings found!" << std::endl;
	}

	for(auto error : errors) {
		std::cout << error.describe() << std::endl;
	}
}

/**
 * \brief Used to check character inputs for correctness (if they are found in an expected list)
 * \param[in] numArgs the number of possible options for the expected character
 * \param[in] ... the possible options for the expected character
 * \returns the entered character
 *
 * This function repeatedly prompts the user for input until a matching input is entered
 * Example usage: 
 * char input = getCharInput(std::vector<char> {'F', 'u', 'b', 'a', 'r'});
 */
char getCharInput(std::vector<char> allowed) {
	bool found = false;
	std::string input;

	while(!found) {
		std::cin >> input;

		if(input.length() == 1) {
			for(char compare : allowed) {
				if(compare == input.c_str()[0]) {
					found = true;
					break;
				}
			}
		}

		if(!found) {
			std::cout << "Input did not match expected options. Please try again." << std::endl;
		}
	}

	return input.c_str()[0];
}

/**
 * \brief Prompts the user to select a device from the list of currently known devices
 * \returns a pointer to the device in devices[] selected by the user
 * Requires an input from 1-9, so a maximum of 9 devices are supported
 */
std::shared_ptr<icsneo::Device> selectDevice() {
	printf("Please select a device:\n");
	printAllDevices();
	printf("\n");

	int selectedDeviceNum = 10;

	while((size_t) selectedDeviceNum > devices.size()) {
		char deviceSelection = getCharInput(std::vector<char> {'1', '2', '3', '4', '5', '6', '7', '8', '9'});
		selectedDeviceNum = deviceSelection - '0';
		if((size_t) selectedDeviceNum > devices.size()) {
			std::cout << "Selected device out of range!" << std::endl;
		}
	}

	std::cout << std::endl;

	return devices.at(selectedDeviceNum - 1);
}

int main() {
	std::cout << "Running libicsneo " << icsneo::GetVersion() << std::endl << std::endl;

	while(true) {
		printMainMenu();
		std::cout << std::endl;
		char input = getCharInput(std::vector<char> {'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f', 'G', 'g', 'H', 'h', 'I', 'i', 'J', 'j', 'X', 'x'});
		std::cout << std::endl;

		switch(input) {
		// List current devices
		case 'A':
		case 'a':
			printAllDevices();
			std::cout << std::endl;
			break;
		// Find all devices
		case 'B':
		case 'b':
		{
			devices = icsneo::FindAllDevices();
			if(devices.size() == 1) {
				std::cout << "1 device found!" << std::endl;
			} else {
				std::cout << devices.size() << " devices found!" << std::endl;
			}
			printAllDevices();
			std::cout << std::endl;
			break;
		}
		// Open/Close
		case 'C':
		case 'c':
		{
			// Select a device and get its description
			if(devices.size() == 0) {
				std::cout << "No devices found! Please scan for new devices." << std::endl << std::endl;
				break;
			}
			selectedDevice = selectDevice();

			std::cout << "Would you like to open or close " << selectedDevice->describe() << "?" << std::endl;
			std::cout << "[1] Open" << std::endl << "[2] Close" << std::endl <<  "[3] Cancel" << std::endl << std::endl;
			char selection = getCharInput(std::vector<char> {'1', '2', '3'});
			std::cout << std::endl;
			
			switch(selection) {
			case '1':
				if(selectedDevice->open()) {
					std::cout << selectedDevice->describe() << " successfully opened!" << std::endl << std::endl;
				} else {
					std::cout << selectedDevice->describe() << " failed to open!" << std::endl << std::endl;
					printDeviceWarnings(selectedDevice);
					printDeviceErrors(selectedDevice);
					std::cout << std::endl;
				}
				
				break;
			case '2':
				// Attempt to close the device
				if(selectedDevice->close()) {
					std::cout << "Successfully closed " << selectedDevice->describe() << "!" << std::endl << std::endl;
					selectedDevice = NULL;
				} else {
					std::cout << "Failed to close " << selectedDevice->describe() << "!" << std::endl << std::endl;
					printDeviceWarnings(selectedDevice);
					printDeviceErrors(selectedDevice);
					std::cout << std::endl;
				}
				
				break;
			default:
				std::cout << "Canceling!" << std::endl << std::endl;
				break;
			}
		}
		break;
		// Go online/offline
		case 'D':
		case 'd':
		{
			// Select a device and get its description
			if(devices.size() == 0) {
				std::cout << "No devices found! Please scan for new devices." << std::endl << std::endl;
				break;
			}
			selectedDevice = selectDevice();

			std::cout << "Would you like to have " << selectedDevice->describe() << " go online or offline?" << std::endl;
			std::cout << "[1] Online" << std::endl << "[2] Offline" << std::endl << "[3] Cancel" << std::endl << std::endl;
			char selection = getCharInput(std::vector<char> {'1', '2', '3'});
			std::cout << std::endl;

			switch(selection) {
			case '1':
				// Attempt to have the selected device go online
				if(selectedDevice->goOnline()) {
					std::cout << selectedDevice->describe() << " succecssfully went online!" << std::endl << std::endl;
				} else {
					std::cout << selectedDevice->describe() << " failed to go online!" << std::endl << std::endl;
					printDeviceWarnings(selectedDevice);
					printDeviceErrors(selectedDevice);
					std::cout << std::endl;
				}
				break;
			case '2':
				// Attempt to go offline
				if(selectedDevice->goOffline()) {
					std::cout << selectedDevice->describe() << " successfully went offline!" << std::endl << std::endl;
				} else {
					std::cout << selectedDevice->describe() << " failed to go offline!" << std::endl << std::endl;
					printDeviceWarnings(selectedDevice);
					printDeviceErrors(selectedDevice);
					std::cout << std::endl;
				}
				break;
			default:
				std::cout << "Canceling!" << std::endl << std::endl;
				break;
			}
		}
		break;
		// Enable/disable message polling
		case 'E':
		case 'e':
		{
			// Select a device and get its description
			if(devices.size() == 0) {
				std::cout << "No devices found! Please scan for new devices." << std::endl << std::endl;
				break;
			}
			selectedDevice = selectDevice();

			std::cout << "Would you like to enable or disable message polling for " << selectedDevice->describe() << "?" << std::endl;
			std::cout << "[1] Enable" << std::endl << "[2] Disable" << std::endl << "[3] Cancel" << std::endl << std::endl;
			char selection = getCharInput(std::vector<char> {'1', '2', '3'});
			std::cout << std::endl;

			switch(selection) {
			case '1':
				// Attempt to enable message polling
				if(selectedDevice->enableMessagePolling()) {
					std::cout << "Successfully enabled message polling for " << selectedDevice->describe() << "!" << std::endl << std::endl;
				} else {
					std::cout << "Failed to enable message polling for " << selectedDevice->describe() << "!" << std::endl << std::endl;
					printDeviceWarnings(selectedDevice);
					printDeviceErrors(selectedDevice);
					std::cout << std::endl;
				}
				// Manually setting the polling message limit as done below is optional
				// It will default to 20k if not set
				selectedDevice->setPollingMessageLimit(50000);
				if(selectedDevice->getPollingMessageLimit() == 50000) {
					std::cout << "Successfully set polling message limit for " << selectedDevice->describe() << "!" << std::endl << std::endl;
				} else {
					std::cout << "Failed to set polling message limit for " << selectedDevice->describe() << "!" << std::endl << std::endl;
					printDeviceWarnings(selectedDevice);
					printDeviceErrors(selectedDevice);
					std::cout << std::endl;
				}
				break;
			case '2':
				// Attempt to disable message polling
				if(selectedDevice->disableMessagePolling()) {
					std::cout << "Successfully disabled message polling for " << selectedDevice->describe() << "!" << std::endl;
				} else {
					std::cout << "Failed to disable message polling for " << selectedDevice->describe() << "!" << std::endl << std::endl;
					printDeviceWarnings(selectedDevice);
					printDeviceErrors(selectedDevice);
					std::cout << std::endl;
				}
				break;
			default:
				std::cout << "Canceling!" << std::endl << std::endl;
				break;
			}			
		}
		break;
		// Get messages
		case 'F':
		case 'f':
		{
			// Select a device and get its description
			if(devices.size() == 0) {
				std::cout << "No devices found! Please scan for new devices." << std::endl << std::endl;
				break;
			}
			selectedDevice = selectDevice();

			std::vector<std::shared_ptr<icsneo::Message>> msgs;

			// Attempt to get messages, limiting the number of messages at once to 50,000
			// A third parameter of type std::chrono::milliseconds is also accepted if a timeout is desired
			if(!selectedDevice->getMessages(msgs, msgLimit)) {
				std::cout << "Failed to get messages for " << selectedDevice->describe() << "!" << std::endl << std::endl;
				printDeviceWarnings(selectedDevice);
				printDeviceErrors(selectedDevice);
				std::cout << std::endl;
				break;
			}

			// Alternative way to get messages using default options (no limit on msg count and no timeout)
			// Important to check both if the returned vector has size 1 and contains a nullptr for error checking purposes.
			/*
			auto ret = selectedDevice->getMessages();
			if(ret.size() == 1 && ret.at(0) == nullptr) {
				std::cout << "Failed to get messages for " << selectedDevice->describe() << "!" << std::endl << std::endl;
				printDeviceWarnings(selectedDevice);
				printDeviceErrors(selectedDevice);
				std::cout << std::endl;
				break;
			}
			*/

			if(msgs.size() == 1) {
				std::cout << "1 message received from " << selectedDevice->describe() << "!" << std::endl;
			} else {
				std::cout << msgs.size() << " messages received from " << selectedDevice->describe() << "!" << std::endl;
			}

			// Print out the received messages
			for(auto msg : msgs) {
				std::cout << msg->description << std::endl;
			}
						
			std::cout << std::endl;
		}

		break;
		// Send message
		case 'G':
		case 'g':
		{
			// Select a device and get its description
			if(devices.size() == 0) {
				std::cout << "No devices found! Please scan for new devices." << std::endl << std::endl;
				break;
			}
			selectedDevice = selectDevice();

			// We can transmit messages
			std::cout << "Transmitting an extended CAN FD frame... " << std::endl;
			auto txMessage = std::make_shared<icsneo::CANMessage>();
			txMessage->network = icsneo::Network::NetID::HSCAN;
			txMessage->arbid = 0x1C5001C5;
			txMessage->data.insert(txMessage->data.end(), {0xaa, 0xbb, 0xcc});
			// The DLC will come from the length of the data vector
			txMessage->isExtended = true;
			txMessage->isCANFD = true;

			// Attempt to transmit the sample msg
			if(selectedDevice->transmit(txMessage)) {
				std::cout << "Message transmit successful!" << std::endl;
			} else {
				std::cout << "Failed to transmit message to " << selectedDevice->describe() << "!" << std::endl << std::endl;
				printDeviceWarnings(selectedDevice);
				printDeviceErrors(selectedDevice);
			}

			std::cout << std::endl;
		}
		break;
		// Get errors
		case 'H':
		case 'h':
		{
			// API errors only, no device specific ones
			printAPIWarnings();
			printAPIErrors();
			std::cout << std::endl;
		}
		break;
		// Set HS CAN to 250k
		case 'I':
		case 'i':
		{
			// Select a device and get its description
			if(devices.size() == 0) {
				std::cout << "No devices found! Please scan for new devices." << std::endl << std::endl;
				break;
			}
			selectedDevice = selectDevice();

			// Attempt to set baudrate and apply settings
			if(selectedDevice->settings->setBaudrateFor(icsneo::Network::NetID::HSCAN, 250000) && selectedDevice->settings->apply()) {
				std::cout << "Successfully set HS CAN baudrate for " << selectedDevice->describe() << " to 250k!" << std::endl;
			} else {
				std::cout << "Failed to set HS CAN baudrate for " << selectedDevice->describe() << " to 250k!" << std::endl << std::endl;
				printDeviceWarnings(selectedDevice);
				printDeviceErrors(selectedDevice);
			}
			std::cout << std::endl;
		}
		break;
		// Set LSFT CAN to 250k
		case 'J':
		case 'j':
		{
			// Select a device and get its description
			if(devices.size() == 0) {
				std::cout << "No devices found! Please scan for new devices." << std::endl << std::endl;
				break;
			}
			selectedDevice = selectDevice();

			// Attempt to set baudrate and apply settings
			if(selectedDevice->settings->setBaudrateFor(icsneo::Network::NetID::LSFTCAN, 250000) && selectedDevice->settings->apply()) {
				std::cout << "Successfully set LSFT CAN baudrate for " << selectedDevice->describe() << " to 250k!" << std::endl;
			} else {
				std::cout << "Failed to set LSFT CAN baudrate for " << selectedDevice->describe() << " to 250k!" << std::endl << std::endl;
				printDeviceWarnings(selectedDevice);
				printDeviceErrors(selectedDevice);
			}
			std::cout << std::endl;
		}
		break;
		// Exit
		case 'X':
		case 'x':
			printf("Exiting program\n");
			return 0;
		default:
			printf("Unexpected input, exiting!\n");
			return 1;
		}
	}

	return 0;
}