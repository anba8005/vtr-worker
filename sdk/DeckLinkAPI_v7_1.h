/* -LICENSE-START-
** Copyright (c) 2009 Blackmagic Design
**
** Permission is hereby granted, free of charge, to any person or organization
** obtaining a copy of the software and accompanying documentation covered by
** this license (the "Software") to use, reproduce, display, distribute,
** execute, and transmit the Software, and to prepare derivative works of the
** Software, and to permit third-parties to whom the Software is furnished to
** do so, all subject to the following:
** 
** The copyright notices in the Software and this entire statement, including
** the above license grant, this restriction and the following disclaimer,
** must be included in all copies of the Software, in whole or in part, and
** all derivative works of the Software, unless such copies or derivative
** works are solely in the form of machine-executable object code generated by
** a source language processor.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
** SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
** FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
** -LICENSE-END-
*/
/* DeckLinkAPI_v7_1.h */

#ifndef __DeckLink_API_v7_1_h__
#define __DeckLink_API_v7_1_h__

#include "../sdk/DeckLinkAPI.h"

// "B28131B6-59AC-4857-B5AC-CD75D5883E2F"
#define IID_IDeckLinkDisplayModeIterator_v7_1	(REFIID){0xB2,0x81,0x31,0xB6,0x59,0xAC,0x48,0x57,0xB5,0xAC,0xCD,0x75,0xD5,0x88,0x3E,0x2F}

// "AF0CD6D5-8376-435E-8433-54F9DD530AC3"
#define IID_IDeckLinkDisplayMode_v7_1			(REFIID){0xAF,0x0C,0xD6,0xD5,0x83,0x76,0x43,0x5E,0x84,0x33,0x54,0xF9,0xDD,0x53,0x0A,0xC3}

// "EBD01AFA-E4B0-49C6-A01D-EDB9D1B55FD9"
#define IID_IDeckLinkVideoOutputCallback_v7_1	(REFIID){0xEB,0xD0,0x1A,0xFA,0xE4,0xB0,0x49,0xC6,0xA0,0x1D,0xED,0xB9,0xD1,0xB5,0x5F,0xD9}

// "7F94F328-5ED4-4E9F-9729-76A86BDC99CC"
#define IID_IDeckLinkInputCallback_v7_1			(REFIID){0x7F,0x94,0xF3,0x28,0x5E,0xD4,0x4E,0x9F,0x97,0x29,0x76,0xA8,0x6B,0xDC,0x99,0xCC}

// "AE5B3E9B-4E1E-4535-B6E8-480FF52F6CE5"
#define IID_IDeckLinkOutput_v7_1				(REFIID){0xAE,0x5B,0x3E,0x9B,0x4E,0x1E,0x45,0x35,0xB6,0xE8,0x48,0x0F,0xF5,0x2F,0x6C,0xE5}

// "2B54EDEF-5B32-429F-BA11-BB990596EACD"
#define IID_IDeckLinkInput_v7_1					(REFIID){0x2B,0x54,0xED,0xEF,0x5B,0x32,0x42,0x9F,0xBA,0x11,0xBB,0x99,0x05,0x96,0xEA,0xCD}

// "333F3A10-8C2D-43CF-B79D-46560FEEA1CE"
#define IID_IDeckLinkVideoFrame_v7_1			(REFIID){0x33,0x3F,0x3A,0x10,0x8C,0x2D,0x43,0xCF,0xB7,0x9D,0x46,0x56,0x0F,0xEE,0xA1,0xCE}

// "C8B41D95-8848-40EE-9B37-6E3417FB114B"
#define IID_IDeckLinkVideoInputFrame_v7_1		(REFIID){0xC8,0xB4,0x1D,0x95,0x88,0x48,0x40,0xEE,0x9B,0x37,0x6E,0x34,0x17,0xFB,0x11,0x4B}

// "C86DE4F6-A29F-42E3-AB3A-1363E29F0788"
#define IID_IDeckLinkAudioInputPacket_v7_1		(REFIID){0xC8,0x6D,0xE4,0xF6,0xA2,0x9F,0x42,0xE3,0xAB,0x3A,0x13,0x63,0xE2,0x9F,0x07,0x88}

#if defined(__cplusplus)

class IDeckLinkDisplayModeIterator_v7_1;
class IDeckLinkDisplayMode_v7_1;
class IDeckLinkVideoFrame_v7_1;
class IDeckLinkVideoInputFrame_v7_1;
class IDeckLinkAudioInputPacket_v7_1;

class IDeckLinkDisplayModeIterator_v7_1 : public IUnknown
{
public:
	virtual	HRESULT	STDMETHODCALLTYPE	Next (IDeckLinkDisplayMode_v7_1* *deckLinkDisplayMode) = 0;
};


class IDeckLinkDisplayMode_v7_1 : public IUnknown
{
public:
	virtual	HRESULT			STDMETHODCALLTYPE	GetName (const char **name) = 0;
	virtual	BMDDisplayMode	STDMETHODCALLTYPE	GetDisplayMode () = 0;
	virtual	long			STDMETHODCALLTYPE	GetWidth () = 0;
	virtual	long			STDMETHODCALLTYPE	GetHeight () = 0;
	virtual	HRESULT			STDMETHODCALLTYPE	GetFrameRate (BMDTimeValue *frameDuration, BMDTimeScale *timeScale) = 0;
};

class IDeckLinkVideoOutputCallback_v7_1 : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE	ScheduledFrameCompleted (IDeckLinkVideoFrame_v7_1* completedFrame, BMDOutputFrameCompletionResult result) = 0;
};

class IDeckLinkInputCallback_v7_1 : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE	VideoInputFrameArrived (IDeckLinkVideoInputFrame_v7_1* videoFrame, IDeckLinkAudioInputPacket_v7_1* audioPacket) = 0;
};

// IDeckLinkOutput_v7_1.  Created by QueryInterface from IDeckLink.
class IDeckLinkOutput_v7_1 : public IUnknown
{
public:
	// Display mode predicates
	virtual	HRESULT	STDMETHODCALLTYPE	DoesSupportVideoMode (BMDDisplayMode displayMode, BMDPixelFormat pixelFormat, BMDDisplayModeSupport *result) = 0;
	virtual HRESULT	STDMETHODCALLTYPE	GetDisplayModeIterator (IDeckLinkDisplayModeIterator_v7_1* *iterator) = 0;
	
	
	// Video output
	virtual HRESULT STDMETHODCALLTYPE	EnableVideoOutput (BMDDisplayMode displayMode) = 0;
	virtual HRESULT STDMETHODCALLTYPE	DisableVideoOutput () = 0;
	
	virtual HRESULT STDMETHODCALLTYPE	SetVideoOutputFrameMemoryAllocator (IDeckLinkMemoryAllocator* theAllocator) = 0;
	virtual HRESULT STDMETHODCALLTYPE	CreateVideoFrame (int32_t width, int32_t height, int32_t rowBytes, BMDPixelFormat pixelFormat, BMDFrameFlags flags, IDeckLinkVideoFrame_v7_1* *outFrame) = 0;
	virtual HRESULT STDMETHODCALLTYPE	CreateVideoFrameFromBuffer (void* buffer, int32_t width, int32_t height, int32_t rowBytes, BMDPixelFormat pixelFormat, BMDFrameFlags flags, IDeckLinkVideoFrame_v7_1* *outFrame) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE	DisplayVideoFrameSync (IDeckLinkVideoFrame_v7_1* theFrame) = 0;
	virtual HRESULT STDMETHODCALLTYPE	ScheduleVideoFrame (IDeckLinkVideoFrame_v7_1* theFrame, BMDTimeValue displayTime, BMDTimeValue displayDuration, BMDTimeScale timeScale) = 0;
	virtual HRESULT STDMETHODCALLTYPE	SetScheduledFrameCompletionCallback (IDeckLinkVideoOutputCallback_v7_1* theCallback) = 0;
	
	
	// Audio output
	virtual HRESULT STDMETHODCALLTYPE	EnableAudioOutput (BMDAudioSampleRate sampleRate, BMDAudioSampleType sampleType, uint32_t channelCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE	DisableAudioOutput () = 0;
	
	virtual HRESULT STDMETHODCALLTYPE	WriteAudioSamplesSync (void* buffer, uint32_t sampleFrameCount, uint32_t *sampleFramesWritten) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE	BeginAudioPreroll () = 0;
	virtual HRESULT STDMETHODCALLTYPE	EndAudioPreroll () = 0;
	virtual HRESULT STDMETHODCALLTYPE	ScheduleAudioSamples (void* buffer, uint32_t sampleFrameCount, BMDTimeValue streamTime, BMDTimeScale timeScale, uint32_t *sampleFramesWritten) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE	GetBufferedAudioSampleFrameCount (uint32_t *bufferedSampleCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE	FlushBufferedAudioSamples () = 0;
	
	virtual HRESULT STDMETHODCALLTYPE	SetAudioCallback (IDeckLinkAudioOutputCallback* theCallback) = 0;
	
	
	// Output control
	virtual HRESULT STDMETHODCALLTYPE	StartScheduledPlayback (BMDTimeValue playbackStartTime, BMDTimeScale timeScale, double playbackSpeed) = 0;
	virtual HRESULT STDMETHODCALLTYPE	StopScheduledPlayback (BMDTimeValue stopPlaybackAtTime, BMDTimeValue *actualStopTime, BMDTimeScale timeScale) = 0;
	virtual HRESULT STDMETHODCALLTYPE	GetHardwareReferenceClock (BMDTimeScale desiredTimeScale, BMDTimeValue *elapsedTimeSinceSchedulerBegan) = 0;
};

// IDeckLinkInput_v7_1.  Created by QueryInterface from IDeckLink.
class IDeckLinkInput_v7_1 : public IUnknown
{
public:
	virtual	HRESULT	STDMETHODCALLTYPE	DoesSupportVideoMode (BMDDisplayMode displayMode, BMDPixelFormat pixelFormat, BMDDisplayModeSupport *result) = 0;
	virtual HRESULT	STDMETHODCALLTYPE	GetDisplayModeIterator (IDeckLinkDisplayModeIterator_v7_1 **iterator) = 0;
	
	// Video input
	virtual HRESULT STDMETHODCALLTYPE	EnableVideoInput (BMDDisplayMode displayMode, BMDPixelFormat pixelFormat, BMDVideoInputFlags flags) = 0;
	virtual HRESULT STDMETHODCALLTYPE	DisableVideoInput () = 0;
	
	// Audio input
	virtual HRESULT STDMETHODCALLTYPE	EnableAudioInput (BMDAudioSampleRate sampleRate, BMDAudioSampleType sampleType, uint32_t channelCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE	DisableAudioInput () = 0;
	virtual HRESULT STDMETHODCALLTYPE	ReadAudioSamples (void* buffer, uint32_t sampleFrameCount, uint32_t *sampleFramesRead, BMDTimeValue *audioPacketTime, BMDTimeScale timeScale) = 0;
	virtual HRESULT STDMETHODCALLTYPE	GetBufferedAudioSampleFrameCount (uint32_t *bufferedSampleCount) = 0;
	
	// Input control
	virtual HRESULT	STDMETHODCALLTYPE	StartStreams () = 0;
	virtual HRESULT	STDMETHODCALLTYPE	StopStreams () = 0;
	virtual HRESULT	STDMETHODCALLTYPE	PauseStreams () = 0;
	virtual HRESULT STDMETHODCALLTYPE	SetCallback (IDeckLinkInputCallback_v7_1* theCallback) = 0;
};

// IDeckLinkVideoFrame_v7_1.  Created by IDeckLinkOutput::CreateVideoFrame.
class IDeckLinkVideoFrame_v7_1 : public IUnknown
{
public:
	virtual long STDMETHODCALLTYPE					GetWidth () = 0;
	virtual long STDMETHODCALLTYPE					GetHeight () = 0;
	virtual long STDMETHODCALLTYPE					GetRowBytes () = 0;
	virtual BMDPixelFormat STDMETHODCALLTYPE		GetPixelFormat () = 0;
	virtual BMDFrameFlags STDMETHODCALLTYPE			GetFlags () = 0;
	virtual HRESULT STDMETHODCALLTYPE				GetBytes (void* *buffer) = 0;
};

// IDeckLinkVideoInputFrame_v7_1.  Provided by the IDeckLinkInput_v7_1 frame arrival callback.
class IDeckLinkVideoInputFrame_v7_1 : public IDeckLinkVideoFrame_v7_1
{
public:
	virtual HRESULT STDMETHODCALLTYPE			GetFrameTime (BMDTimeValue *frameTime, BMDTimeValue *frameDuration, BMDTimeScale timeScale) = 0;
};

// IDeckLinkAudioInputPacket_v7_1.  Provided by the IDeckLinkInput_v7_1 callback.
class IDeckLinkAudioInputPacket_v7_1 : public IUnknown
{
public:
	virtual long STDMETHODCALLTYPE				GetSampleCount () = 0;
	virtual HRESULT STDMETHODCALLTYPE			GetBytes (void* *buffer) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE			GetAudioPacketTime (BMDTimeValue *packetTime, BMDTimeScale timeScale) = 0;
};

#endif		// defined(__cplusplus)

#endif		// __DeckLink_API_v7_1_h__

