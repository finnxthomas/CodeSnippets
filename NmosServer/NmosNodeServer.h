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
	class NmosNodeServer
	{
	public:	
		NmosNodeServer(const char* interface_ip, unsigned int http_port, int total_stream_count);
		~NmosNodeServer();

		std::vector<std::string> GetSourceSDPs();
		uint32_t GetTotalConnectedSourceCount();

		void AddReceivers();

	private:
		const char* m_asset_functions[1] = { "Nmos Reciever"};
		char m_receiver_sdps[2][2048] = { 0 };
		const char* m_video_receiver_id;
		const char* m_audio_receiver_id;
		char m_seed[512];
		const char* m_interface_ip;
		uint32_t m_total_expected_stream_count = 0;

		shared_ptr<NvNmosNodeServer> m_node_server;
		shared_ptr<NvNmosAssetConfig> m_asset_config;
		shared_ptr<NvNmosNodeConfig> m_node_config;
		shared_ptr<NvNmosReceiverConfig> m_receiver_config[2];

		void InitializeAssetConfig();
		void InitializeNode(unsigned int http_port);
	};
}