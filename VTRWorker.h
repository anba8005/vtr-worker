/*
 * VTRWorker.h
 *
 *  Created on: Aug 7, 2015
 *      Author: anba8005
 */

#ifndef VTRWORKER_H_
#define VTRWORKER_H_

#include <mutex>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <condition_variable>
#include <thread>

#include "AWorker.h"
#include "sdk/DeckLinkAPI.h"

class VTRWorker: public AWorker, public IDeckLinkDeckControlStatusCallback {
public:
	VTRWorker(int id);
	virtual ~VTRWorker();

	void open();
	void close();

	void play();
	void pause();
	void stop();
	void eject();

	void shuttle(double rate);
	void jog(double rate);
	void seek(long timecode);
	void rew();
	void ff();
	void stepBack();
	void stepForward();

	void startCapture(long in, long out);
	void startExport(long in, long out);

private:
	int id;
	std::shared_ptr<IDeckLink> deckLink;
	std::shared_ptr<IDeckLinkDeckControl> deckLinkDeckControl;
	bool opened;

	bool isConnected();
	bool connected;
	std::mutex connected_mutex;

	void worker();
	std::recursive_mutex mutex;
	std::thread thread;
	std::condition_variable_any condition;

	std::string state;
	std::string lastState;
	std::string mode;
	std::string lastMode;

	long timecode;
	long lastTimecode;
	double lastShuttleRate;
	long conformTimecode(long timecode);

	long seekTimecode;
	void seekToTimecode();
	long calculateFactor(long delta, long factor);

	long actionIn;
	long actionOut;
	void captureActionCheck();
	void exportActionCheck();

	long bmdTimecode;
	std::mutex bmdTimecodeMutex;

	std::string bmdState;
	std::mutex bmdStateMutex;

	static void release(IUnknown* o);

	void throwBMDErrorException(std::string message, BMDDeckControlError error);
	std::string bmdErrorToString(BMDDeckControlError error);
	void sendDeckCommand(uint8_t* input, size_t inputSize);

public:
	// Decklink common callback interface
	virtual HRESULT QueryInterface(REFIID, LPVOID*) {
		return E_NOINTERFACE;
	}
	virtual ULONG AddRef() {
		return S_OK;
	}
	virtual ULONG Release() {
		return S_OK;
	}

	// IDeckLinkDeckControlStatusCallback interface
	virtual HRESULT TimecodeUpdate(BMDTimecodeBCD currentTimecode);
	virtual HRESULT VTRControlStateChanged(BMDDeckControlVTRControlState newState, BMDDeckControlError error);
	virtual HRESULT DeckControlEventReceived(BMDDeckControlEvent event, BMDDeckControlError error);
	virtual HRESULT DeckControlStatusChanged(BMDDeckControlStatusFlags flags, uint32_t mask);
};

#endif /* VTRWORKER_H_ */
