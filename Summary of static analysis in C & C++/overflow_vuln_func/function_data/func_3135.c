void dct32(INTFLOAT *out, const INTFLOAT *tab)
{
    SUINTFLOAT tmp0, tmp1;

    SUINTFLOAT val0 , val1 , val2 , val3 , val4 , val5 , val6 , val7 ,
             val8 , val9 , val10, val11, val12, val13, val14, val15,
             val16, val17, val18, val19, val20, val21, val22, val23,
             val24, val25, val26, val27, val28, val29, val30, val31;

    /
    BF0( 0, 31, COS0_0 , 1);
    BF0(15, 16, COS0_15, 5);
    /
    BF( 0, 15, COS1_0 , 1);
    BF(16, 31,-COS1_0 , 1);
    /
    BF0( 7, 24, COS0_7 , 1);
    BF0( 8, 23, COS0_8 , 1);
    /
    BF( 7,  8, COS1_7 , 4);
    BF(23, 24,-COS1_7 , 4);
    /
    BF( 0,  7, COS2_0 , 1);
    BF( 8, 15,-COS2_0 , 1);
    BF(16, 23, COS2_0 , 1);
    BF(24, 31,-COS2_0 , 1);
    /
    BF0( 3, 28, COS0_3 , 1);
    BF0(12, 19, COS0_12, 2);
    /
    BF( 3, 12, COS1_3 , 1);
    BF(19, 28,-COS1_3 , 1);
    /
    BF0( 4, 27, COS0_4 , 1);
    BF0(11, 20, COS0_11, 2);
    /
    BF( 4, 11, COS1_4 , 1);
    BF(20, 27,-COS1_4 , 1);
    /
    BF( 3,  4, COS2_3 , 3);
    BF(11, 12,-COS2_3 , 3);
    BF(19, 20, COS2_3 , 3);
    BF(27, 28,-COS2_3 , 3);
    /
    BF( 0,  3, COS3_0 , 1);
    BF( 4,  7,-COS3_0 , 1);
    BF( 8, 11, COS3_0 , 1);
    BF(12, 15,-COS3_0 , 1);
    BF(16, 19, COS3_0 , 1);
    BF(20, 23,-COS3_0 , 1);
    BF(24, 27, COS3_0 , 1);
    BF(28, 31,-COS3_0 , 1);



    /
    BF0( 1, 30, COS0_1 , 1);
    BF0(14, 17, COS0_14, 3);
    /
    BF( 1, 14, COS1_1 , 1);
    BF(17, 30,-COS1_1 , 1);
    /
    BF0( 6, 25, COS0_6 , 1);
    BF0( 9, 22, COS0_9 , 1);
    /
    BF( 6,  9, COS1_6 , 2);
    BF(22, 25,-COS1_6 , 2);
    /
    BF( 1,  6, COS2_1 , 1);
    BF( 9, 14,-COS2_1 , 1);
    BF(17, 22, COS2_1 , 1);
    BF(25, 30,-COS2_1 , 1);

    /
    BF0( 2, 29, COS0_2 , 1);
    BF0(13, 18, COS0_13, 3);
    /
    BF( 2, 13, COS1_2 , 1);
    BF(18, 29,-COS1_2 , 1);
    /
    BF0( 5, 26, COS0_5 , 1);
    BF0(10, 21, COS0_10, 1);
    /
    BF( 5, 10, COS1_5 , 2);
    BF(21, 26,-COS1_5 , 2);
    /
    BF( 2,  5, COS2_2 , 1);
    BF(10, 13,-COS2_2 , 1);
    BF(18, 21, COS2_2 , 1);
    BF(26, 29,-COS2_2 , 1);
    /
    BF( 1,  2, COS3_1 , 2);
    BF( 5,  6,-COS3_1 , 2);
    BF( 9, 10, COS3_1 , 2);
    BF(13, 14,-COS3_1 , 2);
    BF(17, 18, COS3_1 , 2);
    BF(21, 22,-COS3_1 , 2);
    BF(25, 26, COS3_1 , 2);
    BF(29, 30,-COS3_1 , 2);

    /
    BF1( 0,  1,  2,  3);
    BF2( 4,  5,  6,  7);
    BF1( 8,  9, 10, 11);
    BF2(12, 13, 14, 15);
    BF1(16, 17, 18, 19);
    BF2(20, 21, 22, 23);
    BF1(24, 25, 26, 27);
    BF2(28, 29, 30, 31);

    /

    ADD( 8, 12);
    ADD(12, 10);
    ADD(10, 14);
    ADD(14,  9);
    ADD( 9, 13);
    ADD(13, 11);
    ADD(11, 15);

    out[ 0] = val0;
    out[16] = val1;
    out[ 8] = val2;
    out[24] = val3;
    out[ 4] = val4;
    out[20] = val5;
    out[12] = val6;
    out[28] = val7;
    out[ 2] = val8;
    out[18] = val9;
    out[10] = val10;
    out[26] = val11;
    out[ 6] = val12;
    out[22] = val13;
    out[14] = val14;
    out[30] = val15;

    ADD(24, 28);
    ADD(28, 26);
    ADD(26, 30);
    ADD(30, 25);
    ADD(25, 29);
    ADD(29, 27);
    ADD(27, 31);

    out[ 1] = val16 + val24;
    out[17] = val17 + val25;
    out[ 9] = val18 + val26;
    out[25] = val19 + val27;
    out[ 5] = val20 + val28;
    out[21] = val21 + val29;
    out[13] = val22 + val30;
    out[29] = val23 + val31;
    out[ 3] = val24 + val20;
    out[19] = val25 + val21;
    out[11] = val26 + val22;
    out[27] = val27 + val23;
    out[ 7] = val28 + val18;
    out[23] = val29 + val19;
    out[15] = val30 + val17;
    out[31] = val31;
}
