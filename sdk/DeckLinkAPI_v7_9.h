/* -LICENSE-START-
** Copyright (c) 2010 Blackmagic Design
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

/* DeckLinkAPI_v7_9.h */

#ifndef __DeckLink_API_v7_9_h__
#define __DeckLink_API_v7_9_h__

#include "../sdk/DeckLinkAPI.h"

// Interface ID Declarations
#define IID_IDeckLinkDeckControl_v7_9                    /* A4D81043-0619-42B7-8ED6-602D29041DF7 */ (REFIID){0xA4,0xD8,0x10,0x43,0x06,0x19,0x42,0xB7,0x8E,0xD6,0x60,0x2D,0x29,0x04,0x1D,0xF7}

#if defined(__cplusplus)


// Forward Declarations
class IDeckLinkDeckControl_v7_9;

/* Interface IDeckLinkDeckControl_v7_9 - Deck Control main interface */
class IDeckLinkDeckControl_v7_9 : public IUnknown
{
public:
    virtual HRESULT Open (/* in */ BMDTimeScale timeScale, /* in */ BMDTimeValue timeValue, /* in */ bool timecodeIsDropFrame, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT Close (/* in */ bool standbyOn) = 0;
    virtual HRESULT GetCurrentState (/* out */ BMDDeckControlMode *mode, /* out */ BMDDeckControlVTRControlState *vtrControlState, /* out */ BMDDeckControlStatusFlags *flags) = 0;
    virtual HRESULT SetStandby (/* in */ bool standbyOn) = 0;
    virtual HRESULT Play (/* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT Stop (/* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT TogglePlayStop (/* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT Eject (/* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT GoToTimecode (/* in */ BMDTimecodeBCD timecode, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT FastForward (/* in */ bool viewTape, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT Rewind (/* in */ bool viewTape, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT StepForward (/* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT StepBack (/* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT Jog (/* in */ double rate, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT Shuttle (/* in */ double rate, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT GetTimecodeString (/* out */ const char **currentTimeCode, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT GetTimecode (/* out */ IDeckLinkTimecode **currentTimecode, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT GetTimecodeBCD (/* out */ BMDTimecodeBCD *currentTimecode, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT SetPreroll (/* in */ uint32_t prerollSeconds) = 0;
    virtual HRESULT GetPreroll (/* out */ uint32_t *prerollSeconds) = 0;
    virtual HRESULT SetExportOffset (/* in */ int32_t exportOffsetFields) = 0;
    virtual HRESULT GetExportOffset (/* out */ int32_t *exportOffsetFields) = 0;
    virtual HRESULT GetManualExportOffset (/* out */ int32_t *deckManualExportOffsetFields) = 0;
    virtual HRESULT SetCaptureOffset (/* in */ int32_t captureOffsetFields) = 0;
    virtual HRESULT GetCaptureOffset (/* out */ int32_t *captureOffsetFields) = 0;
    virtual HRESULT StartExport (/* in */ BMDTimecodeBCD inTimecode, /* in */ BMDTimecodeBCD outTimecode, /* in */ BMDDeckControlExportModeOpsFlags exportModeOps, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT StartCapture (/* in */ bool useVITC, /* in */ BMDTimecodeBCD inTimecode, /* in */ BMDTimecodeBCD outTimecode, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT GetDeviceID (/* out */ uint16_t *deviceId, /* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT Abort (void) = 0;
    virtual HRESULT CrashRecordStart (/* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT CrashRecordStop (/* out */ BMDDeckControlError *error) = 0;
    virtual HRESULT SetCallback (/* in */ IDeckLinkDeckControlStatusCallback *callback) = 0;
	
protected:
    virtual ~IDeckLinkDeckControl_v7_9 () {}; // call Release method to drop reference count
};



#endif      // defined(__cplusplus)
#endif      // __DeckLink_API_v7_9_h__
