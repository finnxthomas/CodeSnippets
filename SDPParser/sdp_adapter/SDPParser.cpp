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

#include "sdp_adapter.h"

namespace Cf
{
	/// <summary>
	/// SDPParser Constructor: Parse a given SDP into a SDP object.	
	/// </summary>
	SDPParser::SDPParser(std::string SDP)
	{
		// Get SDP Session
		m_session = parse(SDP);

		//	Parse SDP Session file...
		//	An SDP is composed of three parts:
		//		1. The session description
		//		2. One or more time descriptions (sdptransform lib only supports
		//		   one currently) ("t=" and "r=" lines).
		//		3. Zero or more media descriptions (starting with "m=" line).
		ParseSessionDescription();
		ParseTimingDescription();
		ParseMediaDescriptions();
	}


	/// <summary>
	/// GetSDP: Get SDP as deserialized object.
	/// </summary>
	SDP SDPParser::GetSDP()
	{
		return m_sdp;
	}
	

	/// <summary>
	/// GetSDPFileString: Returns SDP file as a usable buffer for SDP parser
	/// </summary>
	std::string SDPParser::GetSDPFileString(std::string SDPFilePath)
	{
		std::ifstream t(SDPFilePath);
		std::stringstream buffer;
		buffer << t.rdbuf();

		return buffer.str();
	}


	/// <summary>
	/// ParseOrigin: Parses one origin object. If origin is not present, throws
	///				 error since it is a required param of the SDP.
	/// </summary>
	void SDPParser::ParseOrigin()
	{
		// Origin Params
		auto iter = m_session.find("origin");
		if (iter != m_session.end())
		{
			json origin_session = m_session["origin"];

			iter = origin_session.find("address");
			if (iter != origin_session.end() && iter->is_string())
			{
				m_sdp.origin.unicast_address = *iter;
			}

			iter = origin_session.find("ipVer");
			if (iter != origin_session.end() && iter->is_number())
			{
				m_sdp.origin.addr_type = *iter;
			}

			iter = origin_session.find("netType");
			if (iter != origin_session.end() && iter->is_string())
			{
				m_sdp.origin.net_type = *iter;
			}

			iter = origin_session.find("sessionId");
			if (iter != origin_session.end() && iter->is_number())
			{
				m_sdp.origin.sess_id = *iter;
			}

			iter = origin_session.find("sessionVersion");
			if (iter != origin_session.end() && iter->is_number())
			{
				m_sdp.origin.sess_version = *iter;
			}

			iter = origin_session.find("username");
			if (iter != origin_session.end() && iter->is_string())
			{
				m_sdp.origin.username = *iter;
			}
		}
		else
		{
			throw std::runtime_error("No origin and session identifier found in SDP. This is a required parameter.");
		}
	}


	/// <summary>
	/// ParseConnectionInformation: Given session containing connection
	///								information, parses one 
	///								ConnectionInformation object from SDP into
	///							    given pointer.
	/// </summary>
	void SDPParser::ParseConnectionInformation(SDP::ConnectionInformation* connection_information_ptr, json session)
	{
		auto iter = session.find("connection");
		if (iter != session.end())
		{
			json connection_session = session["connection"];

			iter = connection_session.find("ip");
			if (iter != connection_session.end() && iter->is_string())
			{
				connection_information_ptr->connection_address = *iter;
			}

			iter = connection_session.find("version");
			if (iter != connection_session.end() && iter->is_number())
			{
				connection_information_ptr->addr_type = *iter;
			}

			iter = connection_session.find("ttl");
			if (iter != connection_session.end() && iter->is_number())
			{
				connection_information_ptr->ttl = *iter;
			}
		}
	}


	/// <summary>
	/// ParseBandwidthInformation: Parses all bandwidth informations in an SDP.
	/// </summary>
	void SDPParser::ParseBandwidthInformation()
	{
		auto iter = m_session.find("bandwidth");
		if (iter != m_session.end())
		{
			json bandwidth_session = m_session.at("bandwidth");
			for (int i = 0; i < bandwidth_session.size(); i++)
			{
				SDP::BandwidthInformation bandwidth_information;

				auto iter = bandwidth_session[i].find("type");
				if (iter != bandwidth_session[i].end() && iter->is_string())
				{
					bandwidth_information.type = *iter;
				}

				iter = bandwidth_session[i].find("limit");
				if (iter != bandwidth_session[i].end() && iter->is_number())
				{
					bandwidth_information.limit = *iter;
				}

				m_sdp.bandwidth_informations.push_back(bandwidth_information);
			}
		}
	}


	/// <summary>
	/// ParseAttributes: Parses different types of attributes in given session 
	///					 portion of the SDP into given pointer. Work in
	///					 progress. See sdptransform/ grammar.cpp for all "a=" 
	///					 scenarios.
	/// </summary>
	void SDPParser::ParseAttributes(SDP::Attributes* attribute_ptr, json session)
	{
		// Parse mediaclk attribute (example: a=mediaclk:direct=0)
		auto iter = session.find("mediaclk");
		if (iter != session.end() && iter->is_string())
		{
			attribute_ptr->media_clock = *iter;
		}

		// Parse framerate attribute (example: a=framerate:29.97)
		iter = session.find("framerate");
		if (iter != session.end() && iter->is_number())
		{
			attribute_ptr->framerate = *iter;
		}

		// Parse rtpmap attribute (example: a=rtpmap:110 opus/48000/2)
		if (session.find("rtp") != session.end())
		{
			json rtp_session = session["rtp"];
			for (int i = 0; i < rtp_session.size(); i++)
			{
				SDP::Attributes::RTP rtp;

				iter = rtp_session[i].find("payload");
				if (iter != rtp_session[i].end() && iter->is_number())
				{
					rtp.payload = *iter;
				}

				iter = rtp_session[i].find("codec");
				if (iter != rtp_session[i].end() && iter->is_string())
				{
					rtp.codec = *iter;
				}

				iter = rtp_session[i].find("rate");
				if (iter != rtp_session[i].end() && iter->is_number())
				{
					rtp.rate = *iter;
				}

				iter = rtp_session[i].find("encoding");
				if (iter != rtp_session[i].end() && iter->is_string())
				{
					rtp.encoding = *iter;
				}

				attribute_ptr->rtp_map.push_back(rtp);
			}
		}

		// Parse fmtp attirbute (example: a=fmtp:108 profile-level-id=24;object=23;bitrate=64000)
		if (session.find("fmtp") != session.end())
		{
			json fmtp_session = session["fmtp"];

			for (int i = 0; i < fmtp_session.size(); i++)
			{
				SDP::Attributes::FMTP fmtp;

				iter = fmtp_session[i].find("payload");
				if (iter != fmtp_session[i].end() && iter->is_number())
				{
					fmtp.payload = *iter;
				}

				iter = fmtp_session[i].find("config");
				if (iter != fmtp_session[i].end() && iter->is_string())
				{
					fmtp.config = *iter;
				}

				attribute_ptr->fmtp.push_back(fmtp);
			}
		}

		// Parse source-filter attribute (example: a=source-filter: incl IN IP4 239.5.2.31 10.1.15.5)
		if (session.find("sourceFilter") != session.end())
		{
			json source_filter_session = session["sourceFilter"];

			iter = source_filter_session.find("filterMode");
			if (iter != source_filter_session.end() && iter->is_string())
			{
				attribute_ptr->source_filter.filter_mode = *iter;
			}

			iter = source_filter_session.find("netType");
			if (iter != source_filter_session.end() && iter->is_string())
			{
				attribute_ptr->source_filter.net_type = *iter;
			}

			iter = source_filter_session.find("addressTypes");
			if (iter != source_filter_session.end() && iter->is_string())
			{
				attribute_ptr->source_filter.address_types = *iter;
			}

			iter = source_filter_session.find("destAddress");
			if (iter != source_filter_session.end() && iter->is_string())
			{
				attribute_ptr->source_filter.dest_address = *iter;
			}

			iter = source_filter_session.find("srcList");
			if (iter != source_filter_session.end() && iter->is_string())
			{
				attribute_ptr->source_filter.src_list = *iter;
			}
		}

		// Parse imageattr attribute (example: a=imageattr:97 send [x=800,y=640,sar=1.1,q=0.6] [x=480,y=320] recv [x=330,y=250])
		if (session.find("imageattrs") != session.end())
		{
			json image_attributes_session = session["imageattrs"];

			for (int i = 0; i < image_attributes_session.size(); i++)
			{
				SDP::Attributes::ImageAttributes image_attributes;

				iter = image_attributes_session[i].find("pt");
				if (iter != image_attributes_session[i].end() && iter->is_string())
				{
					image_attributes.pt = *iter;
				}

				iter = image_attributes_session[i].find("dir1");
				if (iter != image_attributes_session[i].end() && iter->is_string())
				{
					image_attributes.dir1 = *iter;
				}

				iter = image_attributes_session[i].find("attrs1");
				if (iter != image_attributes_session[i].end() && iter->is_string())
				{
					image_attributes.attrs1 = *iter;
				}

				iter = image_attributes_session[i].find("dir2");
				if (iter != image_attributes_session[i].end() && iter->is_string())
				{
					image_attributes.dir2 = *iter;
				}

				iter = image_attributes_session[i].find("attrs2");
				if (iter != image_attributes_session[i].end() && iter->is_string())
				{
					image_attributes.attrs2 = *iter;
				}

				attribute_ptr->image_attributes.push_back(image_attributes);
			}
		}

		// Attribute is unknown - print alert message
		if (m_session.find("invalid") != m_session.end())
		{
			json invalid_attributes = m_session["invalid"];
			for (int i = 0; i < invalid_attributes.size(); i++)
			{
				PLOG_INFO << "Unknown attribute \"" << (std::string)invalid_attributes[i]["value"] << "\" found in the SDP.";
			}
		}
	}


	/// <summary>
	/// ParseSessionDescription: Parses the session desc portion of the SDP. 
	///							 This includes everything but the  
	///							 TimingDescriptions("t=", "r=", "z=") and 
	///							 MediaDescriptions ("m=" and following lines).
	/// </summary>
	void SDPParser::ParseSessionDescription()
	{
		// REDACTED // 
	}


	/// <summary>
	/// ParseTimingDescription:	Parses a timing description present in the
	///							SDP. SDP can contain more than one timing
	///						    description, however the sdptransform lib does
	///							not currently suppport that. If this becomes
	///							relevant, the sdptransform lib can be updated.
	/// </summary>
	void SDPParser::ParseTimingDescription()
	{
		auto iter = m_session.find("timing");
		if (iter != m_session.end())
		{
			json timing_session = m_session["timing"];

			auto iter = timing_session.find("start");
			if (iter != timing_session.end() && iter->is_number())
			{
				m_sdp.time_description.time_active.start_time = *iter;
			}

			iter = timing_session.find("stop");
			if (iter != timing_session.end() && iter->is_number())
			{
				m_sdp.time_description.time_active.start_time = *iter;
			}
		}
		else
		{
			throw std::runtime_error("No time description found in SDP. This is a required parameter.");
		}
	}


	/// <summary>
	/// ParseVideoDescription: Parses one video description from an SDP.
	/// </summary>
	void SDPParser::ParseVideoDescription(json video_description_session)
	{
		m_video_description = make_shared<SDP::VideoDescription>(video_description_session);

		auto iter = video_description_session.find("payloads");
		if (iter != video_description_session.end() && iter->is_string())
		{
			m_video_description->payloads = *iter;
		}

		iter = video_description_session.find("port");
		if (iter != video_description_session.end() && iter->is_number())
		{
			m_video_description->port = *iter;
		}

		iter = video_description_session.find("protocol");
		if (iter != video_description_session.end() && iter->is_string())
		{
			m_video_description->protocol = *iter;
		}

		ParseConnectionInformation(&m_video_description->connection_information, video_description_session);

		ParseAttributes(&m_video_description->attributes, video_description_session);

		m_video_params_session = parseParams(m_video_description->attributes.fmtp[0].config);
		ParseVideoParams();

		m_sdp.media_descriptions.push_back(m_video_description);
	}


	/// <summary>
	/// ParseAudioDescription: Parses one audio description from an SDP.
	/// </summary>
	void SDPParser::ParseAudioDescription(json audio_description_session)
	{
		m_audio_description = make_shared<SDP::AudioDescription>(audio_description_session);

		auto iter = audio_description_session.find("payloads");
		if (iter != audio_description_session.end() && iter->is_string())
		{
			m_audio_description->payloads = *iter;
		}

		iter = audio_description_session.find("port");
		if (iter != audio_description_session.end() && iter->is_number())
		{
			m_audio_description->port = *iter;
		}

		iter = audio_description_session.find("protocol");
		if (iter != audio_description_session.end() && iter->is_string())
		{
			m_audio_description->protocol = *iter;
		}

		ParseAttributes(&m_audio_description->attributes, audio_description_session);

		m_audio_params_session = parseParams(m_audio_description->attributes.fmtp[0].config);
		ParseAudioParams();

		m_sdp.media_descriptions.push_back(m_audio_description);
	}


	/// <summary>
	/// ParseDataDescription: Parses one data description from an SDP.
	/// </summary>
	void SDPParser::ParseDataDescription(json data_description_session)
	{

	}


	/// <summary>
	/// ParseMediaDescriptions: Parses media descriptions as described in the
	///							SDP.
	/// </summary>
	void SDPParser::ParseMediaDescriptions()
	{
		for (int i = 0; i < m_session.at("media").size(); i++)
		{
			json media_description_session = m_session.at("media")[i];

			if ((media_description_session.at("type")) == "video")
			{
				ParseVideoDescription(media_description_session);
			}
			else if ((media_description_session.at("type")) == "audio")
			{
				ParseAudioDescription(media_description_session);
			}
			else
			{
				PLOG_INFO << "SDP Parsing for " <<
					(char*)m_session.at("media")[i].type() << " is not supported yet";
			}
		}
	}


	/// <summary>
	/// ParseAudioParams: Parse audio parameters (channel order) from SDP.
	///					  More params may need to be added if they come up,
	///					  but this was all that was described in 2110-30.
	/// </summary>
	void SDPParser::ParseAudioParams()
	{
		auto iter = m_audio_params_session.find("channel-order");
		if (iter != m_audio_params_session.end() && iter->is_string())
		{
			std::string channel_order = *iter;

			// Regex Pattern for Channel order string. Example: "SMPTE2110.(M,M,M,M,ST,U02)"
			std::regex pattern("SMPTE2110.\\((.+)\\)");
			std::smatch matches;

			// regex_search produces:
			// matches[0] = the full match, matches[1] = group 1
			if (std::regex_search(channel_order, matches, pattern))
			{
				m_audio_description->channel_order = matches[1];
			}
			else
			{
				throw std::runtime_error("Channel order from SDP File is of unknown format. See 2110-30:2017 6.2.2 for details");
			}
		}
		
	}


	/// <summary>
	/// ParseVideoParams: Parse video parameters from the SDP.
	/// </summary>
	void SDPParser::ParseVideoParams()
	{
		// REDACTED // 
	}


	/// <summary>
	/// ParseUncompressedVideoParams: Uncompressed/Compressed 2110 video streams
	///								  have different SDP requirements. This
	///								  function ensures that all necessary video
	///								  metadata for uncompressed 2110 are
	///								  present and parsed. 
	/// </summary>
	void SDPParser::ParseUncompressedVideoParams()
	{
		///
		/// Required Media Type Parameters (ST2110-20 7.2)
		///

		// Framerate = Signals the frame rate in frames per second. 
		// Can be a single decimal number (e.g. "25"), or a ratio of two int decimal numbers seperated by forward slash (e.g. "20000/1001)
		auto iter = m_video_params_session.find("exactframerate");
		if (iter != m_video_params_session.end())
		{
			if (iter->is_number()) // For case: exactframerate=25
			{
				m_video_description->framerate_num = *iter;
				m_video_description->framerate_den = 1;
			}
			else if (iter->is_string()) // For case: exactframerate=30000/1001
			{
				SetFramerate(*iter);
			}
		}
		else
			throw std::runtime_error("No exactframerate found in SDP. This is a required media type parameter.");

		// Depth = Signals the number of bits per sample. Can be 8, 10, 12, 16, or 16f.
		iter = m_video_params_session.find("depth");
		if (iter != m_video_params_session.end())
		{
			if (iter->is_string())
			{
				if (*iter == "16f")
					m_video_description->depth = SDPDepth::FLOAT_16;
				else
					throw std::runtime_error("Depth value from SDP is of unsupported value");
			}
			else if (iter->is_number())
			{
				if (ValidDepth(*iter))
					m_video_description->depth = *iter;
			}
		}
		else
		{
			throw std::runtime_error("No depth found in SDP. This is a required media type parameter.");
		}

		// Colorimetry: Specifies the system colorimetry used by the image samples.
		// Permitted values found in SDPColorimetry enumueration.
		iter = m_video_params_session.find("colorimetry");
		if (iter != m_video_params_session.end() && iter->is_string())
			SetColorimetry(*iter);
		else
			throw std::runtime_error("No colorimetry found in SDP. This is a required media type parameter.");

		// Packing Mode
		// Permitted values found in SDPPackingMode enumeration.
		iter = m_video_params_session.find("PM");
		if (iter != m_video_params_session.end() && iter->is_string())
			SetPackingMode(*iter);
		else
			throw std::runtime_error("No PM (packing mode) found in SDP. This is a required media type parameter.");

		// Sampling = Signals the color difference signal sub-sampling structure
		iter = m_video_params_session.find("sampling");
		if (iter != m_video_params_session.end() && iter->is_string())
			SetSampling(*iter);
		else
			throw std::runtime_error("No sampling found in SDP. This is a required media type parameter.");
	}


	/// <summary>
	/// ParseCompressedVideoParams: Uncompressed/Compressed 2110 video streams
	///								have different SDP requirements. This
	///								function ensures that all necessary video
	///								metadata for compressed 2110 are present
	///								and parsed. 
	/// </summary>
	void SDPParser::ParseCompressedVideoParams()
	{
		/// Required Parameters

		// Framerate = Signals the frame rate in frames per second. 
		// For compresseed video, it can be found as an attribute isntead of in the media type parameters.
		auto iter = m_session.find("framerate");
		if (iter == m_session.end())
		{
			iter = m_video_params_session.find("exactframerate");
			if (iter == m_video_params_session.end())
				throw std::runtime_error("No framerate found in SDP. This is a required media type parameter.");
		}

		// Framerate value found in either attribute or media type parameter location
		if (iter->is_number()) // For case: framerate=25 or exactframerate=25
		{
			m_video_description->framerate_num = *iter;
			m_video_description->framerate_den = 1;
		}
		else if (iter->is_string()) // For case: framerate=30000.1001 or exactframerate = 30000/1001
		{
			SetFramerate(*iter);
		}

		// TP (traffic and delievery)
		iter = m_video_params_session.find("TP");
		if (iter != m_video_params_session.end() && iter->is_string())
			SetTP(*iter);
		else
			throw std::runtime_error("No TP found in SDP. This is a required media type parameter.");


		/// Optional? Parameters in Compressed Video. Spec doesn't say otherwise, but they can arise..

		iter = m_video_params_session.find("CMAX");
		if (iter != m_video_params_session.end() && iter->is_number())
			m_video_description->cmax = *iter;

		// Depth = Signals the number of bits per sample. Can be 8, 10, 12, 16, or 16f.
		iter = m_video_params_session.find("depth");
		if (iter != m_video_params_session.end())
		{
			if (iter->is_string())
			{
				if (*iter == "16f")
					m_video_description->depth = SDPDepth::FLOAT_16;
				else
					throw std::runtime_error("Depth value from SDP is of unsupported value");
			}
			else if (iter->is_number())
			{
				if (ValidDepth(*iter))
					m_video_description->depth = *iter;
			}
		}

		// Colorimetry: Specifies the system colorimetry used by the image samples.
		// Permitted values found in SDPColorimetry enumueration.
		iter = m_video_params_session.find("colorimetry");
		if (iter != m_video_params_session.end() && iter->is_string())
			SetColorimetry(*iter);

		// Packing Mode
		// Permitted values found in SDPPackingMode enumeration.
		iter = m_video_params_session.find("PM");
		if (iter != m_video_params_session.end() && iter->is_string())
			SetPackingMode(*iter);

		// Sampling = Signals the color difference signal sub-sampling structure
		iter = m_video_params_session.find("sampling");
		if (iter != m_video_params_session.end() && iter->is_string())
			SetSampling(*iter);
	}


	/// <summary>
	/// SetFramerate: Finds and sets framerate numerator and denominator from 
	///				  the SDP in the deserialized video_description.
	/// </summary>
	void SDPParser::SetFramerate(std::string framerate_string)
	{
		// Regex Pattern for Framerate string. 
		// Examples: "30000.1001" or "30000/1001"
		std::regex pattern1("(\\d+)\\.(\\d+)");
		std::regex pattern2("(\\d+)\\/(\\d+)");
		std::smatch matches;

		// regex_search produces:
		// matches[0] = the full match, matches[1] = group 1, matches[2] = group 2
		if (std::regex_search(framerate_string, matches, pattern1))
		{
			m_video_description->framerate_num = stoi(matches[1]);
			m_video_description->framerate_den = stoi(matches[2]);
			return;
		}
		else if (std::regex_search(framerate_string, matches, pattern2))
		{
			m_video_description->framerate_num = stoi(matches[1]);
			m_video_description->framerate_den = stoi(matches[2]);
			return;
		}
		else
		{
			throw std::runtime_error("Framerate from SDP File is of unknown format. See 2110-20:2022 7.2 or 2110-22:2022 7.4 for valid format descriptions.");
		}
	}


	/// <summary>
	/// SetColorimetry: Sets colorimetry from the SDP as an enum in the
	///					deserialized video_description.
	/// </summary>
	void SDPParser::SetColorimetry(std::string colorimetry)
	{
		if (colorimetry == "BT601")
			m_video_description->colorimetry = SDPColorimetry::BT601;
		else if (colorimetry == "BT709")
			m_video_description->colorimetry = SDPColorimetry::BT709;
		else if (colorimetry == "BT2020")
			m_video_description->colorimetry = SDPColorimetry::BT2020;
		else if (colorimetry == "BT2100")
			m_video_description->colorimetry = SDPColorimetry::BT2100;
		else if (colorimetry == "ST2065-1")
			m_video_description->colorimetry = SDPColorimetry::ST2065_1;
		else if (colorimetry == "ST2065-3")
			m_video_description->colorimetry = SDPColorimetry::ST2065_3;
		else if (colorimetry == "UNSPECIFIED")
			m_video_description->colorimetry = SDPColorimetry::UNSPECIFIED;
		else if (colorimetry == "XYZ")
			m_video_description->colorimetry = SDPColorimetry::XYZ;
		else if (colorimetry == "ALPHA")
			m_video_description->colorimetry = SDPColorimetry::ALPHA;
		else
			throw std::runtime_error("Colorimetry type from SDP File is of unsupported type. See 2110-20:2022 7.5 for valid types");
	}


	/// <summary>
	/// SetPackingMode: Sets packing mode from the SDP as an enum in the
	///					deserialized video_description.
	/// </summary>
	void SDPParser::SetPackingMode(std::string packing_mode)
	{
		if (packing_mode == "2110GPM")
			m_video_description->packing_mode = SDPPackingMode::GPM;
		else if (packing_mode == "2110BPM")
			m_video_description->packing_mode = SDPPackingMode::BPM;
	}


	/// <summary>
	/// SetTCS: Sets TCS (Transfer characteristic system) from the SDP as an 
	///		    enum in the deserialized video_description.
	/// </summary>
	void SDPParser::SetTCS(std::string tcs)
	{
		if (tcs == "SDR")
			m_video_description->tcs = SDPTransferCharacteristicSystem::SDR;
		else if (tcs == "PQ")
			m_video_description->tcs = SDPTransferCharacteristicSystem::PQ;
		else if (tcs == "HLG")
			m_video_description->tcs = SDPTransferCharacteristicSystem::HLG;
		else if (tcs == "LINEAR")
			m_video_description->tcs = SDPTransferCharacteristicSystem::LINEAR;
		else if (tcs == "BT2100LINPQ")
			m_video_description->tcs = SDPTransferCharacteristicSystem::BT2100LINPQ;
		else if (tcs == "BT2100LINHLG")
			m_video_description->tcs = SDPTransferCharacteristicSystem::BT2100LINHLG;
		else if (tcs == "ST2065-1")
			m_video_description->tcs = SDPTransferCharacteristicSystem::ST2065_1;
		else if (tcs == "ST428-1")
			m_video_description->tcs = SDPTransferCharacteristicSystem::ST428_1;
		else if (tcs == "DENSITY")
			m_video_description->tcs = SDPTransferCharacteristicSystem::DENSITY;
		else if (tcs == "ST2115LOGS3")
			m_video_description->tcs = SDPTransferCharacteristicSystem::ST2115LOGS3;
		else if (tcs == "UNSPECIFIED")
			m_video_description->tcs = SDPTransferCharacteristicSystem::UNSPECIFIED;
		else
			throw std::runtime_error("TCS from SDP File is of unsupported type. See 2110-20:2022 7.6 for valid types");

	}


	/// <summary>
	/// SetRange: Sets range from the SDP as an enum in the deserialized 
	///			  video_description.
	/// </summary>
	void SDPParser::SetRange(std::string range)
	{
		if (range == "NARROW")
			m_video_description->range = SDPRange::NARROW;
		else if (range == "FULLPROTECT")
			m_video_description->range = SDPRange::FULLPROTECT;
		else if (range == "FULL")
			m_video_description->range = SDPRange::FULL;
		else
			throw std::runtime_error("RANGE type from SDP file is of unsupported type. See 2110-20:2022 7.3 for valid types");

		if (m_video_description->colorimetry == SDPColorimetry::BT2100)
		{
			if (m_video_description->range != SDPRange::NARROW && m_video_description->range != SDPRange::FULL)
				throw std::runtime_error("In the SDP, if colorimetry is BT2100, then RANGE must be NARROW or FULL");
		}
	}


	/// <summary>
	/// SetPAR: Finds and sets PAR (pixel aspect ratio) from the SDP in the
	///			deserialized video_description.
	/// </summary>
	void SDPParser::SetPAR(std::string par)
	{
		// Regex Pattern for pixel aspect ratio string. Example: "1:1"
		std::regex pattern("(\\d+):(\\d+)");
		std::smatch matches;

		// regex_search produces:
		// matches[0] = the full match, matches[1] = group 1, matches[2] = group 2
		if (std::regex_search(par, matches, pattern))
		{
			m_video_description->par_width = stoi(matches[1]);
			m_video_description->par_height = stoi(matches[2]);
		}
		else
		{
			throw std::runtime_error("PAR (Pixel Aspect Ratio) from SDP File is of unknown format. See 2110-20:2022 7.3 for valid format description.");
		}
	}


	/// <summary>
	/// SetTP: Sets TP (traffic shape) from the SDP as an enum in the
	///		   deserialized video_description.
	/// </summary>
	void SDPParser::SetTP(std::string TP)
	{
		if (TP == "2110TPN")
			m_video_description->tp = SDP_TP::NARROW;
		else if (TP == "2110TPNL")
			m_video_description->tp = SDP_TP::NARROWLINEAR;
		else if (TP == "2110TPW")
			m_video_description->tp = SDP_TP::WIDE;
		else
			throw std::runtime_error("TP from SDP File is of unknown type. See 2110-22:2022 5.3 for valid types.");

	}


	/// <summary>
	/// SetSampling: Sets sampling from the SDP as an enum in the deserialized
	///			     video_description.
	/// </summary>
	void SDPParser::SetSampling(std::string sampling)
	{
		if (sampling == "YCbCr-4:4:4")
			m_video_description->sampling = SDPSampling::YCBCR_444;
		else if (sampling == "YCbCr-4:2:2")
			m_video_description->sampling = SDPSampling::YCBCR_422;
		else if (sampling == "YCbCr-4:2:0")
			m_video_description->sampling = SDPSampling::YCBCR_420;
		else if (sampling == "CLYCbCr-4:4:4")
			m_video_description->sampling = SDPSampling::CLYCBCR_444;
		else if (sampling == "CLYCbCr-4:2:2")
			m_video_description->sampling = SDPSampling::CLYCBCR_422;
		else if (sampling == "CLYCbCr-4:2:0")
			m_video_description->sampling = SDPSampling::CLYCBCR_420;
		else if (sampling == "ICtCp-4:4:4")
			m_video_description->sampling = SDPSampling::ICTCP_444;
		else if (sampling == "ICtCp-4:2:2")
			m_video_description->sampling = SDPSampling::ICTCP_422;
		else if (sampling == "ICtCp-4:2:0")
			m_video_description->sampling = SDPSampling::ICTCP_420;
		else if (sampling == "RGB")
			m_video_description->sampling = SDPSampling::RGB;
		else if (sampling == "XYZ")
			m_video_description->sampling = SDPSampling::XYZ;
		else if (sampling == "KEY")
		{
			m_video_description->sampling = SDPSampling::KEY;
			PLOG_INFO << "KEY sampling from SDP not implemented.";
			throw std::runtime_error("KEY sampling declared in SDP is not supported");
		}
		else
		{
			throw std::runtime_error("Sampling from SDP File is of unknown format. See 2110-20:2022 7.3 for valid format description.");
		}
	}

	
	/// <summary>
	/// SetStandard: Finds and sets 2110 standard (jpegxs or uncompresesd) from
	///				 the SDP in the deserialized video_description.
	/// </summary>
	void SDPParser::SetStandard(std::string standard)
	{
		// Param not required for compressed video SDPs.
		if (standard == "")
		{
			m_video_description->standard = SDPStandard::JPEGXS;
			return;
		}

		std::regex pattern("ST2110-(\\d+):\\d+");
		std::smatch matches;
		if (std::regex_search(standard, matches, pattern))
		{
			if (matches[1].str() == "22")
			{
				m_video_description->standard = SDPStandard::JPEGXS;
				return;
			}
			else if (matches[1].str() == "20")
			{
				m_video_description->standard = SDPStandard::UNCOMPRESSED;
				return;
			}
		}

		throw std::runtime_error("SMPTE Standard Number (SSN) \"" + standard + "\" was not recognized");
	}


	/// <summary>
	/// ValidDepth: Returns true if depth is valid for a 2110 stream.
	/// </summary>
	bool SDPParser::ValidDepth(int32_t depth)
	{
		switch ((SDPDepth)depth)
		{
		case SDPDepth::INT_8:
			break;
		case SDPDepth::INT_10:
			break;
		case SDPDepth::INT_12:
			break;
		case SDPDepth::INT_16:
			break;
		default:
			throw std::runtime_error("Depth value from SDP is of unsupported value. See 2110-20:2022 7.4.2 for valid values");
		}

		return true;
	}
}