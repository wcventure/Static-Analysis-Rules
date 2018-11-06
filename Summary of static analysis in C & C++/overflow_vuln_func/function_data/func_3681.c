static int dv_extract_audio(uint8_t* frame, uint8_t* ppcm[4],
                            const DVprofile *sys)
{
    int size, chan, i, j, d, of, smpls, freq, quant, half_ch;
    uint16_t lc, rc;
    const uint8_t* as_pack;
    uint8_t *pcm, ipcm;

    as_pack = dv_extract_pack(frame, dv_audio_source);
    if (!as_pack)    /
        return 0;

    smpls =  as_pack[1] & 0x3f;       /
    freq  = (as_pack[4] >> 3) & 0x07; /
    quant =  as_pack[4] & 0x07;       /

    if (quant > 1)
        return -1; /

    size = (sys->audio_min_samples[freq] + smpls) * 4; /
    half_ch = sys->difseg_size / 2;

    /
    ipcm = (sys->height == 720 && !(frame[1] & 0x0C)) ? 2 : 0;
    pcm  = ppcm[ipcm++];

    /
    for (chan = 0; chan < sys->n_difchan; chan++) {
        /
        for (i = 0; i < sys->difseg_size; i++) {
            frame += 6 * 80; /
            if (quant == 1 && i == half_ch) {
                /
                pcm = ppcm[ipcm++];
                if (!pcm)
                    break;
            }

            /
            for (j = 0; j < 9; j++) {
                for (d = 8; d < 80; d += 2) {
                    if (quant == 0) {  /
                        of = sys->audio_shuffle[i][j] + (d - 8) / 2 * sys->audio_stride;
                        if (of*2 >= size)
                            continue;

                        pcm[of*2]   = frame[d+1]; // FIXME: maybe we have to admit
                        pcm[of*2+1] = frame[d];   //        that DV is a big-endian PCM
                        if (pcm[of*2+1] == 0x80 && pcm[of*2] == 0x00)
                            pcm[of*2+1] = 0;
                    } else {           /
                        lc = ((uint16_t)frame[d]   << 4) |
                             ((uint16_t)frame[d+2] >> 4);
                        rc = ((uint16_t)frame[d+1] << 4) |
                             ((uint16_t)frame[d+2] & 0x0f);
                        lc = (lc == 0x800 ? 0 : dv_audio_12to16(lc));
                        rc = (rc == 0x800 ? 0 : dv_audio_12to16(rc));

                        of = sys->audio_shuffle[i%half_ch][j] + (d - 8) / 3 * sys->audio_stride;
                        if (of*2 >= size)
                            continue;

                        pcm[of*2]   = lc & 0xff; // FIXME: maybe we have to admit
                        pcm[of*2+1] = lc >> 8;   //        that DV is a big-endian PCM
                        of = sys->audio_shuffle[i%half_ch+half_ch][j] +
                            (d - 8) / 3 * sys->audio_stride;
                        pcm[of*2]   = rc & 0xff; // FIXME: maybe we have to admit
                        pcm[of*2+1] = rc >> 8;   //        that DV is a big-endian PCM
                        ++d;
                    }
                }

                frame += 16 * 80; /
            }
        }

        /
        pcm = ppcm[ipcm++];
        if (!pcm)
            break;
    }

    return size;
}
