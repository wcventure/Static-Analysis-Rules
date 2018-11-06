static void mc_block(uint8_t *dst, uint8_t *src, uint8_t *tmp, int stride, int b_w, int b_h, int dx, int dy){
    int x, y;
START_TIMER
    for(y=0; y < b_h+5; y++){
        for(x=0; x < b_w; x++){
            int a0= src[x    ];
            int a1= src[x + 1];
            int a2= src[x + 2];
            int a3= src[x + 3];
            int a4= src[x + 4];
            int a5= src[x + 5];
//            int am= 9*(a1+a2) - (a0+a3);
            int am= 20*(a2+a3) - 5*(a1+a4) + (a0+a5);
//            int am= 18*(a2+a3) - 2*(a1+a4);
//             int aL= (-7*a0 + 105*a1 + 35*a2 - 5*a3)>>3;
//             int aR= (-7*a3 + 105*a2 + 35*a1 - 5*a0)>>3;

//            if(b_w==16) am= 8*(a1+a2);

            if(dx<8) tmp[x]= (32*a2*( 8-dx) +    am* dx    + 128)>>8;
            else     tmp[x]= (   am*(16-dx) + 32*a3*(dx-8) + 128)>>8;

/
        }
        tmp += stride;
        src += stride;
    }
    tmp -= (b_h+5)*stride;
    
    for(y=0; y < b_h; y++){
        for(x=0; x < b_w; x++){
            int a0= tmp[x + 0*stride];
            int a1= tmp[x + 1*stride];
            int a2= tmp[x + 2*stride];
            int a3= tmp[x + 3*stride];
            int a4= tmp[x + 4*stride];
            int a5= tmp[x + 5*stride];
            int am= 20*(a2+a3) - 5*(a1+a4) + (a0+a5);
//            int am= 18*(a2+a3) - 2*(a1+a4);
/
            
//            if(b_w==16) am= 8*(a1+a2);

            if(dy<8) dst[x]= (32*a2*( 8-dy) +    am* dy    + 128)>>8;
            else     dst[x]= (   am*(16-dy) + 32*a3*(dy-8) + 128)>>8;

/
        }
        dst += stride;
        tmp += stride;
    }
STOP_TIMER("mc_block")
}
