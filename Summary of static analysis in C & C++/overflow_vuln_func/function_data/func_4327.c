typedef struct {
    /
    GetBitContext gb;             ///< packet bitreader. During decoder init,
                                  ///< it contains the extradata from the
                                  ///< demuxer. During decoding, it contains
                                  ///< packet data.
    int8_t vbm_tree[25];          ///< converts VLC codes to frame type

    int spillover_bitsize;        ///< number of bits used to specify
                                  ///< #spillover_nbits in the packet header
                                  ///< = ceil(log2(ctx->block_align << 3))
    int history_nsamples;         ///< number of samples in history for signal
                                  ///< prediction (through ACB)

    /
    int do_apf;                   ///< whether to apply the averaged
                                  ///< projection filter (APF)
    int denoise_strength;         ///< strength of denoising in Wiener filter
                                  ///< [0-11]
    int denoise_tilt_corr;        ///< Whether to apply tilt correction to the
                                  ///< Wiener filter coefficients (postfilter)
    int dc_level;                 ///< Predicted amount of DC noise, based
                                  ///< on which a DC removal filter is used

    int lsps;                     ///< number of LSPs per frame [10 or 16]
    int lsp_q_mode;               ///< defines quantizer defaults [0, 1]
    int lsp_def_mode;             ///< defines different sets of LSP defaults
                                  ///< [0, 1]
    int frame_lsp_bitsize;        ///< size (in bits) of LSPs, when encoded
                                  ///< per-frame (independent coding)
    int sframe_lsp_bitsize;       ///< size (in bits) of LSPs, when encoded
                                  ///< per superframe (residual coding)

    int min_pitch_val;            ///< base value for pitch parsing code
    int max_pitch_val;            ///< max value + 1 for pitch parsing
    int pitch_nbits;              ///< number of bits used to specify the
                                  ///< pitch value in the frame header
    int block_pitch_nbits;        ///< number of bits used to specify the
                                  ///< first block's pitch value
    int block_pitch_range;        ///< range of the block pitch
    int block_delta_pitch_nbits;  ///< number of bits used to specify the
                                  ///< delta pitch between this and the last
                                  ///< block's pitch value, used in all but
                                  ///< first block
    int block_delta_pitch_hrange; ///< 1/2 range of the delta (full range is
                                  ///< from -this to +this-1)
    uint16_t block_conv_table[4]; ///< boundaries for block pitch unit/scale
                                  ///< conversion

    /
    int spillover_nbits;          ///< number of bits of the previous packet's
                                  ///< last superframe preceeding this
                                  ///< packet's first full superframe (useful
                                  ///< for re-synchronization also)
    int has_residual_lsps;        ///< if set, superframes contain one set of
                                  ///< LSPs that cover all frames, encoded as
                                  ///< independent and residual LSPs; if not
                                  ///< set, each frame contains its own, fully
                                  ///< independent, LSPs
    int skip_bits_next;           ///< number of bits to skip at the next call
                                  ///< to #wmavoice_decode_packet() (since
                                  ///< they're part of the previous superframe)

    uint8_t sframe_cache[SFRAME_CACHE_MAXSIZE + FF_INPUT_BUFFER_PADDING_SIZE];
                                  ///< cache for superframe data split over
                                  ///< multiple packets
    int sframe_cache_size;        ///< set to >0 if we have data from an
                                  ///< (incomplete) superframe from a previous
                                  ///< packet that spilled over in the current
                                  ///< packet; specifies the amount of bits in
                                  ///< #sframe_cache
    PutBitContext pb;             ///< bitstream writer for #sframe_cache

    /
    double prev_lsps[MAX_LSPS];   ///< LSPs of the last frame of the previous
                                  ///< superframe
    int last_pitch_val;           ///< pitch value of the previous frame
    int last_acb_type;            ///< frame type [0-2] of the previous frame
    int pitch_diff_sh16;          ///< ((cur_pitch_val - #last_pitch_val)
                                  ///< << 16) / #MAX_FRAMESIZE
    float silence_gain;           ///< set for use in blocks if #ACB_TYPE_NONE

    int aw_idx_is_ext;            ///< whether the AW index was encoded in
                                  ///< 8 bits (instead of 6)
    int aw_pulse_range;           ///< the range over which #aw_pulse_set1()
                                  ///< can apply the pulse, relative to the
                                  ///< value in aw_first_pulse_off. The exact
                                  ///< position of the first AW-pulse is within
                                  ///< [pulse_off, pulse_off + this], and
                                  ///< depends on bitstream values; [16 or 24]
    int aw_n_pulses[2];           ///< number of AW-pulses in each block; note
                                  ///< that this number can be negative (in
                                  ///< which case it basically means "zero")
    int aw_first_pulse_off[2];    ///< index of first sample to which to
                                  ///< apply AW-pulses, or -0xff if unset
    int aw_next_pulse_off_cache;  ///< the position (relative to start of the
                                  ///< second block) at which pulses should
                                  ///< start to be positioned, serves as a
                                  ///< cache for pitch-adaptive window pulses
                                  ///< between blocks

    int frame_cntr;               ///< current frame index [0 - 0xFFFE]; is
                                  ///< only used for comfort noise in #pRNG()
    float gain_pred_err[6];       ///< cache for gain prediction
    float excitation_history[MAX_SIGNAL_HISTORY];
                                  ///< cache of the signal of previous
                                  ///< superframes, used as a history for
                                  ///< signal generation
    float synth_history[MAX_LSPS]; ///< see #excitation_history
    /
    RDFTContext rdft, irdft;      ///< contexts for FFT-calculation in the
                                  ///< postfilter (for denoise filter)
    DCTContext dct, dst;          ///< contexts for phase shift (in Hilbert
                                  ///< transform, part of postfilter)
    float sin[511], cos[511];     ///< 8-bit cosine/sine windows over [-pi,pi]
                                  ///< range
    float postfilter_agc;         ///< gain control memory, used in
                                  ///< #adaptive_gain_control()
    float dcf_mem[2];             ///< DC filter history
    float zero_exc_pf[MAX_SIGNAL_HISTORY + MAX_SFRAMESIZE];
                                  ///< zero filter output (i.e. excitation)
                                  ///< by postfilter
    float denoise_filter_cache[MAX_FRAMESIZE];
    int   denoise_filter_cache_size; ///< samples in #denoise_filter_cache
    DECLARE_ALIGNED(16, float, tilted_lpcs_pf)[0x80];
                                  ///< aligned buffer for LPC tilting
    DECLARE_ALIGNED(16, float, denoise_coeffs_pf)[0x80];
                                  ///< aligned buffer for denoise coefficients
    DECLARE_ALIGNED(16, float, synth_filter_out_buf)[80 + MAX_LSPS_ALIGN16];
                                  ///< aligned buffer for postfilter speech
                                  ///< synthesis
    /
} WMAVoiceContext;
