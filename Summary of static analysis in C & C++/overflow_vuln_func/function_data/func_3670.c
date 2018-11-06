static void RENAME(mix8to2)(SAMPLE **out, const SAMPLE **in, COEFF *coeffp, integer len){
    int i;

    for(i=0; i<len; i++) {
        INTER t = in[2][i]*coeffp[0*8+2] + in[3][i]*coeffp[0*8+3];
        out[0][i] = R(t + in[0][i]*(INTER)coeffp[0*8+0] + in[4][i]*(INTER)coeffp[0*8+4] + in[6][i]*(INTER)coeffp[0*8+6]);
        out[1][i] = R(t + in[1][i]*(INTER)coeffp[1*8+1] + in[5][i]*(INTER)coeffp[1*8+5] + in[7][i]*(INTER)coeffp[1*8+7]);
    }
}
