/*
COPYRIGHT AND PERMISSION NOTICE

Copyright (c) 1995-2011 International Business Machines Corporation and others

All rights reserved.

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, 
merge, publish, distribute, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
provided that the above copyright notice(s) and this permission 
notice appear in all copies of the Software and that both the 
above copyright notice(s) and this permission notice appear in 
supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE 
COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR 
ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR 
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.

Except as contained in this notice, the name of a copyright holder 
shall not be used in advertising or otherwise to promote the sale, 
use or other dealings in this Software without prior written 
authorization of the copyright holder. 
 */

/* 
Modified csrutf8.cpp from ICU for the purposes of MTA.

Heuristically detect whether a file is in UTF-8 or not.
*/

int icu_getUTF8Confidence (unsigned char* input, int len) {
    bool hasBOM = false;
    int numValid = 0;
    int numInvalid = 0;
    int i;
    int trailBytes = 0;
    int confidence;

    if (len >= 3 && 
        input[0] == 0xEF && input[1] == 0xBB && input[2] == 0xBF) {
            hasBOM = true;
    }

    // Scan for multi-byte sequences
    for (i=0; i < len; i += 1) {
        int b = input[i];

        if ((b & 0x80) == 0) {
            continue;   // ASCII
        }

        // Hi bit on char found.  Figure out how long the sequence should be
        if ((b & 0x0E0) == 0x0C0) {
            trailBytes = 1;
        } else if ((b & 0x0F0) == 0x0E0) {
            trailBytes = 2;
        } else if ((b & 0x0F8) == 0xF0) {
            trailBytes = 3;
        } else {
            numInvalid += 1;

            if (numInvalid > 5) {
                break;
            }

            trailBytes = 0;
        }

        // Verify that we've got the right number of trail bytes in the sequence
        for (;;) {
            i += 1;

            if (i >= len) {
                break;
            }

            b = input[i];

            if ((b & 0xC0) != 0x080) {
                numInvalid += 1;
                break;
            }

            if (--trailBytes == 0) {
                numValid += 1;
                break;
            }
        }

    }

    // Cook up some sort of confidence score, based on presense of a BOM
    //    and the existence of valid and/or invalid multi-byte sequences.
    confidence = 0;
    if (hasBOM && numInvalid == 0) {
        confidence = 100;
    } else if (hasBOM && numValid > numInvalid*10) {
        confidence = 80;
    } else if (numValid > 3 && numInvalid == 0) {
        confidence = 100;
    } else if (numValid > 0 && numInvalid == 0) {
        confidence = 80;
    } else if (numValid == 0 && numInvalid == 0) {
        // Plain ASCII.
        confidence = 10;
    } else if (numValid > numInvalid*10) {
        // Probably corruput utf-8 data.  Valid sequences aren't likely by chance.
        confidence = 25;
    }

    return confidence;
}
