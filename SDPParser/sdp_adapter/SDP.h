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
	/// <summary>
	/// SDP: Class to describe one Session Description Protocol. See RFC
	///		 8866 Section 5 for full description.
	/// </summary>
	class SDP
	{
	public:
		/// <summary>
		/// Origin: Struct describing one origin ("o=") section of the SDP. See
		///			RFC 8866 Section 5.2 for full description.
		/// </summary>
		struct Origin
		{
		public:

			// <username>
			// The user's login on the originating host. Can be "-" if the host 
			// does not have the concept of IDs. Must NOT contain spaces.
			std::string username;

			// <sess-id>
			// A numeric string such that the tuple of <username>, <sess-id>, 
			// <nettype>, <addrtype>, and <unicast-address> forms a globally 
			// unique identifier for the session.
			int32_t sess_id;

			// <sess-version>
			// Version number for this session description. 
			int32_t sess_version;

			// <nettype>
			// Describes the type of network. Usually "IN" for Internet. 
			std::string net_type;

			// <addrtype>
			// Describes the type of address such as "IP4" or "IP6". Will be
			// set as and int.
			int32_t addr_type;

			// <unicast-address>
			// The address of the machine from which the session was created.
			std::string unicast_address;
		};


		/// <summary>
		/// ConnectionInformation: Struct describing one connection information
		///						   ("c=") which contains information necessary
		///						   to establish a network connection.
		/// </summary>
		struct ConnectionInformation
		{
		public:

			// <addrtype>
			// Describes the type of address such as "IP4" or "IP6". Set as int
			// like 4 or 6.
			int32_t addr_type;

			// <connection-address>
			// Describes the connection address. Additional subfields MAY be
			// added after the connection address depending on the value of the 
			// <addrtype> subfield. See RFC 8866 5.7 for full description.
			std::string connection_address = "";


			// Time to live
			// The TTL for the session is appended to the address using a slash 
			// as a separator. An example is: c = IN IP4 233.252.0.1/127
			int32_t ttl;
		};


		/// <summary>
		/// BandwidthInformation: Struct describing the bandwidth information
		///						  ("b="). Contains the type and number.
		/// </summary>
		struct BandwidthInformation
		{
		public:

			// <bwtype>
			// The alphanumberic modifer that provides the meaning of the 
			// bandwidth number.
			// "CT" = Conference total. Bandwidth of the session is different 
			//		  from the bandwidth implicit from the scope. Gives the 
			//		  proposed upper limit to the bandwidth. Mux category is 
			//		  NORMAL. More information in RFC 8866 5.8.
			// "AS" = Application specific. The application's concept of the
			//		  maximum bandwidth. Mux category is SUM. 
			std::string type;

			// <bandwidth>
			// The bandwidth limit
			int32_t limit; 
		};


		/// <summary>
		/// TimeDescription: Class describing the mandatory time description
		///					 that is included in every SDP.
		/// </summary>
		class TimeDescription
		{
		private:

			/// <summary>
			/// TimeActive: Struct describing one time active ("t="). Contains
			///				start and stop times for a session. More
			///				information in RFC 8866 5.9.
			/// </summary>
			struct TimeActive
			{
			public:
				int32_t start_time;
				int32_t stop_time;
			};


			/// <summary>
			/// RepeatTimes: Struct describing one repeat times ("r=") for
			///				 a session. More information in RFC 8866 5.10
			/// <para>
			/// ** Not supported correctly by sdptransform lib. If this becomes
			///	   relevant later, sdptransform lib can be updated. </para>
			/// </summary>
			struct RepeatTimes
			{
			public:
				int32_t repeat_interval;
				int32_t active_duration;
				std::vector<int32_t> offsets_from_start_time;
			};


			/// <summary>
			/// TimeZoneAdjustment: Struct describing time zone adjustments for
			///						daylight savings ("z=").
			/// <para>
			/// ** Not supported correctly by sdptransform lib. If this becomes 
			///	   relevant later, sdptransform lib can be updated. </para>
			/// </summary>
			struct TimeZoneAdjustment
			{
			public:
				int32_t adjustment_time;
				std::string offset;
			};

		public:

			// TimeActive ("t="). REQUIRED.
			// Begins a time description that specifies the start and stop 
			// times for a session.
			// t = <start-time> <stop-time>
			TimeActive time_active;

			// Repeat Times ("r=") OPTIONAL.
			// Specifies repeat times for a session.
			// r=<repeat interval> <active duration> <offsets from start-time>
			// ** Not supported correctly by sdptransform lib yet Keep unset.
			///RepeatTimes repeat_times;


			// Time Zone Adjustment ("z=") OPTIONAL.
			// List of offsets for different time zones in the case of
			// daylight savings. 
			// z=[<adjustment time> <offset>] [<adjustment time> <offset>] ...
			// ** Not supported correctly by sdptransform lib yet Keep unset.
			///std::vector<TimeZoneAdjustment> time_zone_adjustment;

		};


		/// <summary>
		/// Attribute: Class describing attributes ("a="), the primary means
		///			   for extending SDP. May be defined to be used as
		///			   session-level, media-level, or both. The sdptransform
		///			   lib provides special parsing for each type of attribute.
		///			   They are contained in this class.
		/// <para>
		///	Attribute-fields may be of two forms: </para>
		/// <para>
		///		1. Property attribute "a=attribute-name". These are binary
		///		attributes, and the presence of the attribute conveys that the 
		///		attribute is a property of the session. An example might be 
		///		"a=recvonly". </para>
		/// <para>
		///		2. Value attribute "a=attribute-name:attribute-value". For 
		///		example, a whiteboard could have the value attribute 
		///		"a=orient:landscape". </para> 
		/// 
		/// ** Uncompleted class, only need to parse relevant attributes.
		/// </summary>
		class Attributes
		{
		public:

			// example: a=rtpmap:110 opus/48000/2
			struct RTP
			{
				int32_t payload;
				std::string codec;
				int32_t rate;
				std::string encoding;
			};
			std::vector<RTP> rtp_map;

			// example: a=fmtp:108 profile-level-id=24;object=23;bitrate=64000
			struct FMTP
			{
				int32_t payload;
				std::string config;
			};
			std::vector<FMTP> fmtp;

			// example: a=source-filter: incl IN IP4 239.5.2.31 10.1.15.5
			struct SourceFilter
			{
				std::string filter_mode;
				std::string net_type;
				std::string address_types;
				std::string dest_address;
				std::string src_list;
			};
			SourceFilter source_filter;

			// examples:
			// a=imageattr:97 send [x=800,y=640,sar=1.1,q=0.6] [x=480,y=320] recv [x=330,y=250]
			// a=imageattr:* send [x=800,y=640] recv *
			// a=imageattr:100 recv [x=320,y=240]
			struct ImageAttributes
			{
				std::string pt;
				std::string dir1;
				std::string attrs1;
				std::string dir2;
				std::string attrs2;
			};
			std::vector<ImageAttributes> image_attributes;

			// example: a=mediaclk:direct=0
			std::string media_clock;

			// example: a=framerate:29.97
			float framerate;
		};


		/// <summary>
		/// MediaSource: Describes a parent class for a media description of
		///				 an SDP. Childs include Video, Audio, and Data.
		/// </summary>
		class MediaDescription
		{
		public:
			// make class polymorphic so that dynamic_pointer_cast<>() works
			virtual ~MediaDescription() = default;

			json m_session; // Media Source Session
			SDPMediaType m_type;

			/// Top Level Params for all Media Descriptions
			std::string payloads;
			int32_t port = 0;
			std::string protocol;
			Attributes attributes;
		};


		/// <summary>
		/// VideoSource: Class to encapsulate a video media description from an 
		///				 SDP. 
		/// </summary>
		class VideoDescription : public MediaDescription
		{
		private:
			json m_video_params_session;

		public:
			VideoDescription(json video_description)
			{
				m_session = video_description;
				m_type = SDPMediaType::VIDEO;
			}

			/// Top Level Params
			ConnectionInformation connection_information;

			/// Video Params
			int32_t width;
			int32_t height;

			// In SDP, specified as "SSN". Mandatory for uncompressed. Not
			// mandatory for compressed.
			SDPStandard standard;

			// For uncompressed video, specified as a format specific parameter 
			// in SDP as "exactframerate".
			// For compresed video, can be described like uncompressed, or as 
			// an attribute "framerate".
			int32_t framerate_num;
			int32_t framerate_den;

			/// COMPRESSED
			// Mandatory Format Specific Parameters
			SDP_TP tp;
			// Optional Format Specific Parameters
			int32_t cmax;

			/// UNCOMPRESSED
			// Required Media Type Parameters
			SDPSampling sampling;
			SDPDepth depth;
			SDPColorimetry colorimetry;
			SDPPackingMode packing_mode; // In SDP, specified as "PM"

			// Media Type Parameters with Default Values
			bool interlaced = false; // true = interlaced, false = progressive
			bool segmented = false; // true = Progressive segmented frame (PsF). 

			// Defaults as set
			SDPTransferCharacteristicSystem tcs = SDPTransferCharacteristicSystem::SDR;
			SDPRange range = SDPRange::NARROW;

			// The Standard UDP Size Limit shall be 1460 octets 
			// (2110-10:2022 6.3)
			int32_t max_udp =  1460; 

			// In SDP, described under "PAR" (e.g width:height, like "1:1")
			int32_t par_width = 1; 
			int32_t par_height = 1;
		};


		/// <summary>
		/// VideoSource: Class to encapsulate an audio media description from
		///				 an SDP. 
		/// </summary>
		class AudioDescription: public MediaDescription
		{
		public:
			AudioDescription(json audio_description)
			{
				m_session = audio_description;
				m_type = SDPMediaType::AUDIO;
			}

			// In the SMPTE2110 channel-order convention, the <order> shall be 
			// a listing of Channel Grouping Symbols contained within 
			// parenthesis and separated by commas. 
			// Ex: "channel-order=SMPTE2110.(M,M,M,M,ST,U02)"
			//
			// Channel Order Convetion Grouping Symbols 
			// (from SMPTE ST 2110-30:2017 Table 1)
			// 
			// M (Mono), 1 channel. Order: Mono
			// DM (Dual Mono), 2 channels. Order: M1, M2
			// ST (Standard Stereo, 2 channels. Order: Left, Right
			// LtRt (Matrix Stereo), 2 channels. Order: Left Total, Right Total
			// 51 (5.1 Surround), 6 channels. Order: L, R, C, LFE, Ls, Rs
			// 71 (7.1 Surround), 8 channels. Order: L, R, C, LFE, Lss, Rss, Lrs, Rrs
			// 222 (22.2 Surround), 24 channels. Order: see SMPTE ST 2036-2, Table 1
			// SGRP (One SDI audio group), 4 channels. Order: 1, 2, 3, 4
			// U01-U64 (Undefined), channel indicated by grouping symbol nn. No order specified.
			std::string channel_order;

		};

		class DataDescription : public MediaDescription
		{
		private:
			//json m_data_params_session;
		public:
			DataDescription(json data_description)
			{
				m_session = data_description;
				//m_data_params_session = parseParams(m_session.at("fmtp")[0].at("config"));
				m_type = SDPMediaType::DATA;
			}
		};


	public:

		// Protocol Version ("v="). REQUIRED.
		// Gives the version of the SDP.
		int32_t protocol_version;

		// Origin ("o="). REQUIRED.
		// Gives the originator of the session, a session identifier, and a 
		// version number.
		// o = <username> <sess-id> <sess-version> <nettype> <addrtype> 
		//	   <unicast-address>
		Origin origin;

		// Session Name ("s="). REQUIRED.
		// Describes the textual session name. Required field, if there is no 
		// name can be "s= " or "s=-".
		std::string session_name;

		// Session Information ("i="). OPTIONAL.
		// Provides textual information about the session. Can be one "i=" line
		// per session description, and one per media description.
		std::string session_information;

		// URI ("u="). OPTIONAL.
		// Provides a URI (uniform resource identifier), a pointer to
		// additional human readable information about the session. No more 
		// than one "u=" line per session description.
		std::string uri = "";

		// Email Address ("e=") and Phone Number ("p="). OPTIONAL.
		// Specify contact information for the person responsible for the
		// session. See RFC8866 5.6 for formatting specifics.
		std::string email_address;
		std::string phone_number;

		// Connection Information ("c="). REQUIRED.
		// Contains information necessary to establish a network connection.
		// Session description must contain at least one "c=" at the session
		// level, and at each media level.
		// c = <nettype> <addrtype> <connection-address>
		ConnectionInformation connection_information;


		// Bandwidth Information ("b="). OPTIONAL
		// Denotes the proposed bandwidth to be used by the session or media
		// description.
		// b = <bwtype>:<bandwidth>
		std::vector<BandwidthInformation> bandwidth_informations;


		// Time Descriptions. One REQUIRED. More can be added.
		// Specifies the start and stop times for a session. Multiple
		// time descriptions MAY be used if a session is active at multiple
		// irregularly spaced times.  If the session is active at regular 
		// repeat times, a repeat description can be included.
		//
		// ** sdptransform lib only supports one time description, however SDP
		//	  documentation states there can be more than one. If this becomes
		//    relevant, the sdptransform lib can be updated. 
		TimeDescription time_description;


		// Attributes. OPTIONAL, one or more can be set.
		// Primary means for extending SDP. At the session level they convey
		// additional information that applies to the session as a whole. Can
		// also be used in the media description level. 
		// a = <attribute-name> OR a = <attribute - name>:<attribute - value>
		// ** Not easily recognized within sdptranform lib
		Attributes attributes;


		// Media Descriptions. OPTIONAL.
		// A session description may contain a number of media descriptions. 
		// Each media description starts with an "m=" line (media-field) and is 
		// terminated by either the next "m=" line or by the end of the session 
		// description.
		std::vector<shared_ptr<MediaDescription>> media_descriptions;


		/// <summary>
		/// GetVideoHost: Returns video host for from this SDP for use in 
		///				  SwxtchSourceFilter.
		/// </summary>
		std::string GetVideoHost()
		{
			if (media_descriptions.size() == 0)
				throw std::runtime_error("SDP::GetVideoHost: No media descriptions in SDP");

			for (int i = 0; i < media_descriptions.size(); i++)
			{
				if (media_descriptions[i]->m_type == SDPMediaType::VIDEO)
				{
					VideoDescription video_description = dynamic_cast<VideoDescription&>(*media_descriptions[i]);
					if (video_description.connection_information.connection_address == "")
						throw std::runtime_error("SDP::GetVideoHost: Video host not set in SDP");

					return video_description.connection_information.connection_address;
				}
			}

			throw std::runtime_error("SDP::GetVideoPort: Video port not found in SDP");
		}
		

		/// <summary>
		/// GetVideoPort:: Returns video port for from this SDP for use in 
		///				   SwxtchSourceFilter.
		/// </summary>
		std::string GetVideoPort()
		{
			if (media_descriptions.size() == 0)
				throw std::runtime_error("SDP::GetVideoPort: No media descriptions in SDP");

			for (int i = 0; i < media_descriptions.size(); i++)
			{
				if (media_descriptions[i]->m_type == SDPMediaType::VIDEO)
				{
					if (media_descriptions[i]->port == 0)
						throw std::runtime_error("SDP::GetVideoPort: Video port not set in SDP");
					
					return std::to_string(media_descriptions[i]->port);
				}
			}

			throw std::runtime_error("SDP::GetVideoPort: Video port not found in SDP");
		}

	};
}

	