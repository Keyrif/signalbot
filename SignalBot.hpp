#pragma once
#include <string>
#include <boost/process.hpp>

class SignalBot {
private:
	boost::process::opstream writing_channel;

public:
	bool is_chatty = true; // default
	std::string phone_number;
	std::string ytdlp_path;
	std::string ytdlp_cookies;
	void listen();
	void send_message(std::string text, std::string group_id, std::string group_name);
	void send_video(std::string text, std::string group_id, std::string file_path);
};

