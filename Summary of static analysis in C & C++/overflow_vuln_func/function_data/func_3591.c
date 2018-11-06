static void RENAME(mix6to2)(SAMPLE **out, const SAMPLE **in, COEFF *coeffp, integer len){
    int i;

    for(i=0; i<len; i++) {
        INTER t = in[2][i]*coeffp[0*6+2] + in[3][i]*coeffp[0*6+3];
        out[0][i] = R(t + in[0][i]*(INTER)coeffp[0*6+0] + in[4][i]*(INTER)coeffp[0*6+4]);
        out[1][i] = R(t + in[1][i]*(INTER)coeffp[1*6+1] + in[5][i]*(INTER)coeffp[1*6+5]);
    }
}
