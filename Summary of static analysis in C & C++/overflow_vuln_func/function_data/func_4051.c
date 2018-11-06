#define TM2_RECALC_BLOCK(CHR, stride, last, CD) {\
    CD[0] = CHR[1] - last[1];\
    CD[1] = (int)CHR[stride + 1] - (int)CHR[1];\
    last[0] = (int)CHR[stride + 0];\
    last[1] = (int)CHR[stride + 1];}
