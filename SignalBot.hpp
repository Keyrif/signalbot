#pragma once
#include <string>
#include <boost/process.hpp>

class SignalBot {
private:
	boost::process::opstream writing_channel;

public:
	std::string phoneNumber;
	void listen();
	void send_message(std::string text, std::string group_id, std::string group_name);
};