static int vorbis_floor1_decode(vorbis_context *vc,
                                vorbis_floor_data *vfu, float *vec)
{
    vorbis_floor1 *vf = &vfu->t1;
    GetBitContext *gb = &vc->gb;
    uint16_t range_v[4] = { 256, 128, 86, 64 };
    unsigned range = range_v[vf->multiplier - 1];
    uint16_t floor1_Y[258];
    uint16_t floor1_Y_final[258];
    int floor1_flag[258];
    unsigned class, cdim, cbits, csub, cval, offset, i, j;
    int book, adx, ady, dy, off, predicted, err;


    if (!get_bits1(gb)) // silence
        return 1;

// Read values (or differences) for the floor's points

    floor1_Y[0] = get_bits(gb, ilog(range - 1));
    floor1_Y[1] = get_bits(gb, ilog(range - 1));

    av_dlog(NULL, "floor 0 Y %d floor 1 Y %d \n", floor1_Y[0], floor1_Y[1]);

    offset = 2;
    for (i = 0; i < vf->partitions; ++i) {
        class = vf->partition_class[i];
        cdim   = vf->class_dimensions[class];
        cbits  = vf->class_subclasses[class];
        csub = (1 << cbits) - 1;
        cval = 0;

        av_dlog(NULL, "Cbits %u\n", cbits);

        if (cbits) // this reads all subclasses for this partition's class
            cval = get_vlc2(gb, vc->codebooks[vf->class_masterbook[class]].vlc.table,
                            vc->codebooks[vf->class_masterbook[class]].nb_bits, 3);

        for (j = 0; j < cdim; ++j) {
            book = vf->subclass_books[class][cval & csub];

            av_dlog(NULL, "book %d Cbits %u cval %u  bits:%d\n",
                    book, cbits, cval, get_bits_count(gb));

            cval = cval >> cbits;
            if (book > -1) {
                floor1_Y[offset+j] = get_vlc2(gb, vc->codebooks[book].vlc.table,
                vc->codebooks[book].nb_bits, 3);
            } else {
                floor1_Y[offset+j] = 0;
            }

            av_dlog(NULL, " floor(%d) = %d \n",
                    vf->list[offset+j].x, floor1_Y[offset+j]);
        }
        offset+=cdim;
    }

// Amplitude calculation from the differences

    floor1_flag[0] = 1;
    floor1_flag[1] = 1;
    floor1_Y_final[0] = floor1_Y[0];
    floor1_Y_final[1] = floor1_Y[1];

    for (i = 2; i < vf->x_list_dim; ++i) {
        unsigned val, highroom, lowroom, room, high_neigh_offs, low_neigh_offs;

        low_neigh_offs  = vf->list[i].low;
        high_neigh_offs = vf->list[i].high;
        dy  = floor1_Y_final[high_neigh_offs] - floor1_Y_final[low_neigh_offs];  // render_point begin
        adx = vf->list[high_neigh_offs].x - vf->list[low_neigh_offs].x;
        ady = FFABS(dy);
        err = ady * (vf->list[i].x - vf->list[low_neigh_offs].x);
        off = err / adx;
        if (dy < 0) {
            predicted = floor1_Y_final[low_neigh_offs] - off;
        } else {
            predicted = floor1_Y_final[low_neigh_offs] + off;
        } // render_point end

        val = floor1_Y[i];
        highroom = range-predicted;
        lowroom  = predicted;
        if (highroom < lowroom) {
            room = highroom * 2;
        } else {
            room = lowroom * 2;   // SPEC mispelling
        }
        if (val) {
            floor1_flag[low_neigh_offs]  = 1;
            floor1_flag[high_neigh_offs] = 1;
            floor1_flag[i]               = 1;
            if (val >= room) {
                if (highroom > lowroom) {
                    floor1_Y_final[i] = val - lowroom + predicted;
                } else {
                    floor1_Y_final[i] = predicted - val + highroom - 1;
                }
            } else {
                if (val & 1) {
                    floor1_Y_final[i] = predicted - (val + 1) / 2;
                } else {
                    floor1_Y_final[i] = predicted + val / 2;
                }
            }
        } else {
            floor1_flag[i]    = 0;
            floor1_Y_final[i] = predicted;
        }

        av_dlog(NULL, " Decoded floor(%d) = %u / val %u\n",
                vf->list[i].x, floor1_Y_final[i], val);
    }

// Curve synth - connect the calculated dots and convert from dB scale FIXME optimize ?

    ff_vorbis_floor1_render_list(vf->list, vf->x_list_dim, floor1_Y_final, floor1_flag, vf->multiplier, vec, vf->list[1].x);

    av_dlog(NULL, " Floor decoded\n");

    return 0;
}
