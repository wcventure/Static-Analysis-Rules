static int av_dict_set_fxp(AVDictionary **pm, const char *key, uint64_t value, unsigned int digits,
                int flags)
{
    char valuestr[44];
    snprintf(valuestr, sizeof(valuestr), "%"PRId64".%0*"PRId64,
             value / PRECISION, digits, ( value % PRECISION ) / ( PRECISION / uintpow(10,digits) ));
    return av_dict_set(pm, key, valuestr, flags);
}
