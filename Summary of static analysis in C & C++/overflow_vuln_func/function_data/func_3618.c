int av_reduce(int *dst_nom, int *dst_den, int64_t nom, int64_t den, int64_t max){
    AVRational a0={0,1}, a1={1,0};
    int sign= (nom<0) ^ (den<0);
    int64_t gcd= ff_gcd(FFABS(nom), FFABS(den));

    nom = FFABS(nom)/gcd;
    den = FFABS(den)/gcd;
    if(nom<=max && den<=max){
        a1= (AVRational){nom, den};
        den=0;
    }

    while(den){
        int64_t x       = nom / den;
        int64_t next_den= nom - den*x;
        int64_t a2n= x*a1.num + a0.num;
        int64_t a2d= x*a1.den + a0.den;

        if(a2n > max || a2d > max){
            if(a1.num) x= (max - a0.num) / a1.num;
            if(a1.den) x= FFMIN(x, (max - a0.den) / a1.den);

            // Won't overflow, sum == original denominator
            if (den*(2*x*a1.den + a0.den) > nom*a1.den)
                a1 = (AVRational){x*a1.num + a0.num, x*a1.den + a0.den};
            break;
        }

        a0= a1;
        a1= (AVRational){a2n, a2d};
        nom= den;
        den= next_den;
    }
    assert(ff_gcd(a1.num, a1.den) == 1);

    *dst_nom = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den==0;
}
