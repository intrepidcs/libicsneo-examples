#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#include <icsneo/icsneocpp.h>

int main() {
	// Print version
	std::cout <<std::endl <<"Example Network Baudrate setter" << std::endl;
	std::cout << "Running libicsneo " << icsneo::GetVersion() << std::endl;
	
	std::cout<< "Supported devices:" << std::endl;
	for(auto& dev : icsneo::GetSupportedDevices())
		std::cout << '\t' << dev << std::endl;

	std::cout << "\nFinding devices... " << std::flush;
	auto devices = icsneo::FindAllDevices(); // This is type std::vector<std::shared_ptr<icsneo::Device>>
	// You now hold the shared_ptrs for these devices, you are considered to "own" these devices from a memory perspective
	std::cout << "OK, " << devices.size() << " device" << (devices.size() == 1 ? "" : "s") << " found" << std::endl;

	// List off the devices
	for(auto& device : devices)
		std::cout << '\t' << device->getType() << " - " << device->getSerial() << " @ Handle " << device->getNeoDevice().handle << std::endl;
	std::cout << std::endl;

	for(auto& device : devices) {
		std::cout << "Connecting to " << device->getType() << ' ' << device->getSerial() << "... ";
		bool ret = device->open();
		if(!ret) { // Failed to open
			std::cout << "FAIL" << std::endl;
			for(auto& err : icsneo::GetErrors())
				std::cout << "\t" << err << "\n";
			std::cout << std::endl;
			continue;
		}
		std::cout << "OK" << std::endl << std:: endl;


				char choice = 'N';
				int inputBaud=500;
				int64_t baud;

		std:: cout <<"This device supports these networks:" << std:: endl;

		for(auto& netw : device->getSupportedRXNetworks())
		std::cout << '\t' << netw << std::endl;

		std::cout<<std::endl<<"Would you like to return this device to default settings?"<<std::endl;
		std::cout<<"Y/N"<<std::endl;
		std::cin >>choice;

		if(choice=='Y')
		{
			// Apply default settings
		std::cout << "\tSetting default settings... ";
		ret = device->settings->applyDefaults(); // This will also write to the device
		std::cout << (ret ? "OK" : "FAIL") << std::endl;
		
		}

		std::cout <<std::endl <<"Getting Network Baudrates" << std::endl << std::endl;

				
		
		for(auto& netw2 : device->getSupportedRXNetworks())
			{
	std::cout <<std::endl << "Getting " << netw2 <<" Baudrate... ";						//HS CAN2 CAN FD
				baud = device->settings->getBaudrateFor(netw2);
				if(baud < 0)
					{
						std::cout << "FAIL" << std::endl;
						continue;
					}
				else
					std::cout << "OK, " << (baud/1000) << "kbit/s" << std::endl;


				std::cout <<"\tWould you like to change the "<< netw2 <<" Baudrate for " << device->getType() << ' ' << device->getSerial() << '?' << std:: endl;
				std::cout<<"\tY/N"<<std::endl;

				std::cin >>choice;
				
				if(choice=='Y')
					{
						std:: cout <<"\tPlease input your desired Baudrate in kbits/s. Common values include 125, 250, and 500 kbits/s."<<std::endl;
						std:: cin >>inputBaud;
						std::cout << "\tSetting "<< netw2 <<" to operate at " << inputBaud <<"kbits/s...";
						inputBaud = inputBaud*1000;
					
						ret = device->settings->setBaudrateFor(netw2, inputBaud);
						std::cout << (ret ? "OK" : "FAIL") << std::endl;

						ret = device->settings->apply(true);
						std::cout << (ret ? "OK" : "FAIL") << std::endl;
					}
			}
		std::cout << std::endl;
		for(auto& netw3 : device->getSupportedRXNetworks())
			{
	
				//CAN FD Networks
				baud = device->settings->getFDBaudrateFor(netw3); //First check if this device even supports CAN FD on this network. If it doesn't, it will continue.
				if(baud < 0)
					{
						std::cout << "CAN FD is not supported on "<< netw3 << std::endl;
						
						continue;
					}
				else
					{
						std::cout <<std::endl << "Getting "<< netw3 <<" FD Baudrate... ";
						std::cout << "OK, " << (baud/1000000) << "Mbit/s" << std::endl;
					}
				std::cout <<"\tWould you like to change the "<< netw3 <<" FD Baudrate for "<< device->getType() <<' '<< device->getSerial() <<'?'<< std:: endl;
				std::cout<<"\tY/N"<<std::endl;

				std::cin >>choice;
				
				if(choice=='Y')
					{
						std:: cout <<"Please input your desired Baudrate in Mbits/s. Common values for CAN FD include 1, 2, 8, and 10 Mbits/s."<<std::endl;
						std:: cin >>inputBaud;
						std::cout << "\tSetting "<< netw3 <<" FD to operate at "<< inputBaud <<"Mbits/s...";
						inputBaud = inputBaud*1000000;
					
						ret = device->settings->setFDBaudrateFor(netw3, inputBaud);
						std::cout << (ret ? "OK" : "FAIL") << std::endl;

						ret = device->settings->apply(true);
						std::cout << (ret ? "OK" : "FAIL") << std::endl;
					}
			}
				
		std::cout << std::endl << "Disconnecting from "<< device->getType() << ' ' << device->getSerial()<<"... ";
		ret = device->close();
		std::cout << (ret ? "OK\n" : "FAIL\n") << std::endl;
	}
	
	std::cout << "Press any key to continue..." << std::endl;
	std::cin.get();
	return 0;
}
