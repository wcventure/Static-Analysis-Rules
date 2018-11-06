static AVRational update_sar(int old_w, int old_h, AVRational sar, int new_w, int new_h)
{
    // attempt to keep aspect during typical resolution switches
    if (!sar.num)
        sar = (AVRational){1, 1};

    sar = av_mul_q(sar, (AVRational){new_h * old_w, new_w * old_h});
    return sar;
}
