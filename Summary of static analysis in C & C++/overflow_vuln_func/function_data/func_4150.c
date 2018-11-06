void ff_cavs_mv(AVSContext *h, enum cavs_mv_loc nP, enum cavs_mv_loc nC,
                enum cavs_mv_pred mode, enum cavs_block size, int ref)
{
    cavs_vector *mvP = &h->mv[nP];
    cavs_vector *mvA = &h->mv[nP-1];
    cavs_vector *mvB = &h->mv[nP-4];
    cavs_vector *mvC = &h->mv[nC];
    const cavs_vector *mvP2 = NULL;

    mvP->ref  = ref;
    mvP->dist = h->dist[mvP->ref];
    if (mvC->ref == NOT_AVAIL || (nP == MV_FWD_X3) || (nP == MV_BWD_X3 ))
        mvC = &h->mv[nP - 5];  // set to top-left (mvD)
    if (mode == MV_PRED_PSKIP &&
        (mvA->ref == NOT_AVAIL ||
         mvB->ref == NOT_AVAIL ||
         (mvA->x | mvA->y | mvA->ref) == 0 ||
         (mvB->x | mvB->y | mvB->ref) == 0)) {
        mvP2 = &un_mv;
    /
    } else if (mvA->ref >= 0 && mvB->ref < 0  && mvC->ref < 0) {
        mvP2 = mvA;
    } else if (mvA->ref < 0  && mvB->ref >= 0 && mvC->ref < 0) {
        mvP2 = mvB;
    } else if (mvA->ref < 0  && mvB->ref < 0  && mvC->ref >= 0) {
        mvP2 = mvC;
    } else if (mode == MV_PRED_LEFT     && mvA->ref == ref) {
        mvP2 = mvA;
    } else if (mode == MV_PRED_TOP      && mvB->ref == ref) {
        mvP2 = mvB;
    } else if (mode == MV_PRED_TOPRIGHT && mvC->ref == ref) {
        mvP2 = mvC;
    }
    if (mvP2) {
        mvP->x = mvP2->x;
        mvP->y = mvP2->y;
    } else
        mv_pred_median(h, mvP, mvA, mvB, mvC);

    if (mode < MV_PRED_PSKIP) {
        mvP->x += get_se_golomb(&h->gb);
        mvP->y += get_se_golomb(&h->gb);
    }
    set_mvs(mvP, size);
}
