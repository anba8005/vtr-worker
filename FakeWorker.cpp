/*
 * FakeWorker.cpp
 *
 *  Created on: Aug 7, 2015
 *      Author: anba8005
 */

#include "FakeWorker.h"

FakeWorker::FakeWorker() {
	connected = false;
	factor = 0;
	state = "NONE";
	lastState = "NONE";
	timecode = 0;
	lastTimecode = -1;
}

FakeWorker::~FakeWorker() {
	close();
}

void FakeWorker::open() {
	if (isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	thread = std::thread(&FakeWorker::worker,this);
}

void FakeWorker::close() {
	std::unique_lock<std::mutex> lock(mutex);
	setConnected(false);
	thread.join();
}

void FakeWorker::play() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	factor = 1;
	state = "PLAYING";
}

void FakeWorker::pause() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	factor = 0;
	state = "STILL";

}

void FakeWorker::stop() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	factor = 0;
	state = "STOPPED";
}

void FakeWorker::eject() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	factor = 0;
	state = "STOPPED";
}

void FakeWorker::shuttle(double rate) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);

}

void FakeWorker::jog(double rate) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);

}

void FakeWorker::seek(long timecode) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	state = "SEEKING";
}

void FakeWorker::rew() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	state = "SEEKING";
	factor = -10;
}

void FakeWorker::ff() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	state = "SEEKING";
	factor = 10;
}

void FakeWorker::stepBack() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	factor = 0;
	timecode -= 1;
}

void FakeWorker::stepForward() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	factor = 0;
	timecode += 1;
}

void FakeWorker::startCapture(long in, long out) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);

}

void FakeWorker::startExport(long in, long out) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
}

bool FakeWorker::isConnected() {
	std::unique_lock<std::mutex> lock(connected_mutex);
	return connected;
}

void FakeWorker::setConnected(bool connected) {
	std::unique_lock<std::mutex> lock(connected_mutex);
	this->connected = connected;
	print(connected ? "CONNECTED" : "DISCONNECTED");
}

void FakeWorker::worker() {
	//
	setConnected(true);
	state = "STOPPED";
	//
	while(isConnected()) {
		std::unique_lock<std::mutex> lock(connected_mutex);
		//
		timecode += factor;
		//
		if (state != lastState) {
			print("STATE:"+state);
			lastState = state;
		}
		if (timecode != lastTimecode) {

			print("TIMECODE:"+std::to_string(timecode));
			lastTimecode = timecode;
		}
		//
		condition.wait_for(lock, std::chrono::milliseconds(40));
	}
}
