// this is based on the reference AES code by Paulo Barreto and Vincent Rijmen
int av_aes_init(AVAES *a, const uint8_t *key, int key_bits, int decrypt) {
    int i, j, t, rconpointer = 0;
    uint8_t tk[8][4];
    int KC= key_bits>>5;
    int rounds= KC + 6;
    uint8_t  log8[256];
    uint8_t alog8[512];

    if(!enc_multbl[0][sizeof(enc_multbl)/sizeof(enc_multbl[0][0])-1]){
        j=1;
        for(i=0; i<255; i++){
            alog8[i]=
            alog8[i+255]= j;
            log8[j]= i;
            j^= j+j;
            if(j>255) j^= 0x11B;
        }
        for(i=0; i<256; i++){
            j= i ? alog8[255-log8[i]] : 0;
            j ^= (j<<1) ^ (j<<2) ^ (j<<3) ^ (j<<4);
            j = (j ^ (j>>8) ^ 99) & 255;
            inv_sbox[j]= i;
            sbox    [i]= j;
        }
        init_multbl2(dec_multbl[0], (const int[4]){0xe, 0x9, 0xd, 0xb}, log8, alog8, inv_sbox);
        init_multbl2(enc_multbl[0], (const int[4]){0x2, 0x1, 0x1, 0x3}, log8, alog8, sbox);
    }

    if(key_bits!=128 && key_bits!=192 && key_bits!=256)
        return -1;

    a->rounds= rounds;

    memcpy(tk, key, KC*4);

    for(t= 0; t < (rounds+1)*16;) {
        memcpy(a->round_key[0].u8+t, tk, KC*4);
        t+= KC*4;

        for(i = 0; i < 4; i++)
            tk[0][i] ^= sbox[tk[KC-1][(i+1)&3]];
        tk[0][0] ^= rcon[rconpointer++];

        for(j = 1; j < KC; j++){
            if(KC != 8 || j != KC>>1)
                for(i = 0; i < 4; i++) tk[j][i] ^=      tk[j-1][i];
            else
                for(i = 0; i < 4; i++) tk[j][i] ^= sbox[tk[j-1][i]];
        }
    }

    if(decrypt){
        for(i=1; i<rounds; i++){
            av_aes_block tmp[3];
            memcpy(&tmp[2], &a->round_key[i], 16);
            subshift(&tmp[1], 0, sbox);
            mix(tmp, dec_multbl, 1, 3);
            memcpy(&a->round_key[i], &tmp[0], 16);
        }
    }else{
        for(i=0; i<(rounds+1)>>1; i++){
            for(j=0; j<16; j++)
                FFSWAP(int, a->round_key[i].u8[j], a->round_key[rounds-i].u8[j]);
        }
    }

    return 0;
}
