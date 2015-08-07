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

VTRWorker::VTRWorker(int id) :
		id(id) {
	connected = false;
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
	std::unique_lock<std::mutex> lock(mutex);

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
	std::unique_lock<std::mutex> connected_lock(connected_mutex);
	connected_condition.wait_for(connected_lock, std::chrono::milliseconds(2000));
	if (!connected)
		throw std::runtime_error("Unable to connect VTR");
	connected_lock.unlock();

	// get current
	BMDDeckControlMode modeCurrent;
	BMDDeckControlVTRControlState state;
	BMDDeckControlStatusFlags flags;
	if (deckLinkDeckControl->GetCurrentState(&modeCurrent, &state, &flags) == S_OK) {
		VTRControlStateChanged(state, bmdDeckControlNoError);
	} else {
		throw std::runtime_error("Unable to get decklink deck state");
	}
}

void VTRWorker::close() {
	std::unique_lock<std::mutex> lock(mutex);
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
}

void VTRWorker::play() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Play(&error) != S_OK) {
		throwBMDErrorException("Error starting decklink deck", error);
	}
}

void VTRWorker::pause() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Stop(&error) != S_OK) {
		throwBMDErrorException("Error stopping decklink deck", error);
	}
}

void VTRWorker::stop() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Stop(&error) != S_OK) {
		throwBMDErrorException("Error stopping decklink deck", error);
	}
}

void VTRWorker::eject() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Eject(&error) != S_OK) {
		throwBMDErrorException("Error ejecting decklink deck", error);
	}
}

void VTRWorker::shuttle(double rate) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);

	BMDDeckControlError error;
	if (deckLinkDeckControl->Shuttle(rate, &error) != S_OK)
		throwBMDErrorException("Error shuttling decklink deck", error);
}

void VTRWorker::jog(double rate) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Jog(rate, &error) != S_OK)
		throwBMDErrorException("Error joging decklink deck", error);
}

void VTRWorker::seek(long timecode) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->GoToTimecode(BMDTools::toBCD(timecode), &error) != S_OK)
		throwBMDErrorException("Error seeking decklink deck", error);
}

void VTRWorker::rew() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->Rewind(true, &error) != S_OK)
		throwBMDErrorException("Error REW'ing decklink deck", error);
}

void VTRWorker::ff() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->FastForward(true, &error) != S_OK)
		throwBMDErrorException("Error FF'ing decklink deck", error);
}

void VTRWorker::stepBack() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->StepBack(&error) != S_OK)
		throwBMDErrorException("Error stepping back decklink deck", error);
}

void VTRWorker::stepForward() {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->StepForward(&error) != S_OK)
		throwBMDErrorException("Error stepping forward decklink deck", error);
}

void VTRWorker::startCapture(long in, long out) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);
	BMDDeckControlError error;
	if (deckLinkDeckControl->StartCapture(false, BMDTools::toBCD(in), BMDTools::toBCD(out), &error) != S_OK)
		throwBMDErrorException("Error starting decklink deck capture", error);
}

void VTRWorker::startExport(long in, long out) {
	if (!isConnected())
		return;
	std::unique_lock<std::mutex> lock(mutex);

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
}

void VTRWorker::sendDeckCommand(uint8_t* input, size_t inputSize) {
	uint32_t outputBufferSize = 1024;
	std::shared_ptr < uint8_t > output = std::shared_ptr < uint8_t > (new uint8_t[outputBufferSize]);
	uint32_t outputSize;
	BMDDeckControlError error;

	if (deckLinkDeckControl->SendCommand(input, inputSize, output.get(), &outputSize, outputBufferSize, &error)
			!= S_OK)
		throwBMDErrorException("Error sending command", error);
}

void VTRWorker::throwBMDErrorException(std::string message, BMDDeckControlError error) {
	throw std::runtime_error(message + " : " + bmdErrorToString(error));
}

bool VTRWorker::isConnected() {
	std::unique_lock<std::mutex> connected_lock(connected_mutex);
	return connected;
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
	print("TIMECODE:" + BMDTools::fromBCD(currentTimecode));
	return S_OK;
}

HRESULT VTRWorker::VTRControlStateChanged(BMDDeckControlVTRControlState newState, BMDDeckControlError error) {

	if (newState == bmdDeckControlNotInVTRControlMode) {
		print("STATE:NONE");
	} else if (newState == bmdDeckControlVTRControlPlaying) {
		print("STATE:PLAYING");
	} else if (newState == bmdDeckControlVTRControlRecording) {
		print("STATE:RECORDING");
	} else if (newState == bmdDeckControlVTRControlStill) {
		print("STATE:STILL");
	} else if (newState == bmdDeckControlVTRControlShuttleForward || newState == bmdDeckControlVTRControlShuttleReverse
			|| newState == bmdDeckControlVTRControlJogForward || newState == bmdDeckControlVTRControlJogReverse) {
		print("STATE:SEEKING");
	} else if (newState == bmdDeckControlVTRControlStopped) {
		print("STATE:STOPPED");
	}

	if (error != bmdDeckControlNoError)
		std::cerr << "[error] Generic error" << std::endl;

	return S_OK;
}

HRESULT VTRWorker::DeckControlEventReceived(BMDDeckControlEvent event, BMDDeckControlError error) {
	if (event == bmdDeckControlPrepareForCaptureEvent) {
		print("ACTION:CAPTURE:START");
	} else if (event == bmdDeckControlCaptureCompleteEvent) {
		print("ACTION:CAPTURE:COMPLETE");
	} else if (event == bmdDeckControlAbortedEvent) {
		print("ACTION:ABORT");
	} else if (event == bmdDeckControlPrepareForExportEvent) {
		print("ACTION:EXPORT:START");
	} else if (event == bmdDeckControlExportCompleteEvent) {
		print("ACTION:EXPORT:COMPLETE");
	}
	return S_OK;
}

HRESULT VTRWorker::DeckControlStatusChanged(BMDDeckControlStatusFlags flags, uint32_t mask) {
	std::unique_lock<std::mutex> lock(connected_mutex);
	if ((flags & bmdDeckControlStatusDeckConnected) && (flags & bmdDeckControlStatusRemoteMode)) {
		connected = true;
		print("CONNECTED");
	} else {
		connected = false;
		print("DISCONNECTED");
	}
	connected_condition.notify_one();
	return S_OK;
}
