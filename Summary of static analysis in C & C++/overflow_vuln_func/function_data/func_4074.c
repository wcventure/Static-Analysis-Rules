typedef struct RTMPContext {
    const AVClass *class;
    URLContext*   stream;                     ///< TCP stream used in interactions with RTMP server
    RTMPPacket    *prev_pkt[2];               ///< packet history used when reading and sending packets ([0] for reading, [1] for writing)
    int           nb_prev_pkt[2];             ///< number of elements in prev_pkt
    int           in_chunk_size;              ///< size of the chunks incoming RTMP packets are divided into
    int           out_chunk_size;             ///< size of the chunks outgoing RTMP packets are divided into
    int           is_input;                   ///< input/output flag
    char          *playpath;                  ///< stream identifier to play (with possible "mp4:" prefix)
    int           live;                       ///< 0: recorded, -1: live, -2: both
    char          *app;                       ///< name of application
    char          *conn;                      ///< append arbitrary AMF data to the Connect message
    ClientState   state;                      ///< current state
    int           stream_id;                  ///< ID assigned by the server for the stream
    uint8_t*      flv_data;                   ///< buffer with data for demuxer
    int           flv_size;                   ///< current buffer size
    int           flv_off;                    ///< number of bytes read from current buffer
    int           flv_nb_packets;             ///< number of flv packets published
    RTMPPacket    out_pkt;                    ///< rtmp packet, created from flv a/v or metadata (for output)
    uint32_t      client_report_size;         ///< number of bytes after which client should report to server
    uint32_t      bytes_read;                 ///< number of bytes read from server
    uint32_t      last_bytes_read;            ///< number of bytes read last reported to server
    uint32_t      last_timestamp;             ///< last timestamp received in a packet
    int           skip_bytes;                 ///< number of bytes to skip from the input FLV stream in the next write call
    int           has_audio;                  ///< presence of audio data
    int           has_video;                  ///< presence of video data
    int           received_metadata;          ///< Indicates if we have received metadata about the streams
    uint8_t       flv_header[RTMP_HEADER];    ///< partial incoming flv packet header
    int           flv_header_bytes;           ///< number of initialized bytes in flv_header
    int           nb_invokes;                 ///< keeps track of invoke messages
    char*         tcurl;                      ///< url of the target stream
    char*         flashver;                   ///< version of the flash plugin
    char*         swfhash;                    ///< SHA256 hash of the decompressed SWF file (32 bytes)
    int           swfhash_len;                ///< length of the SHA256 hash
    int           swfsize;                    ///< size of the decompressed SWF file
    char*         swfurl;                     ///< url of the swf player
    char*         swfverify;                  ///< URL to player swf file, compute hash/size automatically
    char          swfverification[42];        ///< hash of the SWF verification
    char*         pageurl;                    ///< url of the web page
    char*         subscribe;                  ///< name of live stream to subscribe
    int           server_bw;                  ///< server bandwidth
    int           client_buffer_time;         ///< client buffer time in ms
    int           flush_interval;             ///< number of packets flushed in the same request (RTMPT only)
    int           encrypted;                  ///< use an encrypted connection (RTMPE only)
    TrackedMethod*tracked_methods;            ///< tracked methods buffer
    int           nb_tracked_methods;         ///< number of tracked methods
    int           tracked_methods_size;       ///< size of the tracked methods buffer
    int           listen;                     ///< listen mode flag
    int           listen_timeout;             ///< listen timeout to wait for new connections
    int           nb_streamid;                ///< The next stream id to return on createStream calls
    double        duration;                   ///< Duration of the stream in seconds as returned by the server (only valid if non-zero)
    char          username[50];
    char          password[50];
    char          auth_params[500];
    int           do_reconnect;
    int           auth_tried;
} RTMPContext;
