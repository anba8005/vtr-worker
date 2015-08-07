/*
 * BMDTools.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: anba8005
 */

#include "BMDTools.h"
#include <string.h>
#include <stdlib.h>

BMDTimecodeBCD BMDTools::toBCD(long timecode) {
	uint8_t values[4];
	values[3] = timecode / (3600 * 25);
	timecode %= 3600 * 25;
	values[2] = timecode / (60 * 25);
	timecode %= 60 * 25;
	values[1] = timecode / 25;
	values[0] = timecode % 25;

	BMDTimecodeBCD result = 0;
	uint8_t* t12m = (uint8_t*) &result;
	t12m[0] = ((values[0] % 10) & 0x0f) | (((values[0] / 10) & 0x3) << 4);
	t12m[1] = ((values[1] % 10) & 0x0f) | (((values[1] / 10) & 0x7) << 4);
	t12m[2] = ((values[2] % 10) & 0x0f) | (((values[2] / 10) & 0x7) << 4);
	t12m[3] = ((values[3] % 10) & 0x0f) | (((values[3] / 10) & 0x3) << 4);

	return result;
}

long BMDTools::fromBCD(BMDTimecodeBCD timecode) {
	uint8_t values[4];

	uint8_t* t12m = (uint8_t*) &timecode;

	values[0] = ((t12m[0] >> 4) & 0x03) * 10 + (t12m[0] & 0xf);
	values[1] = ((t12m[1] >> 4) & 0x07) * 10 + (t12m[1] & 0xf);
	values[2] = ((t12m[2] >> 4) & 0x07) * 10 + (t12m[2] & 0xf);
	values[3] = ((t12m[3] >> 4) & 0x03) * 10 + (t12m[3] & 0xf);

	return values[0] + values[1] * 25 + values[2] * 25 * 60 + values[3] * 25 * 3600;
}

#ifdef WIN32

char* BMDTools::toCString(BMDString bmdString) {
	char* cString = NULL;
	if ( bmdString ) {
		int size = WideCharToMultiByte( CP_UTF8, 0, bmdString, -1, NULL, 0, NULL, NULL );
		if (size) {
			cString = new char[ size ];
			size = WideCharToMultiByte( CP_UTF8, 0, bmdString, -1, cString, size, NULL, NULL );
			if ( !size ) {
				delete[] cString;
				cString = NULL;
			}
		}
	}
	return cString;
}

void BMDTools::freeCString(char* cString) {
	delete[] cString;
}

void BMDTools::freeBMDString(BMDString bmdString) {
	if (bmdString) free((void*)bmdString);
}

#else

char* BMDTools::toCString(BMDString bmdString) {
	return bmdString ? strdup((const char*) bmdString) : NULL;
}

void BMDTools::freeCString(char* cString) {
}

void BMDTools::freeBMDString(BMDString bmdString) {
	if (bmdString)
		free((void*)bmdString);
}

#endif
