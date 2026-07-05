#include "SignalBot.hpp"
#include <cstdlib>
#include <iostream>

int main() {
	const char* phone_number = std::getenv("SIGNAL_PHONE");

	if (!phone_number) {
		std::cout << "ERROR: Cannot find the phone number!";
		return 1;
	}

	SignalBot marcel;

	marcel.phoneNumber = phone_number;
	marcel.listen();

	return 0;
}