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
namespace Cf
{
	enum class SDPMediaType
	{
		VIDEO,
		AUDIO,
		DATA,
		UNKNOWN
	};

	// Specificed in 2110-20:2022 7.5
	enum class SDPColorimetry
	{
		BT601,
		BT709,
		BT2020,
		BT2100,
		ST2065_1,
		ST2065_3,
		UNSPECIFIED,
		XYZ,
		ALPHA
	};

	// Specified in 2110_20:2022 7.4.1
	enum class SDPSampling
	{
		// Non-const Y`Cb`Cr` including:
		//   BT.601, BT.709, BT.2020, BT.2100
		YCBCR_444,
		YCBCR_422,
		YCBCR_420,
		// Constant Luminance Yc`Cbc`Crc` including:
		//   BT.2020
		CLYCBCR_444,
		CLYCBCR_422,
		CLYCBCR_420,
		// Constant Intensity ICtCp including:
		//   BT.2100
		ICTCP_444,
		ICTCP_422,
		ICTCP_420,
		// 4:4:4 R`G`B` or RGB including:
		//   BT.601, BT.709, BT.2020, BT.2100, ST.2065-1, ST.2065-3
		RGB,
		// 4:4:4 X`Y`B` including:
		//   ST.428-1
		XYZ,
		// Key (alpha) signals as defined in ST. RP 157
		KEY
	};

	// Specified in 2110_20:2022 7.4.2
	enum class SDPDepth
	{
		INT_8 = 8,
		INT_10 = 10,
		INT_12 = 12,
		INT_16 = 16,
		FLOAT_16 = 16
	};

	// Specified in 2110-20:2022 6.3
	enum class SDPPackingMode
	{
		GPM,	// General Packing Mode
		BPM		// Block Packing Mode
	};

	// Specified in 2110-20:2022 7.6. Default = SDR
	enum class SDPTransferCharacteristicSystem
	{
		SDR,			// Standard Dynamic Range
		PQ,				// Perceptual Quantization 
		HLG,			// Hybrid Log Gamma
		LINEAR,			// Linear encoded floating-point samples (depth=16f) 
		BT2100LINPQ,	// Linear encoded floating point samples (depth=16f) normalized from PQ
		BT2100LINHLG,	// Linear encoded floating point samples (depth=16f) normalized from HLG
		ST2065_1,		// Linear encoded floating-point samples (depth=16f) as specified in SMPTE ST 2065-1
		ST428_1,		// Streams using transfer characteristic specified in SMPTE ST428-1 4.3.
		DENSITY,		// Streams using density encoded samples specified in SMPTE ST2065-3.
		ST2115LOGS3,	// Video streams of high dynamic range video that utilize the �Camera Log S3� SMPTE ST2115
		UNSPECIFIED		// Video streams whose transfer characteristics are not specified
	};

	// Specified in 2110-20:2022 7.3. Default = Narrow.
	enum class SDPRange
	{
		NARROW,
		FULL,
		FULLPROTECT
	};

	// Traffic shaping and delievery timing for compressed 2110 (2110-20:2022 5.3)
	enum class SDP_TP
	{
		NARROW,			// TP = 2110TPN
		NARROWLINEAR,	// TP = 21100TPNL
		WIDE			// TP = 2110TPW
	};

	enum class SDPStandard
	{
		UNCOMPRESSED,
		JPEGXS
	};
}