static int check_control_digest(l2tpv3_tunnel_t *tunnel,
                                tvbuff_t *tvb,
                                int length,
                                int idx,
                                int avp_len,
                                int msg_type,
                                packet_info *pinfo)
{
    guint8 digest[L2TP_HMAC_SHA1_DIGEST_LEN];

    if (!tunnel)
        return 1;

    update_shared_key(tunnel);

    switch (tvb_get_guint8(tvb, idx)) {
        case L2TP_HMAC_MD5:
            if ((avp_len - 1) != L2TP_HMAC_MD5_DIGEST_LEN)
                return -1;
            md5_hmac_digest(tunnel, tvb, length, idx, avp_len, msg_type, pinfo, digest);
            break;
        case L2TP_HMAC_SHA1:
            if ((avp_len - 1) != L2TP_HMAC_SHA1_DIGEST_LEN)
                return -1;
            sha1_hmac_digest(tunnel, tvb, length, idx, avp_len, msg_type, pinfo, digest);
            break;
        default:
            return 1;
            break;
    }

    return tvb_memeql(tvb, idx + 1, digest, avp_len - 1);
}
