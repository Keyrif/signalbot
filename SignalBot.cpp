#include <iostream>
#include <boost/process.hpp>
#include <nlohmann/json.hpp>
#include "SignalBot.hpp"

using json = nlohmann::json;

void SignalBot::send_message(std::string text, std::string group_id, std::string group_name) {
	json payload = {
			{"jsonrpc", "2.0"},
			{"method", "send"},
			{"params", {
				{"message", text},
				{"groupId", group_id},
				{ "groupName", group_name }
			}}
	};

	writing_channel << payload.dump() << std::endl;
	std::cout << "\n[LOG] Sent message `" << text << "` to group" << group_name;


}

void SignalBot::listen() {
	std::cout << "Chat started, phone number is: " << phoneNumber << "\n";

	boost::process::ipstream reading_channel;

	boost::process::child signal_process(
		"/usr/local/bin/signal-cli", "-a", phoneNumber, "jsonRpc",
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

					// Message reply: Marcel, hei
					if (text == "marcel" && !group.empty()) {
						send_message("hei", group, groupname);
					}
					else if (group.empty()) {
						std::cout << "Error, group may be empty!";
;					}
				}
			}
		}
		catch (const std::exception& e) {
		}

	}
}