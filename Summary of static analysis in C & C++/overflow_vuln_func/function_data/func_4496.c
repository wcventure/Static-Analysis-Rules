typedef struct XMVAudioPacket {
    int stream_index; ///< The decoder stream index for this audio packet.

    /
    uint16_t compression;     ///< The type of compression.
    uint16_t channels;        ///< Number of channels.
    uint32_t sample_rate;     ///< Sampling rate.
    uint16_t bits_per_sample; ///< Bits per compressed sample.
    uint32_t bit_rate;        ///< Bits of compressed data per second.
    uint16_t flags;           ///< Flags
    uint16_t block_align;     ///< Bytes per compressed block.
    uint16_t block_samples;   ///< Decompressed samples per compressed block.

    enum AVCodecID codec_id; ///< The codec ID of the compression scheme.

    uint32_t data_size;   ///< The size of the remaining audio data.
    uint64_t data_offset; ///< The offset of the audio data within the file.

    uint32_t frame_size; ///< Number of bytes to put into an audio frame.

    uint64_t block_count; ///< Running counter of decompressed audio block.
} XMVAudioPacket;
