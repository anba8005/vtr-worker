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
			for (auto & c: command) c = std::toupper(c);
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
			} else if (command.substr(0, 5) == "SEEK:") {
				try {
					worker->seek(std::atol(command.substr(5).c_str()));
				} catch (std::exception& e) {
					cerr << "seek : " << string(e.what()) << std::endl;
				}
			} else {
				cerr << "Invalid command -> " << command << "\n";
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

