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
	class SDPParser
	{
	public:
		
		// Constructor
		SDPParser(std::string SDP);

		// Main function to parse/receive the de-serialized SDP
		SDP GetSDP();

	private:

		// Main SDP parsers
		void ParseSessionDescription();
		void ParseTimingDescription();
		void ParseMediaDescriptions();
		void ParseVideoDescription(json video_description_session);
		void ParseAudioDescription(json audio_description_session);
		void ParseDataDescription(json data_description_session);

		// Session/General helpers
		void ParseOrigin();
		void ParseBandwidthInformation();
		void ParseConnectionInformation(SDP::ConnectionInformation* connection_information_ptr, json session);
		void ParseAttributes(SDP::Attributes* attribute_ptr, json session);

		// Video Description helpers
		json m_video_params_session;
		shared_ptr<SDP::VideoDescription> m_video_description;
		void ParseVideoParams();
		void ParseUncompressedVideoParams(); 
		void ParseCompressedVideoParams();
		void SetFramerate(std::string framerate_string);
		void SetColorimetry(std::string colorimetry); 
		void SetPackingMode(std::string packing_mode);
		void SetTCS(std::string tcs);
		void SetRange(std::string range);
		void SetPAR(std::string par);
		void SetTP(std::string TP);
		void SetSampling(std::string sampling);
		void SetStandard(std::string standard);
		bool ValidDepth(int32_t depth);

		// Audio Description helpers
		json m_audio_params_session;
		shared_ptr<SDP::AudioDescription> m_audio_description;
		void ParseAudioParams();

		// Data Description helpers
		json m_data_params_session;
		shared_ptr<SDP::DataDescription> m_data_description;

		// Helper functions
		std::string GetSDPFileString(std::string SDPFilePath);

		// Full SDP as Json
		json m_session; 

		// Deserialized SDP
		SDP m_sdp;	
	};
}