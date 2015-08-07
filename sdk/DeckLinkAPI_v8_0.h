/* -LICENSE-START-
** Copyright (c) 2011 Blackmagic Design
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

#ifndef BMD_DECKLINKAPI_v8_0_H
#define BMD_DECKLINKAPI_v8_0_H

#include "../sdk/DeckLinkAPI.h"

// Interface ID Declarations

#define IID_IDeckLink_v8_0                                    /* 62BFF75D-6569-4E55-8D4D-66AA03829ABC */ (REFIID){0x62,0xBF,0xF7,0x5D,0x65,0x69,0x4E,0x55,0x8D,0x4D,0x66,0xAA,0x03,0x82,0x9A,0xBC}
#define IID_IDeckLinkIterator_v8_0                            /* 74E936FC-CC28-4A67-81A0-1E94E52D4E69 */ (REFIID){0x74,0xE9,0x36,0xFC,0xCC,0x28,0x4A,0x67,0x81,0xA0,0x1E,0x94,0xE5,0x2D,0x4E,0x69}

#if defined (__cplusplus)

/* Interface IDeckLink_v8_0 - represents a DeckLink device */

class IDeckLink_v8_0 : public IUnknown
{
public:
    virtual HRESULT GetModelName (/* out */ const char **modelName) = 0;
};

/* Interface IDeckLinkIterator_v8_0 - enumerates installed DeckLink hardware */

class IDeckLinkIterator_v8_0 : public IUnknown
{
public:
    virtual HRESULT Next (/* out */ IDeckLink_v8_0 **deckLinkInstance) = 0;
};

extern "C" {
    IDeckLinkIterator_v8_0*                     CreateDeckLinkIteratorInstance_v8_0 (void);
};

#endif	// defined __cplusplus

#endif /* defined(BMD_DECKLINKAPI_v8_0_H) */
