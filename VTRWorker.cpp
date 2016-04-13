/*
 * VTRWorker.cpp
 *
 *  Created on: Aug 7, 2015
 *      Author: anba8005
 */

#include "VTRWorker.h"
#include "BMDTools.h"
#include <ctime>
#include <cstring>
#include <unistd.h>

#define TIMECODE_MAX 2160000 // 24h @ 25fps

VTRWorker::VTRWorker(int id) :
		id(id) {
	connected = false;
	state = "NONE";
	lastState = "NONE";
	timecode = 0;
	lastTimecode = -1;
	seekTimecode = -1;
	mode = "CONTROL";
	lastMode = "NONE";
	actionIn = 0;
	actionOut = 0;
	bmdState = "STOPPED";
	bmdTimecode = 0;
	lastShuttleRate = INT8_MIN;
}

VTRWorker::~VTRWorker() {
	close();
}

void VTRWorker::release(IUnknown* o) {
	if (o != NULL)
		o->Release();
}

void VTRWorker::open() {
	// lock
	std::unique_lock<std::recursive_mutex> lock(mutex);

	// get decklink iterator
	std::shared_ptr<IDeckLinkIterator> deckLinkIterator(CreateDeckLinkIteratorInstance(), &release);
	if (!deckLinkIterator)
		throw std::runtime_error("Unable to get decklink iterator");

	// get decklink
	IDeckLink* deckLinkPtr;
	for (int i = 0; i <= id; i++) {
		if (deckLinkIterator->Next(&deckLinkPtr) != S_OK)
			break;
		if (i == id) {
			deckLink = std::shared_ptr<IDeckLink>(deckLinkPtr, &release);
		} else {
			deckLinkPtr->Release();
		}
	}

	// check
	if (!deckLink)
		throw std::runtime_error("Unable to get decklink");

	// get deck control
	IDeckLinkDeckControl* deckLinkDeckControlPtr;
	if (deckLink->QueryInterface(IID_IDeckLinkDeckControl, (void**) &deckLinkDeckControlPtr) != S_OK)
		throw std::runtime_error("Unable to get decklink deck control");
	deckLinkDeckControl = std::shared_ptr<IDeckLinkDeckControl>(deckLinkDeckControlPtr, &release);

	// check
	if (!deckLinkDeckControl)
		throw std::runtime_error("Error creating decklink output");

	// set callback
	deckLinkDeckControl->SetCallback(this);

	// open
	BMDDeckControlError error;
	if (deckLinkDeckControl->Open(25, 1, false, &error) != S_OK)
		throw std::runtime_error("Unable to open decklink deck control");

	// wait for connect
	int msec = 0;
	while (true) {
		std::unique_lock<std::mutex> connected_lock(connected_mutex);
		if (msec > 5000)
			throw std::runtime_error("Unable to connect VTR");
		if (connected)
			break;
		connected_lock.unlock();
		usleep(1000);
		msec++;
	}

	// start worker
	opened = true;
	thread = std::thread(&VTRWorker::worker, this);
}

void VTRWorker::close() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	//
	if (isConnected()) {
		deckLinkDeckControl->Abort();
		BMDDeckControlError error;
		deckLinkDeckControl->CrashRecordStop(&error);
		deckLinkDeckControl->Stop(&error);
	}
	//
	if (deckLinkDeckControl) {
		deckLinkDeckControl->Close(true);
		deckLinkDeckControl.reset();
		deckLink.reset();
	}
	//
	opened = false;
	lock.unlock();
	thread.join();
}

void VTRWorker::play() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Play(&error) != S_OK)
		throwBMDErrorException("Error starting decklink deck", error);
	seekTimecode = -1;
	lastShuttleRate = INT8_MIN;
}

void VTRWorker::pause() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Stop(&error) != S_OK)
		throwBMDErrorException("Error pausing decklink deck", error);
	seekTimecode = -1;
	lastShuttleRate = INT8_MIN;
}

void VTRWorker::stop() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Stop(&error) != S_OK) {
		std::cerr << "WARN Error stopping decklink deck : " << bmdErrorToString(error) << std::endl;
	}
	seekTimecode = -1;
	lastShuttleRate = INT8_MIN;
	mode = "CONTROL";
}

void VTRWorker::eject() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Eject(&error) != S_OK)
		throwBMDErrorException("Error ejecting decklink deck", error);
	seekTimecode = -1;
	lastShuttleRate = INT8_MIN;
	mode = "CONTROL";
}

void VTRWorker::shuttle(double rate) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (lastShuttleRate == rate)
		return;

	BMDDeckControlError error;
	if (deckLinkDeckControl->Shuttle(rate, &error) != S_OK) {
		std::cerr << "WARN Error shuttling decklink deck : " << bmdErrorToString(error) << std::endl;
		lastShuttleRate = INT8_MIN;
	} else {
		lastShuttleRate = rate;
	}
}

void VTRWorker::jog(double rate) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Jog(rate, &error) != S_OK)
		throwBMDErrorException("Error joging decklink deck", error);
}

void VTRWorker::seek(long timecode) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	seekTimecode = timecode;
	lastShuttleRate = INT8_MIN;
}

void VTRWorker::rew() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Rewind(true, &error) != S_OK)
		throwBMDErrorException("Error REW'ing decklink deck", error);
	seekTimecode = -1;
	lastShuttleRate = INT8_MIN;
}

void VTRWorker::ff() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->FastForward(true, &error) != S_OK)
		throwBMDErrorException("Error FF'ing decklink deck", error);
	seekTimecode = -1;
	lastShuttleRate = INT8_MIN;
}

void VTRWorker::stepBack() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->StepBack(&error) != S_OK)
		throwBMDErrorException("Error stepping back decklink deck", error);
	seekTimecode = -1;
	lastShuttleRate = INT8_MIN;
}

void VTRWorker::stepForward() {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->StepForward(&error) != S_OK)
		throwBMDErrorException("Error stepping forward decklink deck", error);
	seekTimecode = -1;
	lastShuttleRate = INT8_MIN;
}

void VTRWorker::startCapture(long in, long out) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if (mode != "CONTROL")
		return;
	std::cerr << "Starting capture " << in << " -> " << out << std::endl;
	actionIn = conformTimecode(in);
	actionOut = conformTimecode(out);
	seek(conformTimecode(in - 100));
	mode = "CAPTURE";
}

void VTRWorker::startExport(long in, long out) {
	std::unique_lock<std::recursive_mutex> lock(mutex);
	std::cerr << "Starting export " << in << " -> " << out << std::endl;

	// set export timecode (via rs422 deck command)
	std::shared_ptr<uint8_t> timecodePtr = std::shared_ptr<uint8_t>(new uint8_t[6]);
	uint8_t* timecode = timecodePtr.get();
	timecode[0] = 0x44;
	timecode[1] = 0x04;
	BMDTimecodeBCD bcd = BMDTools::toBCD(in); // start timecode here
	std::memcpy(timecode + 2, &bcd, 4);
	sendDeckCommand(timecode, 6);

	// start crash record export
	BMDDeckControlError error;
	if (deckLinkDeckControl->CrashRecordStart(&error) != S_OK)
		throwBMDErrorException("Error starting decklink deck assemble export", error);

	//
	mode = "EXPORT";
	if (out != -1) {
		actionOut = conformTimecode(out);
	} else {
		actionOut = INT32_MAX;
	}
	print("ACTION:EXPORT:START");
}

void VTRWorker::sendDeckCommand(uint8_t* input, size_t inputSize) {
	uint32_t outputBufferSize = 1024;
	std::shared_ptr<uint8_t> output = std::shared_ptr<uint8_t>(new uint8_t[outputBufferSize]);
	uint32_t outputSize;
	BMDDeckControlError error;

	if (deckLinkDeckControl->SendCommand(input, inputSize, output.get(), &outputSize, outputBufferSize, &error) != S_OK)
		throwBMDErrorException("Error sending command", error);
}

void VTRWorker::throwBMDErrorException(std::string message, BMDDeckControlError error) {
	throw std::runtime_error(message + " : " + bmdErrorToString(error));
}

bool VTRWorker::isConnected() {
	std::unique_lock<std::mutex> connected_lock(connected_mutex);
	return connected;
}

void VTRWorker::worker() {
	//
	while (true) {
		std::unique_lock<std::recursive_mutex> lock(mutex);
		if (!opened)
			break;
		//
		std::unique_lock<std::mutex> bmdTimecodeLock(bmdTimecodeMutex);
		timecode = bmdTimecode;
		bmdTimecodeLock.unlock();
		//
		std::unique_lock<std::mutex> bmdStateLock(bmdStateMutex);
		state = bmdState;
		bmdStateLock.unlock();
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
		condition.wait_for(lock, std::chrono::milliseconds(10));
	}
}

long VTRWorker::conformTimecode(long timecode) {
	if (timecode < 0) {
		return TIMECODE_MAX + timecode;
	} else if (timecode >= TIMECODE_MAX) {
		return timecode - TIMECODE_MAX;
	} else {
		return timecode;
	}
}

void VTRWorker::seekToTimecode() {
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
		pause();
	} else {
		double rate = calculateFactor(delta, (delta > 0 ? 32*25 : -32*25)) / 25.0;
		shuttle(rate);
	}
}

void VTRWorker::captureActionCheck() {
	if (state == "STILL" && timecode == conformTimecode(actionIn - 100)) {
		play();
	} else if (state == "PLAYING" && timecode == actionIn) {
		print("ACTION:CAPTURE:START");
	} else if (state == "PLAYING" && timecode == actionOut) {
		print("ACTION:CAPTURE:COMPLETE");
		mode = "CONTROL";
		stop();
	}
}

void VTRWorker::exportActionCheck() {
	if (state == "RECORDING" && timecode == actionOut) {
		print("ACTION:EXPORT:COMPLETE");
		mode = "CONTROL";
		stop();
	}
}

long VTRWorker::calculateFactor(long delta, long factor) {
	if (std::abs(factor) <= 1)
		return (delta > 0 ? 1 : -1);
	long rate = delta / factor;
	if (std::abs(rate) > 0) {
		return factor;
	} else {
		return calculateFactor(delta, factor / 2);
	}
}

std::string VTRWorker::bmdErrorToString(BMDDeckControlError error) {
	if (error == bmdDeckControlNoError) {
		return "No error";
	} else if (error == bmdDeckControlModeError) {
		return "The deck is not in the correct mode for the desired operation";
	} else if (error == bmdDeckControlMissedInPointError) {
		return "The in point was missed while prerolling as the current timecode has passed the begin in / capture timecode";
	} else if (error == bmdDeckControlDeckTimeoutError) {
		return "Deck control timeout error";
	} else if (error == bmdDeckControlCommandFailedError) {
		return "A deck control command request has failed";
	} else if (error == bmdDeckControlDeviceAlreadyOpenedError) {
		return "The deck control device is already open";
	} else if (error == bmdDeckControlFailedToOpenDeviceError) {
		return "Deck control failed to open the serial device";
	} else if (error == bmdDeckControlInLocalModeError) {
		return "The deck in local mode and is no longer controllable";
	} else if (error == bmdDeckControlEndOfTapeError) {
		return "Deck control has reached or is trying to move past the end of the tape";
	} else if (error == bmdDeckControlUserAbortError) {
		return "Abort an export-to-tape or capture operation";
	} else if (error == bmdDeckControlNoTapeInDeckError) {
		return "There is currently no tape in the deck";
	} else if (error == bmdDeckControlNoVideoFromCardError) {
		return "A capture or export operation was attempted when the input signal was invalid";
	} else if (error == bmdDeckControlNoCommunicationError) {
		return "The deck is not responding to requests";
	} else if (error == bmdDeckControlUnknownError) {
		return "Deck control unknown error";
	} else {
		return "Error that should never happen :)";
	}
}

HRESULT VTRWorker::TimecodeUpdate(BMDTimecodeBCD currentTimecode) {
	std::unique_lock<std::mutex> lock(bmdTimecodeMutex);
	bmdTimecode = BMDTools::fromBCD(currentTimecode);
	return S_OK;
}

HRESULT VTRWorker::VTRControlStateChanged(BMDDeckControlVTRControlState newState, BMDDeckControlError error) {
	std::unique_lock<std::mutex> lock(bmdStateMutex);

	if (newState == bmdDeckControlNotInVTRControlMode) {
		bmdState = "STOPPED";
	} else if (newState == bmdDeckControlVTRControlPlaying) {
		bmdState = "PLAYING";
	} else if (newState == bmdDeckControlVTRControlRecording) {
		bmdState = "STATE:RECORDING";
	} else if (newState == bmdDeckControlVTRControlStill) {
		bmdState = "STILL";
	} else if (newState == bmdDeckControlVTRControlSeeking) {
		bmdState = "SEEKING";
	} else if (newState == bmdDeckControlVTRControlStopped) {
		bmdState = "STOPPED";
	}

	if (error != bmdDeckControlNoError)
		std::cerr << "ERROR Generic error" << std::endl;

	return S_OK;
}

HRESULT VTRWorker::DeckControlEventReceived(BMDDeckControlEvent event, BMDDeckControlError error) {
	/*if (event == bmdDeckControlPrepareForCaptureEvent) {
	 print("ACTION:CAPTURE:START");
	 } else if (event == bmdDeckControlCaptureCompleteEvent) {
	 print("ACTION:CAPTURE:COMPLETE");
	 } else if (event == bmdDeckControlAbortedEvent) {
	 print("ACTION:ABORT");
	 } else if (event == bmdDeckControlPrepareForExportEvent) {
	 print("ACTION:EXPORT:START");
	 } else if (event == bmdDeckControlExportCompleteEvent) {
	 print("ACTION:EXPORT:COMPLETE");
	 } */
	std::cerr << "control event received " << event << std::endl;
	return S_OK;
}

HRESULT VTRWorker::DeckControlStatusChanged(BMDDeckControlStatusFlags flags, uint32_t mask) {
	std::unique_lock<std::mutex> lock(connected_mutex);
	if ((flags & bmdDeckControlStatusDeckConnected) && (flags & bmdDeckControlStatusRemoteMode)) {
		connected = true;
		print("CONNECTED");
		std::cerr << "CONNECTED" << std::endl;
	} else if (connected) {
		connected = false;
		print("DISCONNECTED");
		std::cerr << "DISCONNECTED" << std::endl;
	}
	return S_OK;
}
