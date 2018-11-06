typedef struct {
    uint_fast16_t type;
    uint_fast32_t begin;
    uint_fast32_t end;
    unsigned      partition_size;
    uint_fast8_t  classifications;
    uint_fast8_t  classbook;
    int_fast16_t  books[64][8];
    uint_fast8_t  maxpass;
    uint_fast16_t ptns_to_read;
    uint_fast8_t *classifs;
} vorbis_residue;
