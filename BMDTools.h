/*
 * BMDTools.h
 *
 *  Created on: Mar 30, 2011
 *      Author: anba8005
 */

#ifndef BMDTOOLS_H_
#define BMDTOOLS_H_



#ifdef WIN32
	#include "DeckLinkAPI_h.h"
	typedef BSTR BMDString;
#else
	#include "sdk/DeckLinkAPI.h"
	typedef const char* BMDString;
#endif

class BMDTools {
public:
	static BMDTimecodeBCD toBCD(long timecode);
	static long fromBCD(BMDTimecodeBCD timecode);
	static char* toCString(BMDString bmdString);
	static void freeCString(char* cString);
	static void freeBMDString(BMDString bmdString);
};

#endif /* BMDTOOLS_H_ */
