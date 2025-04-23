/*
~~~~~~

Code Sample License Agreement
Effective Date: 4/15/2025
Cinnafilm, Inc. ("Licensor") grants Finn Thomas ("Licensee") a non-exclusive, 
non-transferable, revocable license to use the provided code samples ("Code") 
under the following terms:
- Permitted Use: Licensee may use the Code solely for personal, non-commercial 
purposes, such as inclusion in a portfolio or demonstration during job 
interviews.
- Restrictions: Licensee may not: (a) use the Code for any commercial purpose; 
(b) distribute, sell, sublicense, or otherwise share the Code with third 
parties; (c) modify the Code for purposes beyond personal demonstration; or 
(d) claim ownership of the Code.
- Ownership: The Code remains the exclusive property of Cinnafilm, Inc.
- Termination: This license may be terminated by Licensor at any time with 
written notice to Licensee, after which Licensee must cease all use of the Code.
No Warranty: The Code is provided "as is," with no warranties of any kind.
By using the Code, Licensee agrees to these terms.

~~~~~
*/

#pragma once

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory> // Include the memory header to use shared_ptr

#include "nvnmos.h"

 // example video format
#ifndef VIDEO_DESCRIPTION
#define VIDEO_DESCRIPTION "YCbCr-4:2:2, 10 bit, 1920 x 1080, progressive, 50 Hz"
#endif
#ifndef VIDEO_ENCODING_PARAMETERS
#define VIDEO_ENCODING_PARAMETERS "raw/90000"
#endif
#ifndef VIDEO_FORMAT_SPECIFIC_PARAMETERS
#define VIDEO_FORMAT_SPECIFIC_PARAMETERS "sampling=YCbCr-4:2:2; width=1920; height=1080; exactframerate=50; depth=10; TCS=SDR; colorimetry=BT709; PM=2110GPM; SSN=ST2110-20:2017; TP=2110TPN; "
#endif

// example audio format
#ifndef AUDIO_DESCRIPTION
#define AUDIO_DESCRIPTION "2 ch, 48 kHz, 24 bit"
#endif
#ifndef AUDIO_ENCODING_PARAMETERS
#define AUDIO_ENCODING_PARAMETERS "L24/48000/2"
#endif
#ifndef AUDIO_FORMAT_SPECIFIC_PARAMETERS
#define AUDIO_FORMAT_SPECIFIC_PARAMETERS "channel-order=SMPTE2110.(ST); "
#endif

#ifndef CLK_PTP
#define CLK_PTP true
#endif
