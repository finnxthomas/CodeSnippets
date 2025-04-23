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

#include "nvnmos_adapter.h"

namespace Cf
{
    struct ProtectedSDPList
    {
        std::mutex mutex;
        std::vector<std::string> list_sdp;

        int nmos_source_count = 0;
    };  

    /// <summary>
    /// handle_log: Callback for when the NMOS node server logs a message.
    /// </summary>
    static void handle_log(
        NvNmosNodeServer* server,
        const char* categories,
        int level,
        const char* message)
    {
        PLOG_INFO << "NmosNodeServer: " << message << " [" << std::to_string(level) << ":" << categories << "]";
    }


    /// <summary>
    /// handle_rtp_connection_activated: Callback for when a NMOS sender 
    ///                                  connects to our receiver.  
    /// </summary>
    static bool handle_rtp_connection_activated(
        NvNmosNodeServer* server,
        const char* id,
        const char* sdp)
    {
        auto protected_sdp_list = (ProtectedSDPList*)server->user_data;

        {
            std::lock_guard lock(protected_sdp_list->mutex);

            protected_sdp_list->list_sdp.push_back(sdp);
            protected_sdp_list->nmos_source_count++;

            // destructor of lock will free the mutex, allowing someone else to 
            // access the protected resource
        }

        return true;
    }


    /// <summary>
    /// NvNmosNode Constructor: Create Node Server/Node for Application
    /// </summary>
    NmosNodeServer::NmosNodeServer(const char* interface_ip, unsigned int http_port, int total_stream_count)
    {
        m_interface_ip = interface_ip;
        m_total_expected_stream_count = total_stream_count;

        // Create the Node Server
        m_node_server = std::make_shared<NvNmosNodeServer>();
        m_node_server->user_data = new ProtectedSDPList();

        // Create the Asset Config. This is part of the NodeConfig requirements
        m_asset_config = std::make_shared<NvNmosAssetConfig>();
        InitializeAssetConfig();

        m_node_config = std::make_shared<NvNmosNodeConfig>();
        InitializeNode(http_port);

        PLOG_INFO << "Creating NvNmos server on interface " << interface_ip << " and port " << http_port;
        if (!create_nmos_node_server(m_node_config.get(), m_node_server.get()))
        {
            throw std::runtime_error("NmosNodeServer: Failed to create NMOS Node Server");
        }
    }

    
    /// <summary>
    /// NmosNodeServer Destructor: Destroy the NMOS node server and free up memory.
    /// </summary>
    NmosNodeServer::~NmosNodeServer()
    {
        if (m_node_server->user_data != nullptr)
        {
            delete m_node_server->user_data;
            m_node_server->user_data = nullptr;
        }

        /*if (m_node_config->host_addresses != nullptr)
        {
            delete m_node_config->host_addresses;
            m_node_config->host_addresses = nullptr;
        }*/

        PLOG_INFO << "Destroying NvNmos server";
        destroy_nmos_node_server(m_node_server.get());
    }


    /// <summary>
    /// InitilaizeAssetConfig: Set up the nmos "device" for our node. Describes
    ///                        the capabilities of the node in the NMOS registry.  
    /// </summary>
    void NmosNodeServer::InitializeAssetConfig()
    {
        m_asset_config->manufacturer = "My Company";
        m_asset_config->product = "My Product";
        m_asset_config->instance_id = "my-nmos";
        m_asset_config->functions = &m_asset_functions[0];
        m_asset_config->num_functions = 1;
    }


    /// <summary>
    /// AddReceiver: Add a receiver (for audio and video) to the member 
    ///              node/server if it is available.
    /// </summary>
    void NmosNodeServer::AddReceivers()
    {
        m_video_receiver_id = "video-receiver";
        m_audio_receiver_id = "audio-receiver";

        // Create receiver SDP for video
        if (!init_video_receiver_sdp(m_receiver_sdps[0], sizeof m_receiver_sdps[0], m_video_receiver_id, m_interface_ip, "Nmos Video Receiver"))
        {
            throw std::runtime_error("NmosNodeServer: Failed to initialize video receiver SDP");
        }

        //Create receiver SDP for audio
        if (!init_audio_receiver_sdp(m_receiver_sdps[1], sizeof m_receiver_sdps[1], false, m_audio_receiver_id, m_interface_ip, "Nmos Audio Receiver", "group hint", true))
        {
            throw std::runtime_error("NmosNodeServer: Failed to initialize audio receiver SDP");
        }

        // Initialize video receiver config
        m_receiver_config[0] = make_shared<NvNmosReceiverConfig>();
        m_receiver_config[0]->sdp = m_receiver_sdps[0];

        // Initialize audio receiver config
        m_receiver_config[1] = make_shared<NvNmosReceiverConfig>();
        m_receiver_config[1]->sdp = m_receiver_sdps[1];


        // Add receiver config to node server
        PLOG_INFO << "Adding receivers to the Nmos node server";

        // Adding video receiver
        if (!add_nmos_receiver_to_node_server(m_node_server.get(), m_receiver_config[0].get()))
        {
            throw std::runtime_error("NmosNodeServer: Failed to add receiver to node server");
        }

        // Adding audio receiver
        if (!add_nmos_receiver_to_node_server(m_node_server.get(), m_receiver_config[1].get()))
        {
            throw std::runtime_error("NmosNodeServer: Failed to add receiver to node server");
        }

        PLOG_INFO << "Activating video NMOS receiver...";
        if (!nmos_connection_rtp_activate(m_node_server.get(),
            m_video_receiver_id, m_receiver_config[0]->sdp))
        {
            throw std::runtime_error("Failed to activate receiver");
        }

        PLOG_INFO << "Activating nmos NMOS receiver...";
        if (!nmos_connection_rtp_activate(m_node_server.get(),
            m_audio_receiver_id, m_receiver_config[1]->sdp))
        {
            throw std::runtime_error("Failed to activate receiver");
        }
    }


    /// <summary>
    /// InitializeNode: Set up the node configuration for the NMOS node server
    ///                 This includes the callback for when the receiver is
    ///                 activated when it finds a sender.
    /// </summary>
    void NmosNodeServer::InitializeNode(unsigned int http_port)
    {
       // REDACTED
    }


    /// <summary>
    /// GetTotalConnectedSourceCount: Get the total number of sources that have
    ///                               connected to our NMOS node server.    
    /// </summary>
    uint32_t NmosNodeServer::GetTotalConnectedSourceCount()
    {
        auto protected_sdp_list = (ProtectedSDPList*)m_node_server->user_data;
        {
            std::lock_guard lock(protected_sdp_list->mutex);
            return protected_sdp_list->nmos_source_count;
        }
    }

    
    /// <summary>
    /// GetSourceSDPs: If source SDPs have been connected to the Nmos
    ///                Node via Nmos Controller, then return those SDPs for processing.
    /// </summary>
    std::vector<std::string> NmosNodeServer::GetSourceSDPs()
    {
        auto protected_sdp_list = (ProtectedSDPList*)m_node_server->user_data;
        {
            std::lock_guard lock(protected_sdp_list->mutex);

            if (protected_sdp_list->list_sdp.size() > 0)
            {
                return protected_sdp_list->list_sdp;
            }
            else
            {
                throw std::runtime_error("No NMOS source SDPs found");
            }
        }
    }
}