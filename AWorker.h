/*
 * AWorker.h
 *
 *  Created on: Aug 7, 2015
 *      Author: anba8005
 */

#ifndef AWORKER_H_
#define AWORKER_H_

#include <string>
#include <mutex>
#include <vector>

class AWorker {
public:
	AWorker();
	virtual ~AWorker();

	virtual void open() = 0;
	virtual void close() = 0;

	virtual void play() = 0;
	virtual void pause() = 0;
	virtual void stop() = 0;
	virtual void eject() = 0;

	virtual void shuttle(double rate) = 0;
	virtual void jog(double rate) = 0;
	virtual void seek(long timecode) = 0;
	virtual void rew() = 0;
	virtual void ff() = 0;
	virtual void stepBack() = 0;
	virtual void stepForward() = 0;

	virtual void startCapture(long in, long out) = 0;
	virtual void startExport(long in, long out = -1) = 0;

	void print_commit();
protected:
	void print(std::string s);
private:
	std::mutex print_mutex;
	std::vector<std::string> print_buffer;
};

#endif /* AWORKER_H_ */
