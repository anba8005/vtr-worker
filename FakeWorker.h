/*
 * FakeWorker.h
 *
 *  Created on: Aug 7, 2015
 *      Author: anba8005
 */

#ifndef FAKEWORKER_H_
#define FAKEWORKER_H_

#include "AWorker.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class FakeWorker: public AWorker {
public:
	FakeWorker();
	virtual ~FakeWorker();

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
	bool isConnected();
	void setConnected(bool connected);
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
	bool isControlMode();


	long timecode;
	long lastTimecode;
	long factor;
	long conformTimecode(long timecode);

	long seekTimecode;
	void seekToTimecode();
	long calculateFactor(long delta, long factor);

	long actionIn;
	long actionOut;
	void captureActionCheck();
	void exportActionCheck();



};

#endif /* FAKEWORKER_H_ */
