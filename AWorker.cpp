/*
 * AWorker.cpp
 *
 *  Created on: Aug 7, 2015
 *      Author: anba8005
 */

#include "AWorker.h"
#include <iostream>

AWorker::AWorker() {
	// TODO Auto-generated constructor stub

}

AWorker::~AWorker() {
}

void AWorker::print(std::string s) {
	std::unique_lock<std::mutex> lock(print_mutex);
	print_buffer.push_back(s);
}

void AWorker::print_commit() {
	std::unique_lock<std::mutex> lock(print_mutex);
	// copy from buffer to commit buffer
	std::vector<std::string> current;
	for (std::string &i : print_buffer) {
		current.push_back(i);
	}
	print_buffer.clear();
	lock.unlock();
	// check
	if (current.empty())
		return;
	// print commit buffer
	for (std::string &i : current) {
		std::cout << i << std::endl;
	}
	// flush
	std::cout.flush();
}
