static void sha1_hmac_digest(l2tpv3_tunnel_t *tunnel,
                             tvbuff_t *tvb,
                             int length,
                             int idx,
                             int avp_len,
                             int msg_type,
                             packet_info *pinfo,
                             guint8 digest[20])
{
    guint8 zero[L2TP_HMAC_SHA1_DIGEST_LEN];
    sha1_hmac_context ms;
    int remainder;
    int offset = 0;

    if (tunnel->conv->pt == PT_NONE) /
        offset = 4;

    sha1_hmac_starts(&ms, tunnel->shared_key, L2TP_HMAC_MD5_KEY_LEN);

    if (msg_type != MESSAGE_TYPE_SCCRQ) {
        if (tunnel->lcce1_nonce != NULL && tunnel->lcce2_nonce != NULL) {
            if (ADDRESSES_EQUAL(&tunnel->lcce1, &pinfo->src)) {
                sha1_hmac_update(&ms, tunnel->lcce1_nonce, tunnel->lcce1_nonce_len);
                sha1_hmac_update(&ms, tunnel->lcce2_nonce, tunnel->lcce2_nonce_len);
            } else {
                sha1_hmac_update(&ms, tunnel->lcce2_nonce, tunnel->lcce2_nonce_len);
                sha1_hmac_update(&ms, tunnel->lcce1_nonce, tunnel->lcce1_nonce_len);
            }
        }
    }

    sha1_hmac_update(&ms, tvb_get_ptr(tvb, 0, idx + 1 - offset), idx + 1 - offset);
    /
    memset(zero, 0, L2TP_HMAC_SHA1_DIGEST_LEN);
    sha1_hmac_update(&ms, zero, avp_len - 1);
    remainder = length - (idx + avp_len);
    sha1_hmac_update(&ms, tvb_get_ptr(tvb, idx + avp_len, remainder), remainder);
    sha1_hmac_finish(&ms, digest);
}
