#include <iostream>
#include <boost/process.hpp>
#include <nlohmann/json.hpp>
#include "SignalBot.hpp"

using json = nlohmann::json;

void SignalBot::send_message(std::string text, std::string group_id, std::string group_name) {
	json payload = {
			{"jsonrpc", "2.0"},
			{"method", "send"},
			{"id", "bot_1"},
			{"params", {
				{"message", text},
				{"groupId", group_id},
				{ "groupName", group_name }
			}}
	};

	writing_channel << payload.dump() << std::endl;
	std::cout << "\n[LOG] Sent message `" << text << "` to group: " << group_name;


}

void SignalBot::send_video(std::string text, std::string group_id, std::string file_path) {
	json payload = {
			{"jsonrpc", "2.0"},
			{"method", "send"},
			{"id", "bot_1"},
			{"params", {
				{"message", text},
				{"groupId", group_id},
				{"attachments", {file_path}}
			}}
	};

	writing_channel << payload.dump() << std::endl;
	std::cout << "\n[VIDEO EXTRACTOR] Sent video to group!";
}

void SignalBot::listen() {
	std::cout << "Chat started, phone number is: " << phone_number << "\n";

	boost::process::ipstream reading_channel;

	boost::process::child signal_process(
		"/usr/local/bin/signal-cli", "-a", phone_number, "jsonRpc",
		boost::process::std_out > reading_channel,
		boost::process::std_in < writing_channel
	);

	std::string line;

	while (std::getline(reading_channel, line)) {
		if (line.empty()) continue;

		try {
			json terminal_json_data = json::parse(line);

			if (terminal_json_data.contains("method") && terminal_json_data["method"] == "receive") {
				if (terminal_json_data["params"]["envelope"].contains("dataMessage") && terminal_json_data["params"]["envelope"]["dataMessage"].contains("message")) {

					// the message
					std::string text = terminal_json_data["params"]["envelope"]["dataMessage"]["message"];

					// the group
					std::string group = "";
					std::string groupname = "Unknown";
					if (terminal_json_data["params"]["envelope"]["dataMessage"].contains("groupInfo")) {
						group = terminal_json_data["params"]["envelope"]["dataMessage"]["groupInfo"]["groupId"];

						if (terminal_json_data["params"]["envelope"]["dataMessage"]["groupInfo"].contains("groupName")) {
							groupname = terminal_json_data["params"]["envelope"]["dataMessage"]["groupInfo"]["groupName"];
						}
					}

					// private messages are disabled (for now)
					if (group.empty()) {
						std::cout << "[WARN] Private message (DM) received!";
						continue;
					}

					// message reply
					if (text == "bot") {
						send_message("hei", group, groupname);
					}
					else if (text == "bot shut up") {
						is_chatty = false;
						send_message("[STATUS] Turned OFF status messages!", group, groupname);
					}
					else if (text == "bot talk") {
						is_chatty = true;
						send_message("[STATUS] Turned ON status messages!", group, groupname);
					}
					else if (text == "bot status") {
						if (is_chatty == true) {
							send_message("STATUS: Messages turned on", group, groupname);
						}
						else {
							send_message("STATUS: Messages turned off", group, groupname);
						}
					}
					// Video Extractor, checking for links
					else if (text.find("video ") == 0) {
						std::string link_text = text.substr(6);
						std::cout << "\n[VIDEO EXTRACTOR] Found link: " << link_text;
						if (is_chatty)
							send_message("Video found! Investigating...", group, groupname);

						if (text.find("https://") != std::string::npos) {
							std::cout << "\n[VIDEO EXTRACTOR] Started extracting...";
							if (is_chatty)
								send_message("Started extracting...", group, groupname);
							
							std::string file = "downloaded_video.mp4";
							
							// if existing already, will delete the file
							std::remove(file.c_str()); 

							int download_ytdlp = boost::process::system(
								ytdlp_path,
								"--cookies", ytdlp_cookies,
								"-f", "mp4",             
								"-o", file,  
								link_text                
							);

							if (download_ytdlp == 0) {
								std::cout << "\n[VIDEO EXTRACTOR] Succesully downloaded! Sending to the group chat...";
								send_video("", group, file);
								if (is_chatty)
									send_message("Done! Here is the video!", group, groupname);

							}
							else {
								std::cout << "\n[VIDEO EXTRACTOR] Eroare yt-dlp!";
								if (is_chatty)
									send_message("ERROR! No video found in the link!", group, groupname);
							}
						}
						else {
							std::cout << "\n[VIDEO EXTRACTOR] ERROR!";
							if(is_chatty)
								send_message("ERROR! Check the terminal!", group, groupname);
						}
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cout << "\n[ERROR] Error text: " << e.what() << "\n";
		}

	}
}