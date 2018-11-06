static void imdct12(int *out, int *in)
{
    int in0, in1, in2, in3, in4, in5, t1, t2;
    in0= in[0*3]<<5;
    in1= (in[1*3] + in[0*3])<<5;
    in2= (in[2*3] + in[1*3])<<5;
    in3= (in[3*3] + in[2*3])<<5;
    in4= (in[4*3] + in[3*3])<<5;
    in5= (in[5*3] + in[4*3])<<5;
    in5 += in3;
    in3 += in1;

    in2= MULH(2*in2, C3);
    in3= MULH(2*in3, C3);
    
    t1 = in0 - in4;
    t2 = MULL(in1 - in5, icos36[4]);

    out[ 7]= 
    out[10]= t1 + t2;
    out[ 1]=
    out[ 4]= t1 - t2;

    in0 += in4>>1;
    in4 = in0 + in2;
    in1 += in5>>1;
    in5 = MULL(in1 + in3, icos36[1]);    
    out[ 8]= 
    out[ 9]= in4 + in5;
    out[ 2]=
    out[ 3]= in4 - in5;
    
    in0 -= in2;
    in1 = MULL(in1 - in3, icos36[7]);
    out[ 0]=
    out[ 5]= in0 - in1;
    out[ 6]=
    out[11]= in0 + in1;    
}
