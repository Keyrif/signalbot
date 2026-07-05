#include "SignalBot.hpp"
#include <cstdlib>
#include <iostream>

int main() {
	const char* phone_number = std::getenv("SIGNAL_PHONE");
	const char* ytdlp = std::getenv("YTDLP_PATH");
	const char* ytdlp_cookies = std::getenv("YTDLP_COOKIES");

	if (!phone_number || !ytdlp || !ytdlp_cookies) {
		std::cout << "\nERROR: Missing variables from .env file! If not, run source .env!";
		return 1;
	}

	SignalBot bot;

	bot.phone_number = phone_number;
	bot.ytdlp_path = ytdlp;
	bot.ytdlp_cookies = ytdlp_cookies;
	bot.listen();

	return 0;
}