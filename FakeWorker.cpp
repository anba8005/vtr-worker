/*
 * FakeWorker.cpp
 *
 *  Created on: Aug 7, 2015
 *      Author: anba8005
 */

#include <cstdlib>
#include <iostream>
#include "FakeWorker.h"

#define TIMECODE_MAX 2160000 // 24h @ 25fps
//#define TIMECODE_MAX 100

FakeWorker::FakeWorker() {
	connected = false;
	factor = 0;
	state = "NONE";
	lastState = "NONE";
	timecode = 0;
	lastTimecode = -1;
	seekTimecode = -1;
	mode = "CONTROL";
	lastMode = "NONE";
	actionIn = 0;
	actionOut = 0;
}

FakeWorker::~FakeWorker() {
	close();
}

void FakeWorker::open() {
	if (isConnected())
		return;
	std::unique_lock<std::recursive_mutex> lock(mutex);
	thread = std::thread(&FakeWorker::worker, this);
}

void FakeWorker::close() {
	setConnected(false);
	thread.join();
}

void FakeWorker::play() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	factor = 1;
	state = "PLAYING";
	seekTimecode = -1;
}

void FakeWorker::pause() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	factor = 0;
	state = "STILL";
	seekTimecode = -1;
}

void FakeWorker::stop() {
	if (!isConnected())
		return;
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (mode == "CONTROL") {
		factor = 0;
		state = "STOPPED";
		seekTimecode = -1;
	} else if (mode == "CAPTURE") {
		factor = 0;
		state = "STOPPED";
		mode = "CONTROL";
	} else if (mode == "EXPORT") {

	}
}

void FakeWorker::eject() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	factor = 0;
	state = "STOPPED";
	seekTimecode = -1;
}

void FakeWorker::shuttle(double rate) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
}

void FakeWorker::jog(double rate) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;

}

void FakeWorker::seek(long timecode) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	state = "SEEKING";
	seekTimecode = timecode;
}

void FakeWorker::rew() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	state = "SEEKING";
	factor = -10;
	seekTimecode = -1;
}

void FakeWorker::ff() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	state = "SEEKING";
	factor = 10;
	seekTimecode = -1;
}

void FakeWorker::stepBack() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	factor = 0;
	timecode = conformTimecode(timecode - 1);
	state = "STILL";
	seekTimecode = -1;
}

void FakeWorker::stepForward() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	factor = 0;
	timecode = conformTimecode(timecode + 1);
	state = "STILL";
	seekTimecode = -1;
}

void FakeWorker::startCapture(long in, long out) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	actionIn = conformTimecode(in);
	actionOut = conformTimecode(out);
	seek(conformTimecode(in - 100));
	mode = "CAPTURE";
}

void FakeWorker::startExport(long in, long out) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (!isControlMode())
		return;
	state = "RECORDING";
	factor = 1;
	mode = "EXPORT";
	timecode = conformTimecode(in);
	if (out != -1)
		actionOut = conformTimecode(out);
	print("ACTION:EXPORT:START");
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

bool FakeWorker::isControlMode() {
	return isConnected() && mode == "CONTROL";
}

void FakeWorker::worker() {
	//
	setConnected(true);
	state = "STOPPED";
	//
	while (isConnected()) {
		std::unique_lock<std::recursive_mutex> lock(mutex);
		//
		timecode = conformTimecode(timecode + factor);
		//
		if (seekTimecode != -1)
			seekToTimecode();
		//
		if (mode == "CAPTURE") {
			captureActionCheck();
		} else if (mode == "EXPORT") {
			exportActionCheck();
		}
		//
		if (state != lastState) {
			print("STATE:" + state);
			lastState = state;
		}
		if (timecode != lastTimecode) {
			print("TIMECODE:" + std::to_string(timecode));
			lastTimecode = timecode;
		}
		//
		condition.wait_for(lock, std::chrono::milliseconds(40));
	}
}

long FakeWorker::conformTimecode(long timecode) {
	if (timecode < 0) {
		return TIMECODE_MAX + timecode;
	} else if (timecode >= TIMECODE_MAX) {
		return timecode - TIMECODE_MAX;
	} else {
		return timecode;
	}
}

void FakeWorker::seekToTimecode() {
	// calculate delta from current position to target
	// choose shorter path
	long delta = seekTimecode - this->timecode;
	if (delta != 0) {
		long delta_overflow = seekTimecode - (this->timecode + (delta > 0 ? TIMECODE_MAX : -TIMECODE_MAX));
		if (std::abs(delta) > std::abs(delta_overflow)) {
			delta = delta_overflow;
		}
	}
	//
	if (delta == 0) {
		factor = 0;
		seekTimecode = -1;
		state = "STILL";
	} else {
		factor = calculateFactor(delta, (delta > 0 ? 32 : -32));
	}
}

void FakeWorker::captureActionCheck() {
	if (state == "STILL" && timecode == conformTimecode(actionIn - 100)) {
		factor = 1;
		state = "PLAYING";
	} else if (state == "PLAYING" && timecode == actionIn) {
		print("ACTION:CAPTURE:START");
	} else if (state == "PLAYING" && timecode == actionOut) {
		print("ACTION:CAPTURE:COMPLETE");
		mode = "CONTROL";
		stop();
	}
}

void FakeWorker::exportActionCheck() {
	if (state == "RECORDING" && timecode == actionOut) {
		print("ACTION:EXPORT:COMPLETE");
		mode = "CONTROL";
		stop();
	}
}

long FakeWorker::calculateFactor(long delta, long factor) {
	if (std::abs(factor) <= 1)
		return (delta > 0 ? 1 : -1);
	long rate = delta / factor;
	std::cerr << rate << " @ " << factor << std::endl;
	if (std::abs(rate) > 0) {
		return factor;
	} else {
		return calculateFactor(delta, factor / 2);
	}
}
