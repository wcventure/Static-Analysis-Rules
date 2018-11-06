static void rv34_pred_4x4_block(RV34DecContext *r, uint8_t *dst, int stride, int itype, int up, int left, int down, int right)
{
    uint8_t *prev = dst - stride + 4;
    uint32_t topleft;

    if(!up && !left)
        itype = DC_128_PRED;
    else if(!up){
        if(itype == VERT_PRED) itype = HOR_PRED;
        if(itype == DC_PRED)   itype = LEFT_DC_PRED;
    }else if(!left){
        if(itype == HOR_PRED)  itype = VERT_PRED;
        if(itype == DC_PRED)   itype = TOP_DC_PRED;
        if(itype == DIAG_DOWN_LEFT_PRED) itype = DIAG_DOWN_LEFT_PRED_RV40_NODOWN;
    }
    if(!down){
        if(itype == DIAG_DOWN_LEFT_PRED) itype = DIAG_DOWN_LEFT_PRED_RV40_NODOWN;
        if(itype == HOR_UP_PRED) itype = HOR_UP_PRED_RV40_NODOWN;
        if(itype == VERT_LEFT_PRED) itype = VERT_LEFT_PRED_RV40_NODOWN;
    }
    if(!right && up){
        topleft = dst[-stride + 3] * 0x01010101;
        prev = (uint8_t*)&topleft;
    }
    r->h.pred4x4[itype](dst, prev, stride);
}
