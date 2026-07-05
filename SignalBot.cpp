#include <iostream>
#include "SignalBot.hpp"
#include <boost/process.hpp>

void SignalBot::listen() {
	std::cout << "Chat started, phone number is: " << phoneNumber << "\n";

	boost::process::ipstream reading_channel;
	boost::process::child signal_process(
		"/usr/local/bin/signal-cli", "-a", phoneNumber, "jsonRpc",
		boost::process::std_out > reading_channel
	);
	
	std::string line;

	while (std::getline(reading_channel, line)) {
		if (line.empty()) continue;
		std::cout << "\nWe catch:\n" << line;
	}
}