typedef struct ASFContext {
    uint32_t seqno;
    int is_streamed;
    ASFStream streams[128];              ///< it's max number and it's not that big
    /
    uint64_t nb_packets;                 ///< how many packets are there in the file, invalid if broadcasting
    int64_t duration;                    ///< in 100ns units
    /
    unsigned char multi_payloads_present;
    int packet_size_left;
    int packet_timestamp_start;
    int packet_timestamp_end;
    unsigned int packet_nb_payloads;
    uint8_t packet_buf[PACKET_SIZE];
    AVIOContext pb;
    /
    uint64_t data_offset;                ///< beginning of the first data packet

    int64_t last_indexed_pts;
    ASFIndex *index_ptr;
    uint32_t nb_index_count;
    uint32_t nb_index_memory_alloc;
    uint16_t maximum_packet;
} ASFContext;
