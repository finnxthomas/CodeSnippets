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

#include "NmosTestSender.h"

static void handle_log(
    NvNmosNodeServer* server,
    const char* categories,
    int level,
    const char* message)
{
    printf("%s [%d:%s]\n", message, level, categories);
}

static bool handle_rtp_connection_activated(
    NvNmosNodeServer* server,
    const char* id,
    const char* sdp)
{
    printf("%s %s\n", id, sdp ? "activated via NMOS" : "deactivated via NMOS");
    if (server->user_data && sdp) printf("%s\n", sdp);
    return true;
}

// construct example SDP for video sender or receiver
static bool init_video_sdp(char* sdp, size_t sdp_size, bool sender, const char* id, const char* interface_ip, const char* label, const char* group_hint, bool ptp)
{
    const char* description = VIDEO_DESCRIPTION;
    const char* encoding = VIDEO_ENCODING_PARAMETERS;
    const char* format_specific_parameters = VIDEO_FORMAT_SPECIFIC_PARAMETERS;

    const char* multicast_ip = "x.x.x.x"; 
    const char* source_ip = "x.x.x.x"; 
    int destination_port = 5020;
    int source_port = 5004;
    int payload_type = 96; // conventional
    const char* ts_refclk = ptp
        ? "a=ts-refclk:ptp=IEEE1588-2008:AC-DE-48-23-45-67-01-9F:42\r\n"
        "a=ts-refclk:ptp=IEEE1588-2008:traceable\r\n" // use both to include all parameters required for NMOS
        : "a=ts-refclk:localmac=CA-FE-01-CA-FE-02\r\n";

    int ntp = (int)time(0);

    int result = snprintf(sdp,
        sdp_size,
        "v=0\r\n"                       // Version
        "o=- %d %d IN IP4 %s\r\n"       // Origin: timestamp + interface IP
        "s=%s\r\n"                      // Session Name: Ex: "Nvnmos Video Receiver"
        "t=0 0\r\n"                     // Timing. 0 for start and stop means session is permanent
        "a=x-nvnmos-id:%s\r\n"          // Special ID for nvnmos streams.
        "a=recvonly\r\n"
        "m=video %d RTP/AVP %d\r\n"     // Type of media to be received with receiving port
        "c=IN IP4 %s/64\r\n"            // Multicast group IP ****we might need to change this ???
        //"a=source-filter: incl IN IP4 %s %s\r\n" // omit for any-source multicast for receiver. May neeed to un-comment this so we don't accidently receive SDP's from IP addresses we don't want
        "a=x-nvnmos-iface-ip:%s\r\n"
        "a=rtpmap:%d %s\r\n"
        "a=fmtp:%d %s\r\n"
        "a=mediaclk:direct=0\r\n",
        ntp,
        ntp,
        interface_ip,
        label,
        id,
        destination_port,
        payload_type,
        multicast_ip,
        interface_ip,
        payload_type,
        encoding,
        payload_type,
        format_specific_parameters
    );

    return 0 < result && (size_t)result < sdp_size;
}

// construct example SDP for audio sender or receiver
static bool init_audio_sdp(char* sdp, size_t sdp_size, bool sender, const char* id, const char* interface_ip, const char* label, const char* group_hint, bool ptp)
{
    const char* description = AUDIO_DESCRIPTION;
    const char* encoding = AUDIO_ENCODING_PARAMETERS;
    const char* format_specific_parameters = AUDIO_FORMAT_SPECIFIC_PARAMETERS;

    const char* multicast_ip = "x.x.x.x"; // MCAST-TEST-NET
    const char* source_ip = "x.x.x.x"; // TEST-NET-1
    int destination_port = 5030;
    int source_port = 5004;
    int payload_type = 97; // conventional
    const char* ptime = "a=ptime:1\r\n";
    const char* ts_refclk = ptp
        ? "a=ts-refclk:ptp=IEEE1588-2008:AC-DE-48-23-45-67-01-9F:42\r\n"
        "a=ts-refclk:ptp=IEEE1588-2008:traceable\r\n" // use both to include all parameters required for NMOS
        : "a=ts-refclk:localmac=CA-FE-01-CA-FE-02\r\n";

    int ntp = (int)time(0);

    int result = snprintf(sdp,
        sdp_size,
        "v=0\r\n"
        "o=- %d %d IN IP4 %s\r\n"
        "s=%s\r\n"
        "i=%s\r\n" // optional
        "t=0 0\r\n"
        "a=x-nvnmos-id:%s\r\n"
        "a=x-nvnmos-group-hint:%s\r\n" // optional
        "m=audio %d RTP/AVP %d\r\n"
        "c=IN IP4 %s/64\r\n"
        "a=source-filter: incl IN IP4 %s %s\r\n" // omitted for any-source multicast for receiver
        "a=x-nvnmos-iface-ip:%s\r\n"
        "a=x-nvnmos-src-port:%d\r\n" // not applicable for receiver
        "a=rtpmap:%d %s\r\n"
        "a=fmtp:%d %s\r\n"
        "%s"
        "%s"
        "a=mediaclk:direct=0\r\n",
        ntp,
        ntp,
        interface_ip,
        label,
        description,
        id,
        group_hint,
        destination_port,
        payload_type,
        multicast_ip,
        multicast_ip,
        sender ? interface_ip : source_ip,
        interface_ip,
        source_port,
        payload_type,
        encoding,
        payload_type,
        format_specific_parameters,
        sender ? ptime : "", // optional for receiver caps
        sender ? ts_refclk : ""
    );

    return 0 < result && (size_t)result < sdp_size;
}



static bool get_continue(void)
{
    printf("Continue ([y]/n)?\n");
    int c = fgetc(stdin);
    bool result = c == '\n' || tolower(c) == 'y';
    while (c != '\n' && c != EOF)
        c = fgetc(stdin);
    return result;
}


int main()
{
    // Create Asset Config
    const char* functions[1] = {
        "Nmos Test Sender"
    };
    std::shared_ptr<NvNmosAssetConfig> asset_config_sender = std::make_shared<NvNmosAssetConfig>();
    asset_config_sender->manufacturer = "My Company";
    asset_config_sender->product = "My Sender";
    asset_config_sender->instance_id = "XYZ123-456789";
    asset_config_sender->functions = &functions[0];
    asset_config_sender->num_functions = 1;


    // Create Node Config
    std::shared_ptr<NvNmosNodeConfig> node_config_sender =std::make_shared<NvNmosNodeConfig>();
    node_config_sender->host_name = "nmos-sender.local";
    node_config_sender->http_port = 8086;
    node_config_sender->asset_tags = static_cast<NvNmosAssetConfig*>(asset_config_sender.get());

    char seed[512];
    if (snprintf(seed, sizeof seed, "%s:%s", "nmos-sender.local", "8086") < 0) return 1;
    node_config_sender->seed = seed;
    node_config_sender->senders = NULL;
    node_config_sender->num_senders = 0;
    node_config_sender->receivers = NULL;
    node_config_sender->num_receivers = 0;
    node_config_sender->rtp_connection_activated = &handle_rtp_connection_activated;
    node_config_sender->log_callback = &handle_log;
    node_config_sender->log_level = NVNMOS_LOG_ERROR;


    // Create Sender SDP and Sender Config
    const char* interface_ip = "192.168.56.1";
    bool ptp = CLK_PTP;

    char sender_sdp[2][2048] = { 0 };
    if (!init_video_sdp(sender_sdp[0], sizeof sender_sdp[0], true, "sender-video", interface_ip, "NvNmos Video Sender", "tx-0:video", ptp)) return 1;
    if (!init_audio_sdp(sender_sdp[1], sizeof sender_sdp[1], true, "sender-audio", interface_ip, "NvNmos Audio Sender", "tx-0:audio", ptp)) return 1;

    NvNmosSenderConfig sender_config[2] = { 0 };
    sender_config[0].sdp = sender_sdp[0];
    sender_config[1].sdp = sender_sdp[1];


    // Create Node Server
    std::shared_ptr<NvNmosNodeServer> node_server_sender = std::make_shared<NvNmosNodeServer>();
    node_server_sender->user_data = (void*)1;


    // Run Test Node Sender
    printf("Creating NvNmos server for sending...\n");
    if (!create_nmos_node_server(static_cast<NvNmosNodeConfig*>(node_config_sender.get()), static_cast<NvNmosNodeServer*>(node_server_sender.get()))) return 1;

    printf("Adding sender to NvNmos Server...\n");
    if (!add_nmos_sender_to_node_server(static_cast<NvNmosNodeServer*>(node_server_sender.get()), &sender_config[0])) goto cleanup;
    if (!add_nmos_sender_to_node_server(static_cast<NvNmosNodeServer*>(node_server_sender.get()), &sender_config[1])) goto cleanup;


    printf("Activating sender...\n\n");
    if (!nmos_connection_rtp_activate(static_cast<NvNmosNodeServer*>(node_server_sender.get()), "sender-video", sender_config[0].sdp)) goto cleanup;
    if (!nmos_connection_rtp_activate(static_cast<NvNmosNodeServer*>(node_server_sender.get()), "sender-audio", sender_config[1].sdp)) goto cleanup;

    printf("Deactivate the sender node and destroy server?\n");
    if (!get_continue()) goto cleanup;

    printf("Deactivating sender...\n");
    if (!nmos_connection_rtp_activate(static_cast<NvNmosNodeServer*>(node_server_sender.get()), "sender-video", 0)) goto cleanup;
    if (!nmos_connection_rtp_activate(static_cast<NvNmosNodeServer*>(node_server_sender.get()), "sender-audio", 0)) goto cleanup;


    printf("Destroying NvNmos servers...\n");
    if (!destroy_nmos_node_server(static_cast<NvNmosNodeServer*>(node_server_sender.get()))) return 1;

    printf("Finished\n");
    return 0;

cleanup:
    destroy_nmos_node_server(static_cast<NvNmosNodeServer*>(node_server_sender.get()));
    return 1;
}
