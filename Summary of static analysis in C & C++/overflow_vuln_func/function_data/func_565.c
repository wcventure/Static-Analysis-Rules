#ifdef __cplusplus
extern "C" {
#endif

static INT
AirPDcapRsnaMng(
    UCHAR *decrypt_data,
    guint mac_header_len,
    guint *decrypt_len,
    PAIRPDCAP_KEY_ITEM key,
    AIRPDCAP_SEC_ASSOCIATION *sa,
    INT offset)
{
    INT ret_value=1;
    UCHAR *try_data;
    guint try_data_len = *decrypt_len;

    if (sa->key==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "No key associated", AIRPDCAP_DEBUG_LEVEL_3);
        return AIRPDCAP_RET_REQ_DATA;
    }
    if (sa->validKey==FALSE) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "Key not yet valid", AIRPDCAP_DEBUG_LEVEL_3);
        return AIRPDCAP_RET_UNSUCCESS;
    }

    /
    try_data=(UCHAR *)g_malloc(try_data_len);

    /
    for(/; sa != NULL ;sa=sa->next) {

        if (*decrypt_len > try_data_len) {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "Invalid decryption length", AIRPDCAP_DEBUG_LEVEL_3);
            g_free(try_data);
            return AIRPDCAP_RET_UNSUCCESS;
        }

       /
       memcpy(try_data, decrypt_data, *decrypt_len);

       if (sa->wpa.key_ver==1) {
           /
           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "TKIP", AIRPDCAP_DEBUG_LEVEL_3);
           DEBUG_DUMP("ptk", sa->wpa.ptk, 64);
           DEBUG_DUMP("ptk portion used", AIRPDCAP_GET_TK(sa->wpa.ptk), 16);

           ret_value=AirPDcapTkipDecrypt(try_data+offset, *decrypt_len-offset, try_data+AIRPDCAP_TA_OFFSET, AIRPDCAP_GET_TK(sa->wpa.ptk));
           if (ret_value){
               AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "TKIP failed!", AIRPDCAP_DEBUG_LEVEL_3);
               continue;
           }

           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "TKIP DECRYPTED!!!", AIRPDCAP_DEBUG_LEVEL_3);
           /
           *decrypt_len-=12;
           break;
       } else {
           /
           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "CCMP", AIRPDCAP_DEBUG_LEVEL_3);

           ret_value=AirPDcapCcmpDecrypt(try_data, mac_header_len, (INT)*decrypt_len, AIRPDCAP_GET_TK(sa->wpa.ptk));
           if (ret_value)
              continue;

           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "CCMP DECRYPTED!!!", AIRPDCAP_DEBUG_LEVEL_3);
           /
           *decrypt_len-=8;
           break;
       }
    }
    /

    /
    if(sa == NULL) {
        g_free(try_data);
        return ret_value;
    }

    if (*decrypt_len > try_data_len || *decrypt_len < 8) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "Invalid decryption length", AIRPDCAP_DEBUG_LEVEL_3);
        g_free(try_data);
        return AIRPDCAP_RET_UNSUCCESS;
    }

    /
    memcpy(decrypt_data, try_data, *decrypt_len);
    g_free(try_data);

    /
    decrypt_data[1]&=0xBF;

    /
    offset = mac_header_len;
    *decrypt_len-=8;
    memmove(decrypt_data+offset, decrypt_data+offset+8, *decrypt_len-offset);

    if (key!=NULL) {
        memcpy(key, sa->key, sizeof(AIRPDCAP_KEY_ITEM));
        memcpy(&(key->KeyData.Wpa.Ptk), sa->wpa.ptk, AIRPDCAP_WPA_PTK_LEN); /
        if (sa->wpa.key_ver==AIRPDCAP_WPA_KEY_VER_NOT_CCMP)
            key->KeyType=AIRPDCAP_KEY_TYPE_TKIP;
        else if (sa->wpa.key_ver==AIRPDCAP_WPA_KEY_VER_AES_CCMP)
            key->KeyType=AIRPDCAP_KEY_TYPE_CCMP;
    }

    return AIRPDCAP_RET_SUCCESS;
}

static INT
AirPDcapWepMng(
    PAIRPDCAP_CONTEXT ctx,
    UCHAR *decrypt_data,
    guint mac_header_len,
    guint *decrypt_len,
    PAIRPDCAP_KEY_ITEM key,
    AIRPDCAP_SEC_ASSOCIATION *sa,
    INT offset)
{
    UCHAR wep_key[AIRPDCAP_WEP_KEY_MAXLEN+AIRPDCAP_WEP_IVLEN];
    size_t keylen;
    INT ret_value=1;
    INT key_index;
    AIRPDCAP_KEY_ITEM *tmp_key;
    UINT8 useCache=FALSE;
    UCHAR *try_data;
    guint try_data_len = *decrypt_len;

    try_data = (UCHAR *)g_malloc(try_data_len);

    if (sa->key!=NULL)
        useCache=TRUE;

    for (key_index=0; key_index<(INT)ctx->keys_nr; key_index++) {
        /
        if (!useCache) {
            tmp_key=&ctx->keys[key_index];
        } else {
            if (sa->key!=NULL && sa->key->KeyType==AIRPDCAP_KEY_TYPE_WEP) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapWepMng", "Try cached WEP key...", AIRPDCAP_DEBUG_LEVEL_3);
                tmp_key=sa->key;
            } else {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapWepMng", "Cached key is not valid, try another WEP key...", AIRPDCAP_DEBUG_LEVEL_3);
                tmp_key=&ctx->keys[key_index];
            }
        }

        /
        if (tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WEP)
        {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapWepMng", "Try WEP key...", AIRPDCAP_DEBUG_LEVEL_3);

            memset(wep_key, 0, sizeof(wep_key));
            memcpy(try_data, decrypt_data, *decrypt_len);

            /
            memcpy(wep_key, try_data+mac_header_len, AIRPDCAP_WEP_IVLEN);
            keylen=tmp_key->KeyData.Wep.WepKeyLen;
            memcpy(wep_key+AIRPDCAP_WEP_IVLEN, tmp_key->KeyData.Wep.WepKey, keylen);

            ret_value=AirPDcapWepDecrypt(wep_key,
                keylen+AIRPDCAP_WEP_IVLEN,
                try_data + (mac_header_len+AIRPDCAP_WEP_IVLEN+AIRPDCAP_WEP_KIDLEN),
                *decrypt_len-(mac_header_len+AIRPDCAP_WEP_IVLEN+AIRPDCAP_WEP_KIDLEN+AIRPDCAP_CRC_LEN));

            if (ret_value == AIRPDCAP_RET_SUCCESS)
                memcpy(decrypt_data, try_data, *decrypt_len);
        }

        if (!ret_value && tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WEP) {
            /

            sa->key=tmp_key;

            if (key!=NULL) {
                memcpy(key, &sa->key, sizeof(AIRPDCAP_KEY_ITEM));
                key->KeyType=AIRPDCAP_KEY_TYPE_WEP;
            }

            break;
        } else {
            /

            if (useCache==TRUE) {
                useCache=FALSE;
                key_index--;
            }
        }
    }

    g_free(try_data);
    if (ret_value)
        return ret_value;

    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapWepMng", "WEP DECRYPTED!!!", AIRPDCAP_DEBUG_LEVEL_3);

    /
    *decrypt_len-=4;

    if (*decrypt_len < 4) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapWepMng", "Decryption length too short", AIRPDCAP_DEBUG_LEVEL_3);
        return AIRPDCAP_RET_UNSUCCESS;
    }

    /
    decrypt_data[1]&=0xBF;

    /
    offset = mac_header_len;
    *decrypt_len-=4;
    memcpy(decrypt_data+offset, decrypt_data+offset+AIRPDCAP_WEP_IVLEN+AIRPDCAP_WEP_KIDLEN, *decrypt_len-offset);

    return AIRPDCAP_RET_SUCCESS;
}

/
static INT
AirPDcapRsna4WHandshake(
    PAIRPDCAP_CONTEXT ctx,
    const UCHAR *data,
    AIRPDCAP_SEC_ASSOCIATION *sa,
    PAIRPDCAP_KEY_ITEM key,
    INT offset)
{
    AIRPDCAP_KEY_ITEM *tmp_key, *tmp_pkt_key, pkt_key;
    AIRPDCAP_SEC_ASSOCIATION *tmp_sa;
    INT key_index;
    INT ret_value=1;
    UCHAR useCache=FALSE;
    UCHAR eapol[AIRPDCAP_EAPOL_MAX_LEN];
    USHORT eapol_len;

    if (sa->key!=NULL)
        useCache=TRUE;

    /
    if (AIRPDCAP_EAP_KEY(data[offset+1])!=1) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "Group/STAKey message (not used)", AIRPDCAP_DEBUG_LEVEL_5);
        return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
    }

    /

    /
    if(sa->handshake == 4) {
        tmp_sa= g_new(AIRPDCAP_SEC_ASSOCIATION, 1);
        memcpy(tmp_sa, sa, sizeof(AIRPDCAP_SEC_ASSOCIATION));
        sa->next=tmp_sa;
    }

    /

    /

    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake...", AIRPDCAP_DEBUG_LEVEL_5);

    /

    /
    if (AIRPDCAP_EAP_INST(data[offset+1])==0 &&
        AIRPDCAP_EAP_ACK(data[offset+1])==1 &&
        AIRPDCAP_EAP_MIC(data[offset])==0)
    {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 1", AIRPDCAP_DEBUG_LEVEL_3);

        /
        /
        /
        /

        /
        memcpy(sa->wpa.nonce, data+offset+12, 32);

        /
        sa->wpa.key_ver=AIRPDCAP_EAP_KEY_DESCR_VER(data[offset+1]);

        sa->handshake=1;

        return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
    }

    /
    if (AIRPDCAP_EAP_INST(data[offset+1])==0 &&
        AIRPDCAP_EAP_ACK(data[offset+1])==0 &&
        AIRPDCAP_EAP_MIC(data[offset])==1)
    {
        if (AIRPDCAP_EAP_SEC(data[offset])==0) {

            /
            /
            /
            if (data[offset+92]!=0 || data[offset+93]!=0) {
                /
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 2", AIRPDCAP_DEBUG_LEVEL_3);

                /
                /
                /
                /
                /

                /
                for (key_index=0; key_index<(INT)ctx->keys_nr || useCache; key_index++) {
                    /
                    if (!useCache) {
                        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "Try WPA key...", AIRPDCAP_DEBUG_LEVEL_3);
                        tmp_key=&ctx->keys[key_index];
                    } else {
                        /
                        if (sa->key!=NULL &&
                            (sa->key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PWD ||
                             sa->key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PSK ||
                             sa->key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PMK)) {
                                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "Try cached WPA key...", AIRPDCAP_DEBUG_LEVEL_3);
                                tmp_key=sa->key;
                        } else {
                            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "Cached key is of a wrong type, try WPA key...", AIRPDCAP_DEBUG_LEVEL_3);
                            tmp_key=&ctx->keys[key_index];
                        }
                    }

                    /
                    if (tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PWD ||
                        tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PSK ||
                        tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PMK)
                    {
                        if (tmp_key->KeyType == AIRPDCAP_KEY_TYPE_WPA_PWD && tmp_key->UserPwd.SsidLen == 0 && ctx->pkt_ssid_len > 0 && ctx->pkt_ssid_len <= AIRPDCAP_WPA_SSID_MAX_LEN) {
                            /
                            memcpy(&pkt_key, tmp_key, sizeof(pkt_key));
                            memcpy(&pkt_key.UserPwd.Ssid, ctx->pkt_ssid, ctx->pkt_ssid_len);
                             pkt_key.UserPwd.SsidLen = ctx->pkt_ssid_len;
                            AirPDcapRsnaPwd2Psk(pkt_key.UserPwd.Passphrase, pkt_key.UserPwd.Ssid,
                                pkt_key.UserPwd.SsidLen, pkt_key.KeyData.Wpa.Psk);
                            tmp_pkt_key = &pkt_key;
                        } else {
                            tmp_pkt_key = tmp_key;
                        }

                        /
                        AirPDcapRsnaPrfX(sa,                            /
                                         tmp_pkt_key->KeyData.Wpa.Psk,      /
                                         data+offset+12,                /
                                         512,
                                         sa->wpa.ptk);

                        /
                        eapol_len=pntoh16(data+offset-3)+4;
                        memcpy(eapol, &data[offset-5], (eapol_len<AIRPDCAP_EAPOL_MAX_LEN?eapol_len:AIRPDCAP_EAPOL_MAX_LEN));
                        ret_value=AirPDcapRsnaMicCheck(eapol,           /
                                                       eapol_len,       /
                                                       sa->wpa.ptk,     /
                                                       AIRPDCAP_EAP_KEY_DESCR_VER(data[offset+1])); /

                        /
                        /
                        /
                        /
                        /
                    }

                    if (!ret_value &&
                        (tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PWD ||
                        tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PSK ||
                        tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PMK))
                    {
                        /

                        sa->key=tmp_key;

                        if (key!=NULL) {
                            memcpy(key, &tmp_key, sizeof(AIRPDCAP_KEY_ITEM));
                            if (AIRPDCAP_EAP_KEY_DESCR_VER(data[offset+1])==AIRPDCAP_WPA_KEY_VER_NOT_CCMP)
                                key->KeyType=AIRPDCAP_KEY_TYPE_TKIP;
                            else if (AIRPDCAP_EAP_KEY_DESCR_VER(data[offset+1])==AIRPDCAP_WPA_KEY_VER_AES_CCMP)
                                key->KeyType=AIRPDCAP_KEY_TYPE_CCMP;
                        }

                        break;
                    } else {
                        /

                        if (useCache==TRUE) {
                            useCache=FALSE;
                            key_index--;
                        }
                    }
                }

                if (ret_value) {
                    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "handshake step failed", AIRPDCAP_DEBUG_LEVEL_3);
                    return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
                }

                sa->handshake=2;

                return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
            } else {
                /

                /

                /
                /
                /
                /
                /

                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 4 (patched)", AIRPDCAP_DEBUG_LEVEL_3);

                sa->handshake=4;

                sa->validKey=TRUE;

                return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
            }
            /
            /
        } else {
            /

            /

            /
            /
            /
            /
            /

            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 4", AIRPDCAP_DEBUG_LEVEL_3);

            sa->handshake=4;

            sa->validKey=TRUE;

            return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
        }
    }

    /
    if (AIRPDCAP_EAP_ACK(data[offset+1])==1 &&
        AIRPDCAP_EAP_MIC(data[offset])==1)
    {
        const EAPOL_RSN_KEY *pEAPKey;
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 3", AIRPDCAP_DEBUG_LEVEL_3);

        /
        /
        /

        /

        /
        pEAPKey = (const EAPOL_RSN_KEY *)(&(data[offset-1]));
        if (pEAPKey->type == AIRPDCAP_RSN_WPA2_KEY_DESCRIPTOR){
            PAIRPDCAP_SEC_ASSOCIATION broadcast_sa;
            AIRPDCAP_SEC_ASSOCIATION_ID id;

            /
            memcpy(id.sta, broadcast_mac, AIRPDCAP_MAC_LEN);
            memcpy(id.bssid, sa->saId.bssid, AIRPDCAP_MAC_LEN);
            broadcast_sa = AirPDcapGetSaPtr(ctx, &id);

            if (broadcast_sa == NULL){
                return AIRPDCAP_RET_UNSUCCESS;
            }
            AirPDcapDecryptWPABroadcastKey(pEAPKey, sa->wpa.ptk+16, broadcast_sa, FALSE);
        }

        return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
    }

    return AIRPDCAP_RET_UNSUCCESS;
}

static INT
AirPDcapRsnaMicCheck(
    UCHAR *eapol,
    USHORT eapol_len,
    UCHAR KCK[AIRPDCAP_WPA_KCK_LEN],
    USHORT key_ver)
{
    UCHAR mic[AIRPDCAP_WPA_MICKEY_LEN];
    UCHAR c_mic[20];  /

    /
    memcpy(mic, eapol+AIRPDCAP_WPA_MICKEY_OFFSET+4, AIRPDCAP_WPA_MICKEY_LEN);

    /
    memset(eapol+AIRPDCAP_WPA_MICKEY_OFFSET+4, 0, AIRPDCAP_WPA_MICKEY_LEN);

    if (key_ver==AIRPDCAP_WPA_KEY_VER_NOT_CCMP) {
        /
        md5_hmac(eapol, eapol_len, KCK, AIRPDCAP_WPA_KCK_LEN, c_mic);
    } else if (key_ver==AIRPDCAP_WPA_KEY_VER_AES_CCMP) {
        /
        sha1_hmac(KCK, AIRPDCAP_WPA_KCK_LEN, eapol, eapol_len, c_mic);
    } else
        /
        return AIRPDCAP_RET_UNSUCCESS;

    /
    return memcmp(mic, c_mic, AIRPDCAP_WPA_MICKEY_LEN);
}

static INT
AirPDcapValidateKey(
    PAIRPDCAP_KEY_ITEM key)
{
    size_t len;
    UCHAR ret=TRUE;
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapValidateKey");

    if (key==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapValidateKey", "NULL key", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_START("AirPDcapValidateKey");
        return FALSE;
    }

    switch (key->KeyType) {
        case AIRPDCAP_KEY_TYPE_WEP:
            /
            len=key->KeyData.Wep.WepKeyLen;
            if (len<AIRPDCAP_WEP_KEY_MINLEN || len>AIRPDCAP_WEP_KEY_MAXLEN) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapValidateKey", "WEP key: key length not accepted", AIRPDCAP_DEBUG_LEVEL_5);
                ret=FALSE;
            }
            break;

        case AIRPDCAP_KEY_TYPE_WEP_40:
            /
            key->KeyData.Wep.WepKeyLen=AIRPDCAP_WEP_40_KEY_LEN;
            key->KeyType=AIRPDCAP_KEY_TYPE_WEP;
            break;

        case AIRPDCAP_KEY_TYPE_WEP_104:
            /
            key->KeyData.Wep.WepKeyLen=AIRPDCAP_WEP_104_KEY_LEN;
            key->KeyType=AIRPDCAP_KEY_TYPE_WEP;
            break;

        case AIRPDCAP_KEY_TYPE_WPA_PWD:
            /
            len=strlen(key->UserPwd.Passphrase);
            if (len<AIRPDCAP_WPA_PASSPHRASE_MIN_LEN || len>AIRPDCAP_WPA_PASSPHRASE_MAX_LEN) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapValidateKey", "WPA-PWD key: passphrase length not accepted", AIRPDCAP_DEBUG_LEVEL_5);
                ret=FALSE;
            }

            len=key->UserPwd.SsidLen;
            if (len>AIRPDCAP_WPA_SSID_MAX_LEN) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapValidateKey", "WPA-PWD key: ssid length not accepted", AIRPDCAP_DEBUG_LEVEL_5);
                ret=FALSE;
            }

            break;

        case AIRPDCAP_KEY_TYPE_WPA_PSK:
            break;

        case AIRPDCAP_KEY_TYPE_WPA_PMK:
            break;

        default:
            ret=FALSE;
    }

    AIRPDCAP_DEBUG_TRACE_END("AirPDcapValidateKey");
    return ret;
}

static INT
AirPDcapGetSa(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_SEC_ASSOCIATION_ID *id)
{
    INT sa_index;

    if (ctx->sa_index!=-1) {
        /
        /
        for (sa_index=ctx->sa_index; sa_index>=0; sa_index--) {
            if (ctx->sa[sa_index].used) {
                if (memcmp(id, &(ctx->sa[sa_index].saId), sizeof(AIRPDCAP_SEC_ASSOCIATION_ID))==0) {
                    ctx->index=sa_index;
                    return sa_index;
                }
            }
        }
    }

    return -1;
}

static INT
AirPDcapStoreSa(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_SEC_ASSOCIATION_ID *id)
{
    INT last_free;

    if (ctx->sa[ctx->first_free_index].used) {
        /
        /
        /
        for (last_free=ctx->first_free_index; last_free<AIRPDCAP_MAX_SEC_ASSOCIATIONS_NR; last_free++)
            if (!ctx->sa[last_free].used)
                break;

        if (last_free>=AIRPDCAP_MAX_SEC_ASSOCIATIONS_NR) {
            /
            return -1;
        }

        /
        ctx->first_free_index=last_free;
    }

    /
    ctx->index=ctx->first_free_index;

    /
    memset(ctx->sa+ctx->index, 0, sizeof(AIRPDCAP_SEC_ASSOCIATION));

    ctx->sa[ctx->index].used=1;

    /
    memcpy(&(ctx->sa[ctx->index].saId), id, sizeof(AIRPDCAP_SEC_ASSOCIATION_ID));

    /
    ctx->first_free_index++;

    /
    if (ctx->index > ctx->sa_index)
        ctx->sa_index=ctx->index;

    return ctx->index;
}

/

static const UCHAR *
AirPDcapGetStaAddress(
    const AIRPDCAP_MAC_FRAME_ADDR4 *frame)
{
    switch(AIRPDCAP_DS_BITS(frame->fc[1])) { /
        case 0:
        case 1:
            return frame->addr2;
        case 2:
            return frame->addr1;
        case 3:
            if (memcmp(frame->addr1, frame->addr2, AIRPDCAP_MAC_LEN) < 0)
                return frame->addr1;
            else
                return frame->addr2;

        default:
            return NULL;
    }
}

static const UCHAR *
AirPDcapGetBssidAddress(
    const AIRPDCAP_MAC_FRAME_ADDR4 *frame)
{
    switch(AIRPDCAP_DS_BITS(frame->fc[1])) { /
        case 0:
            return frame->addr3;
        case 1:
            return frame->addr1;
        case 2:
            return frame->addr2;
        case 3:
            if (memcmp(frame->addr1, frame->addr2, AIRPDCAP_MAC_LEN) > 0)
                return frame->addr1;
            else
                return frame->addr2;

        default:
            return NULL;
    }
}

/
static void
AirPDcapRsnaPrfX(
    AIRPDCAP_SEC_ASSOCIATION *sa,
    const UCHAR pmk[32],
    const UCHAR snonce[32],
    const INT x,        /
    UCHAR *ptk)
{
    UINT8 i;
    UCHAR R[100];
    INT offset=sizeof("Pairwise key expansion");
    UCHAR output[80]; /

    memset(R, 0, 100);

    memcpy(R, "Pairwise key expansion", offset);

    /
    if (memcmp(sa->saId.sta, sa->saId.bssid, AIRPDCAP_MAC_LEN) < 0)
    {
        memcpy(R + offset, sa->saId.sta, AIRPDCAP_MAC_LEN);
        memcpy(R + offset+AIRPDCAP_MAC_LEN, sa->saId.bssid, AIRPDCAP_MAC_LEN);
    }
    else
    {
        memcpy(R + offset, sa->saId.bssid, AIRPDCAP_MAC_LEN);
        memcpy(R + offset+AIRPDCAP_MAC_LEN, sa->saId.sta, AIRPDCAP_MAC_LEN);
    }

    offset+=AIRPDCAP_MAC_LEN*2;

    /
    if( memcmp(snonce, sa->wpa.nonce, 32) < 0 )
    {
        memcpy(R + offset, snonce, 32);
        memcpy(R + offset + 32, sa->wpa.nonce, 32);
    }
    else
    {
        memcpy(R + offset, sa->wpa.nonce, 32);
        memcpy(R + offset + 32, snonce, 32);
    }

    offset+=32*2;

    for(i = 0; i < (x+159)/160; i++)
    {
        R[offset] = i;
        sha1_hmac(pmk, 32, R, 100, &output[20 * i]);
    }
    memcpy(ptk, output, x/8);
}

static INT
AirPDcapRsnaPwd2PskStep(
    const guint8 *ppBytes,
    const guint ppLength,
    const CHAR *ssid,
    const size_t ssidLength,
    const INT iterations,
    const INT count,
    UCHAR *output)
{
    UCHAR digest[64], digest1[64];
    INT i, j;

    if (ssidLength+4 > 36)
        return AIRPDCAP_RET_UNSUCCESS;

    memset(digest, 0, 64);
    memset(digest1, 0, 64);

    /
    memcpy(digest, ssid, ssidLength);
    digest[ssidLength] = (UCHAR)((count>>24) & 0xff);
    digest[ssidLength+1] = (UCHAR)((count>>16) & 0xff);
    digest[ssidLength+2] = (UCHAR)((count>>8) & 0xff);
    digest[ssidLength+3] = (UCHAR)(count & 0xff);
    sha1_hmac(ppBytes, ppLength, digest, (guint32) ssidLength+4, digest1);

    /
    memcpy(output, digest1, AIRPDCAP_SHA_DIGEST_LEN);
    for (i = 1; i < iterations; i++) {
        /
        sha1_hmac(ppBytes, ppLength, digest1, AIRPDCAP_SHA_DIGEST_LEN, digest);

        memcpy(digest1, digest, AIRPDCAP_SHA_DIGEST_LEN);
        /
        for (j = 0; j < AIRPDCAP_SHA_DIGEST_LEN; j++) {
            output[j] ^= digest[j];
        }
    }

    return AIRPDCAP_RET_SUCCESS;
}

static INT
AirPDcapRsnaPwd2Psk(
    const CHAR *passphrase,
    const CHAR *ssid,
    const size_t ssidLength,
    UCHAR *output)
{
    UCHAR m_output[2*AIRPDCAP_SHA_DIGEST_LEN];
    GByteArray *pp_ba = g_byte_array_new();

    memset(m_output, 0, 2*AIRPDCAP_SHA_DIGEST_LEN);

    if (!uri_str_to_bytes(passphrase, pp_ba)) {
        g_byte_array_free(pp_ba, TRUE);
        return 0;
    }

    AirPDcapRsnaPwd2PskStep(pp_ba->data, pp_ba->len, ssid, ssidLength, 4096, 1, m_output);
    AirPDcapRsnaPwd2PskStep(pp_ba->data, pp_ba->len, ssid, ssidLength, 4096, 2, &m_output[AIRPDCAP_SHA_DIGEST_LEN]);

    memcpy(output, m_output, AIRPDCAP_WPA_PSK_LEN);
    g_byte_array_free(pp_ba, TRUE);

    return 0;
}

/
decryption_key_t*
parse_key_string(gchar* input_string, guint8 key_type)
{
    gchar *key, *tmp_str;
    gchar *ssid;

    GString    *key_string = NULL;
    GByteArray *ssid_ba = NULL, *key_ba;
    gboolean    res;

    gchar **tokens;
    guint n = 0;
    decryption_key_t *dk;

    if(input_string == NULL)
        return NULL;

    /

    switch(key_type)
    {
    case AIRPDCAP_KEY_TYPE_WEP:
    case AIRPDCAP_KEY_TYPE_WEP_40:
    case AIRPDCAP_KEY_TYPE_WEP_104:

       key_ba = g_byte_array_new();
       res = hex_str_to_bytes(input_string, key_ba, FALSE);

       if (res && key_ba->len > 0) {
           /
           /
           dk = (decryption_key_t *)g_malloc(sizeof(decryption_key_t));

           dk->type = AIRPDCAP_KEY_TYPE_WEP;
           /
           tmp_str = bytes_to_str(NULL, key_ba->data, key_ba->len);
           dk->key  = g_string_new(tmp_str);
           g_string_ascii_down(dk->key);
           dk->bits = key_ba->len * 8;
           dk->ssid = NULL;

           wmem_free(NULL, tmp_str);
           g_byte_array_free(key_ba, TRUE);
           return dk;
       }

       /
       g_byte_array_free(key_ba, TRUE);
       return NULL;

    case AIRPDCAP_KEY_TYPE_WPA_PWD:

        tokens = g_strsplit(input_string,":",0);

        /
        while(tokens[n] != NULL)
            n++;

        if(n < 1)
        {
            /
            g_strfreev(tokens);
            return NULL;
        }

        /
        key = g_strdup(tokens[0]);

        ssid = NULL;
        /
        if(n >= 2)
        {
           ssid = g_strdup(tokens[1]);
        }

        /
        key_string = g_string_new(key);
        ssid_ba = NULL;

        /
        if( ((key_string->len) > WPA_KEY_MAX_CHAR_SIZE) || ((key_string->len) < WPA_KEY_MIN_CHAR_SIZE))
        {
            g_string_free(key_string, TRUE);

            g_free(key);
            g_free(ssid);

            /
            g_strfreev(tokens);
            return NULL;
        }

        if(ssid != NULL) /
        {
            ssid_ba = g_byte_array_new();
            if (! uri_str_to_bytes(ssid, ssid_ba)) {
                g_string_free(key_string, TRUE);
                g_byte_array_free(ssid_ba, TRUE);
                g_free(key);
                g_free(ssid);
                /
                g_strfreev(tokens);
                return NULL;
            }

            if(ssid_ba->len > WPA_SSID_MAX_CHAR_SIZE)
            {
                g_string_free(key_string, TRUE);
                g_byte_array_free(ssid_ba, TRUE);

                g_free(key);
                g_free(ssid);

                /
                g_strfreev(tokens);
                return NULL;
            }
        }

        /
        dk = (decryption_key_t*)g_malloc(sizeof(decryption_key_t));

        dk->type = AIRPDCAP_KEY_TYPE_WPA_PWD;
        dk->key  = g_string_new(key);
        dk->bits = 256; /
        dk->ssid = byte_array_dup(ssid_ba); /

        g_string_free(key_string, TRUE);
        if (ssid_ba != NULL)
            g_byte_array_free(ssid_ba, TRUE);

        g_free(key);
        if(ssid != NULL)
            g_free(ssid);

        /
        g_strfreev(tokens);
        return dk;

    case AIRPDCAP_KEY_TYPE_WPA_PSK:

        key_ba = g_byte_array_new();
        res = hex_str_to_bytes(input_string, key_ba, FALSE);

        /
        if(!res || ((key_ba->len) != WPA_PSK_KEY_SIZE))
        {
            g_byte_array_free(key_ba, TRUE);

            /
            return NULL;
        }

        /
        dk = (decryption_key_t*)g_malloc(sizeof(decryption_key_t));

        dk->type = AIRPDCAP_KEY_TYPE_WPA_PSK;
        dk->key  = g_string_new(input_string);
        dk->bits = (guint) dk->key->len * 4;
        dk->ssid = NULL;

        g_byte_array_free(key_ba, TRUE);
        return dk;
    }

    /
    return NULL;
}

/
gchar*
get_key_string(decryption_key_t* dk)
{
    gchar* output_string = NULL;

    if(dk == NULL || dk->key == NULL)
        return NULL;

    switch(dk->type) {
        case AIRPDCAP_KEY_TYPE_WEP:
            output_string = g_strdup(dk->key->str);
            break;
        case AIRPDCAP_KEY_TYPE_WPA_PWD:
            if(dk->ssid == NULL)
                output_string = g_strdup(dk->key->str);
            else
                output_string = g_strdup_printf("%s:%s",
                    dk->key->str, format_uri(dk->ssid, ":"));
            break;
        case AIRPDCAP_KEY_TYPE_WPA_PMK:
            output_string = g_strdup(dk->key->str);
            break;
        default:
            return NULL;
    }

    return output_string;
}

#ifdef __cplusplus
}
