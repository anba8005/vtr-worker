#include <poll.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "AWorker.h"
#include "VTRWorker.h"
#include "FakeWorker.h"

using namespace std;

std::vector<std::string> split(const std::string& s, char seperator) {
	std::vector<std::string> output;

	std::string::size_type prev_pos = 0, pos = 0;

	while ((pos = s.find(seperator, pos)) != std::string::npos) {
		std::string substring(s.substr(prev_pos, pos - prev_pos));

		output.push_back(substring);

		prev_pos = ++pos;
	}

	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

	return output;
}

bool isCommandAvailable() {
	pollfd fdinfo;
	fdinfo.fd = fileno(stdin);
	fdinfo.events = POLLIN;
	return poll(&fdinfo, 1, 1) > 0;
}

void run(std::shared_ptr<AWorker> worker) {
	while (true) {
		// command
		if (isCommandAvailable()) {
			string command;
			getline(cin, command);
			for (auto & c : command)
				c = std::toupper(c);
			if (command == "EJECT") {
				worker->eject();
			} else if (command == "STOP") {
				worker->stop();
			} else if (command == "START") {
				worker->play();
			} else if (command == "PAUSE") {
				worker->pause();
			} else if (command == "CLOSE") {
				break;
			} else if (command == "FF") {
				worker->ff();
			} else if (command == "REW") {
				worker->rew();
			} else if (command == "STEPF") {
				worker->stepForward();
			} else if (command == "STEPB") {
				worker->stepBack();
			} else if (command.substr(0, 5) == "SEEK:") {
				try {
					worker->seek(std::atol(command.substr(5).c_str()));
				} catch (std::exception& e) {
					cerr << "seek : " << string(e.what()) << std::endl;
				}
			} else if (command.substr(0, 8) == "SHUTTLE:") {
				try {
					worker->shuttle(std::atof(command.substr(8).c_str()));
				} catch (std::exception& e) {
					cerr << "shuttle : " << string(e.what()) << std::endl;
				}
			} else if (command.substr(0, 8) == "CAPTURE:") {
				std::vector<std::string> params = split(command, ':');
				if (params.size() == 3) {
					try {
						worker->startCapture(std::atol(params[1].c_str()), std::atol(params[2].c_str()));
					} catch (std::exception& e) {
						cerr << "capture : " << string(e.what()) << std::endl;
					}
				} else {
					cerr << "capture : invalid parameters " << command << std::endl;
				}
			} else if (command.substr(0, 7) == "EXPORT:") {
				std::vector<std::string> params = split(command, ':');
				if (params.size() == 2 || params.size() == 3) {
					try {
						if (params.size() == 2) {
							worker->startExport(std::atol(params[1].c_str()));
						} else {
							worker->startExport(std::atol(params[1].c_str()), std::atol(params[2].c_str()));
						}
					} catch (std::exception& e) {
						cerr << "capture : " << string(e.what()) << std::endl;
					}
				} else {
					cerr << "capture : invalid parameters " << command << std::endl;
				}
			} else {
				cerr << "Invalid command ->" << command << "<-\n";
			}
		} else {
			// wait
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		// response
		worker->print_commit();
	}
}

int main(int argc, char** argv) {
//
	if (argc < 2) {
		cerr << "Usage : vtr-worker <id>" << endl;
		cerr << "Use negative id for fake vtr :)" << endl;
		return EXIT_FAILURE;
	}
//
	int id;
	try {
		id = std::stoi(argv[1]);
	} catch (...) {
		cerr << "Invalid id " << argv[1] << endl;
		return EXIT_FAILURE;
	}
//
	shared_ptr<AWorker> worker;
	if (id >= 0) {
		worker = shared_ptr<AWorker>(new VTRWorker(id));
	} else {
		worker = shared_ptr<AWorker>(new FakeWorker());
	}
//
	try {
		worker->open();
		run(worker);
	} catch (std::exception const& e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

