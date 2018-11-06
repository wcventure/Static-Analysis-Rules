#ifdef  __cplusplus
extern "C" {
#endif

const guint8 broadcast_mac[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


/
typedef struct {
    guint8 bElementID;
    guint8 bLength;
    guint8  OUI[4];
    guint16 iVersion;
    guint8  multicastOUI[4];
    guint16 iUnicastCount;      /
    guint8  unicastOUI[4];
    guint16 iAuthCount;         /
    guint8  authOUI[4];
    guint16 iWPAcap;
} RSN_IE;

#define EAPKEY_MIC_LEN  16  /
#define NONCE_LEN 32

#define TKIP_GROUP_KEY_LEN 32
#define CCMP_GROUP_KEY_LEN 16
/
#define TKIP_GROUP_KEYBYTES_LEN ( sizeof(RSN_IE) + 8 + TKIP_GROUP_KEY_LEN + 6 ) /
/
#define TKIP_GROUP_KEYBYTES_LEN_MAX ( TKIP_GROUP_KEYBYTES_LEN + 28 )
/
#define TKIP_GROUP_KEYBYTES_LEN_GKEY (8 + 8 + TKIP_GROUP_KEY_LEN ) /
/
#define CCMP_GROUP_KEYBYTES_LEN ( sizeof(RSN_IE) + 8 + CCMP_GROUP_KEY_LEN + 6 ) /
typedef struct {
    guint8  type;
    guint8  key_information[2];  /
    guint8  key_length[2];  /
    guint8  replay_counter[8];
    guint8  key_nonce[NONCE_LEN];
    guint8  key_iv[16];
    guint8  key_sequence_counter[8];  /
    guint8  key_id[8];
    guint8  key_mic[EAPKEY_MIC_LEN];
    guint8  key_data_len[2];  /
    guint8  ie[TKIP_GROUP_KEYBYTES_LEN_MAX]; /
} EAPOL_RSN_KEY,  * P_EAPOL_RSN_KEY;
#define RSN_KEY_WITHOUT_KEYBYTES_LEN sizeof(EAPOL_RSN_KEY)-TKIP_GROUP_KEYBYTES_LEN_MAX
/
#define GROUP_KEY_PAYLOAD_LEN_MIN RSN_KEY_WITHOUT_KEYBYTES_LEN+CCMP_GROUP_KEY_LEN

/


static void
AirPDcapDecryptWPABroadcastKey(const EAPOL_RSN_KEY *pEAPKey, guint8  *decryption_key, PAIRPDCAP_SEC_ASSOCIATION sa, gboolean group_hshake)
{
    guint8  new_key[32];
    guint8 key_version;
    guint8  *szEncryptedKey;
    guint16 key_len = 0;
    static AIRPDCAP_KEY_ITEM dummy_key; /

    /

    /
    /
    key_version = AIRPDCAP_EAP_KEY_DESCR_VER(pEAPKey->key_information[1]);
    if (key_version == AIRPDCAP_WPA_KEY_VER_NOT_CCMP){
        /
        key_len = pntoh16(pEAPKey->key_length);
    }else if (key_version == AIRPDCAP_WPA_KEY_VER_AES_CCMP){
        /
        key_len = pntoh16(pEAPKey->key_data_len);
    }

    if (key_len > TKIP_GROUP_KEYBYTES_LEN_MAX || key_len == 0) { /
        return;
    }

    /
    szEncryptedKey = (guint8 *)g_memdup(pEAPKey->ie, key_len);

    DEBUG_DUMP("Encrypted Broadcast key:", szEncryptedKey, key_len);
    DEBUG_DUMP("KeyIV:", pEAPKey->key_iv, 16);
    DEBUG_DUMP("decryption_key:", decryption_key, 16);

    /
    memcpy(new_key, pEAPKey->key_iv, 16);
    memcpy(new_key+16, decryption_key, 16);
    DEBUG_DUMP("FullDecrKey:", new_key, 32);

    /
    /
    /
    /
    /
    /

    if (key_version == AIRPDCAP_WPA_KEY_VER_NOT_CCMP){
        guint8 dummy[256];
        /
        /
        /

        rc4_state_struct rc4_state;

        /
        /
        sa->wpa.key_ver = (key_len >=TKIP_GROUP_KEY_LEN)?AIRPDCAP_WPA_KEY_VER_NOT_CCMP:AIRPDCAP_WPA_KEY_VER_AES_CCMP;

        crypt_rc4_init(&rc4_state, new_key, sizeof(new_key));

        /
        crypt_rc4(&rc4_state, dummy, 256);
        crypt_rc4(&rc4_state, szEncryptedKey, key_len);

    } else if (key_version == AIRPDCAP_WPA_KEY_VER_AES_CCMP){
        /

        guint8 key_found;
        guint16 key_index;
        guint8 *decrypted_data;

        /
        /
        if (group_hshake)
            sa->wpa.key_ver = (key_len >= (TKIP_GROUP_KEYBYTES_LEN_GKEY))?AIRPDCAP_WPA_KEY_VER_NOT_CCMP:AIRPDCAP_WPA_KEY_VER_AES_CCMP;
        else
            sa->wpa.key_ver = (key_len >= (TKIP_GROUP_KEYBYTES_LEN))?AIRPDCAP_WPA_KEY_VER_NOT_CCMP:AIRPDCAP_WPA_KEY_VER_AES_CCMP;

        /
        decrypted_data = (guint8 *) g_malloc(key_len);

        AES_unwrap(decryption_key, 16, szEncryptedKey,  key_len, decrypted_data);

        /

        key_found = FALSE;
        key_index = 0;
        while(key_index < key_len && !key_found){
            guint8 rsn_id;

            /
            rsn_id = decrypted_data[key_index];

            if (rsn_id != 0xdd){
                key_index += decrypted_data[key_index+1]+2;
            }else{
                key_found = TRUE;
            }
        }

        if (key_found){
            /
            memcpy(szEncryptedKey, decrypted_data+key_index+8, key_len-key_index-8);
        }

        g_free(decrypted_data);
    }

    /
    DEBUG_DUMP("Broadcast key:", szEncryptedKey, key_len);

    /
    sa->key = &dummy_key;  /
    sa->validKey = TRUE;

    /
    /
    memset(sa->wpa.ptk, 0, sizeof(sa->wpa.ptk));
    memcpy(sa->wpa.ptk+32, szEncryptedKey, key_len);
    g_free(szEncryptedKey);
}


/
static PAIRPDCAP_SEC_ASSOCIATION
AirPDcapGetSaPtr(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_SEC_ASSOCIATION_ID *id)
{
    int sa_index;

    /
    if ((sa_index=AirPDcapGetSa(ctx, id))==-1) {
        /
        if ((sa_index=AirPDcapStoreSa(ctx, id))==-1) {
            return NULL;
        }
    }
    /
    return &ctx->sa[sa_index];
}

static INT AirPDcapScanForGroupKey(
    PAIRPDCAP_CONTEXT ctx,
    const guint8 *data,
    const guint mac_header_len,
    const guint tot_len
)
{
    const UCHAR *addr;
    AIRPDCAP_SEC_ASSOCIATION_ID id;
    guint bodyLength;
    PAIRPDCAP_SEC_ASSOCIATION sta_sa;
    PAIRPDCAP_SEC_ASSOCIATION sa;
    int offset = 0;
    const guint8 dot1x_header[] = {
        0xAA,             /
        0xAA,             /
        0x03,             /
        0x00, 0x00, 0x00, /
        0x88, 0x8E        /
    };

    const EAPOL_RSN_KEY *pEAPKey;
#ifdef _DEBUG
    CHAR msgbuf[255];
#endif
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapScanForGroupKey");

    if (mac_header_len + GROUP_KEY_PAYLOAD_LEN_MIN > tot_len) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "Message too short", AIRPDCAP_DEBUG_LEVEL_3);
        return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
    }

    /
    offset = mac_header_len;

    /
    if (memcmp(data+offset, dot1x_header, 8) == 0) {

        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "Authentication: EAPOL packet", AIRPDCAP_DEBUG_LEVEL_3);

        /
        offset+=8;


        /
        if (data[offset+1]!=3) {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "Not EAPOL-Key", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /
        bodyLength=pntoh16(data+offset+2);
        if ((tot_len-offset-4) > bodyLength) {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "EAPOL body too short", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /
        offset+=4;

        pEAPKey = (const EAPOL_RSN_KEY *) (data+offset);

        /
        if (/ /
            pEAPKey->type != AIRPDCAP_RSN_WPA2_KEY_DESCRIPTOR &&             /
            pEAPKey->type != AIRPDCAP_RSN_WPA_KEY_DESCRIPTOR)           /
        {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "Not valid key descriptor type", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /
        offset+=1;

        /
        if (AIRPDCAP_EAP_KEY(data[offset+1])!=0 ||
            AIRPDCAP_EAP_ACK(data[offset+1])!=1 ||
            AIRPDCAP_EAP_MIC(data[offset]) != 1 ||
            AIRPDCAP_EAP_SEC(data[offset]) != 1){

            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "Key bitfields not correct", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /
        if ( (addr=AirPDcapGetBssidAddress((const AIRPDCAP_MAC_FRAME_ADDR4 *)(data))) != NULL) {
            memcpy(id.bssid, addr, AIRPDCAP_MAC_LEN);
#ifdef _DEBUG
            sprintf(msgbuf, "BSSID: %2X.%2X.%2X.%2X.%2X.%2X\t", id.bssid[0],id.bssid[1],id.bssid[2],id.bssid[3],id.bssid[4],id.bssid[5]);
#endif
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", msgbuf, AIRPDCAP_DEBUG_LEVEL_3);
        } else {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "BSSID not found", AIRPDCAP_DEBUG_LEVEL_5);
            return AIRPDCAP_RET_REQ_DATA;
        }

        /
        memcpy(id.sta, broadcast_mac, AIRPDCAP_MAC_LEN);

        /
        sa = AirPDcapGetSaPtr(ctx, &id);
        if (sa == NULL){
            return AIRPDCAP_RET_UNSUCCESS;
        }

        /

        /
        if ( (addr=AirPDcapGetStaAddress((const AIRPDCAP_MAC_FRAME_ADDR4 *)(data))) != NULL) {
            memcpy(id.sta, addr, AIRPDCAP_MAC_LEN);
#ifdef _DEBUG
            sprintf(msgbuf, "ST_MAC: %2X.%2X.%2X.%2X.%2X.%2X\t", id.sta[0],id.sta[1],id.sta[2],id.sta[3],id.sta[4],id.sta[5]);
#endif
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", msgbuf, AIRPDCAP_DEBUG_LEVEL_3);
        } else {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "SA not found", AIRPDCAP_DEBUG_LEVEL_5);
            return AIRPDCAP_RET_REQ_DATA;
        }

        sta_sa = AirPDcapGetSaPtr(ctx, &id);
        if (sta_sa == NULL){
            return AIRPDCAP_RET_UNSUCCESS;
        }

        /
        AirPDcapDecryptWPABroadcastKey(pEAPKey, sta_sa->wpa.ptk+16, sa, TRUE);

    }else{
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForGroupKey", "Skipping: not an EAPOL packet", AIRPDCAP_DEBUG_LEVEL_3);
    }

    AIRPDCAP_DEBUG_TRACE_END("AirPDcapScanForGroupKey");
    return 0;
}


INT AirPDcapPacketProcess(
    PAIRPDCAP_CONTEXT ctx,
    const guint8 *data,
    const guint mac_header_len,
    const guint tot_len,
    UCHAR *decrypt_data,
    guint *decrypt_len,
    PAIRPDCAP_KEY_ITEM key,
    gboolean mngHandshake,
    gboolean mngDecrypt)
{
    const UCHAR *addr;
    AIRPDCAP_SEC_ASSOCIATION_ID id;
    PAIRPDCAP_SEC_ASSOCIATION sa;
    int offset = 0;
    guint bodyLength;
    const guint8 dot1x_header[] = {
        0xAA,             /
        0xAA,             /
        0x03,             /
        0x00, 0x00, 0x00, /
        0x88, 0x8E        /
    };

    const guint8 bt_dot1x_header[] = {
        0xAA,             /
        0xAA,             /
        0x03,             /
        0x00, 0x19, 0x58, /
        0x00, 0x03        /
    };

#ifdef _DEBUG
    CHAR msgbuf[255];
#endif

    AIRPDCAP_DEBUG_TRACE_START("AirPDcapPacketProcess");

    if (ctx==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "NULL context", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapPacketProcess");
        return AIRPDCAP_RET_UNSUCCESS;
    }
    if (data==NULL || tot_len==0) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "NULL data or length=0", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapPacketProcess");
        return AIRPDCAP_RET_UNSUCCESS;
    }

    /
    if (AIRPDCAP_TYPE(data[0])!=AIRPDCAP_TYPE_DATA) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "not data packet", AIRPDCAP_DEBUG_LEVEL_5);
        return AIRPDCAP_RET_NO_DATA;
    }

    /
    if (tot_len < (UINT)(mac_header_len+AIRPDCAP_CRYPTED_DATA_MINLEN)) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "minimum length violated", AIRPDCAP_DEBUG_LEVEL_5);
        return AIRPDCAP_RET_WRONG_DATA_SIZE;
    }

    /
    if ( (addr=AirPDcapGetBssidAddress((const AIRPDCAP_MAC_FRAME_ADDR4 *)(data))) != NULL) {
        memcpy(id.bssid, addr, AIRPDCAP_MAC_LEN);
#ifdef _DEBUG
        sprintf(msgbuf, "BSSID: %2X.%2X.%2X.%2X.%2X.%2X\t", id.bssid[0],id.bssid[1],id.bssid[2],id.bssid[3],id.bssid[4],id.bssid[5]);
#endif
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", msgbuf, AIRPDCAP_DEBUG_LEVEL_3);
    } else {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "BSSID not found", AIRPDCAP_DEBUG_LEVEL_5);
        return AIRPDCAP_RET_REQ_DATA;
    }

    /
    if ( (addr=AirPDcapGetStaAddress((const AIRPDCAP_MAC_FRAME_ADDR4 *)(data))) != NULL) {
        memcpy(id.sta, addr, AIRPDCAP_MAC_LEN);
#ifdef _DEBUG
        sprintf(msgbuf, "ST_MAC: %2X.%2X.%2X.%2X.%2X.%2X\t", id.sta[0],id.sta[1],id.sta[2],id.sta[3],id.sta[4],id.sta[5]);
#endif
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", msgbuf, AIRPDCAP_DEBUG_LEVEL_3);
    } else {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "SA not found", AIRPDCAP_DEBUG_LEVEL_5);
        return AIRPDCAP_RET_REQ_DATA;
    }

    /
    sa = AirPDcapGetSaPtr(ctx, &id);
    if (sa == NULL){
        return AIRPDCAP_RET_UNSUCCESS;
    }

    /
    offset = mac_header_len;

    /
    if (AIRPDCAP_WEP(data[1])==0)
    {
        if (mngHandshake) {
            /
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "Unencrypted data", AIRPDCAP_DEBUG_LEVEL_3);

            /
            if (memcmp(data+offset, dot1x_header, 8) == 0 || memcmp(data+offset, bt_dot1x_header, 8) == 0) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "Authentication: EAPOL packet", AIRPDCAP_DEBUG_LEVEL_3);

                /
                offset+=8;

                /
                /
#if 0
                if (data[offset]!=2) {
                    AIRPDCAP_DEBUG_PRINT_LINE("EAPOL protocol version not recognized", AIRPDCAP_DEBUG_LEVEL_5);
                    return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
                }
#endif

                /
                if (data[offset+1]!=3) {
                    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "Not EAPOL-Key", AIRPDCAP_DEBUG_LEVEL_5);
                    return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
                }

                /
                bodyLength=pntoh16(data+offset+2);
                if ((tot_len-offset-4) < bodyLength) {
                    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "EAPOL body too short", AIRPDCAP_DEBUG_LEVEL_5);
                    return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
                }

                /
                offset+=4;

                /
                if (/    /
                    data[offset]!=0x2 &&        /
                    data[offset]!=0xFE)         /
                {
                    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "Not valid key descriptor type", AIRPDCAP_DEBUG_LEVEL_5);
                    return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
                }

                /
                offset+=1;

                /
                return AirPDcapRsna4WHandshake(ctx, data, sa, key, offset);
            } else {
                /
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "No authentication data", AIRPDCAP_DEBUG_LEVEL_5);
                return AIRPDCAP_RET_NO_DATA_ENCRYPTED;
            }
        }
    } else {
        if (mngDecrypt) {

            if (decrypt_data==NULL)
                return AIRPDCAP_RET_UNSUCCESS;

            /
            *decrypt_len = tot_len;
            memcpy(decrypt_data, data, *decrypt_len);

            /
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "Encrypted data", AIRPDCAP_DEBUG_LEVEL_3);

            /
            /
            /
            /
            if (AIRPDCAP_EXTIV(data[offset+3])==0) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "WEP encryption", AIRPDCAP_DEBUG_LEVEL_3);
                return AirPDcapWepMng(ctx, decrypt_data, mac_header_len, decrypt_len, key, sa, offset);
            } else {
                INT status;
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "TKIP or CCMP encryption", AIRPDCAP_DEBUG_LEVEL_3);

                /
                if (AIRPDCAP_KEY_INDEX(data[offset+3])>=1){

                    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", "The key index = 1. This is encrypted with a group key.", AIRPDCAP_DEBUG_LEVEL_3);

                    /
                    memcpy(id.sta, broadcast_mac, AIRPDCAP_MAC_LEN);

#ifdef _DEBUG
                    sprintf(msgbuf, "ST_MAC: %2X.%2X.%2X.%2X.%2X.%2X\t", id.sta[0],id.sta[1],id.sta[2],id.sta[3],id.sta[4],id.sta[5]);
                    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapPacketProcess", msgbuf, AIRPDCAP_DEBUG_LEVEL_3);
#endif

                    /
                    sa = AirPDcapGetSaPtr(ctx, &id);
                    if (sa == NULL){
                        return AIRPDCAP_RET_UNSUCCESS;
                    }
                }

                /
                status = AirPDcapRsnaMng(decrypt_data, mac_header_len, decrypt_len, key, sa, offset);

                /
                if (status == AIRPDCAP_RET_SUCCESS)
                    AirPDcapScanForGroupKey(ctx, decrypt_data, mac_header_len, *decrypt_len);
                return status;
            }
        }
    }

    return AIRPDCAP_RET_UNSUCCESS;
}

INT AirPDcapSetKeys(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_KEY_ITEM keys[],
    const size_t keys_nr)
{
    INT i;
    INT success;
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapSetKeys");

    if (ctx==NULL || keys==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "NULL context or NULL keys array", AIRPDCAP_DEBUG_LEVEL_3);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapSetKeys");
        return 0;
    }

    if (keys_nr>AIRPDCAP_MAX_KEYS_NR) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Keys number greater than maximum", AIRPDCAP_DEBUG_LEVEL_3);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapSetKeys");
        return 0;
    }

    /
    AirPDcapInitContext(ctx);

    /
    for (i=0, success=0; i<(INT)keys_nr; i++) {
        if (AirPDcapValidateKey(keys+i)==TRUE) {
            if (keys[i].KeyType==AIRPDCAP_KEY_TYPE_WPA_PWD) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Set a WPA-PWD key", AIRPDCAP_DEBUG_LEVEL_4);
                AirPDcapRsnaPwd2Psk(keys[i].UserPwd.Passphrase, keys[i].UserPwd.Ssid, keys[i].UserPwd.SsidLen, keys[i].KeyData.Wpa.Psk);
            }
#ifdef _DEBUG
            else if (keys[i].KeyType==AIRPDCAP_KEY_TYPE_WPA_PMK) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Set a WPA-PMK key", AIRPDCAP_DEBUG_LEVEL_4);
            } else if (keys[i].KeyType==AIRPDCAP_KEY_TYPE_WEP) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Set a WEP key", AIRPDCAP_DEBUG_LEVEL_4);
            } else {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Set a key", AIRPDCAP_DEBUG_LEVEL_4);
            }
#endif
            memcpy(&ctx->keys[success], &keys[i], sizeof(keys[i]));
            success++;
        }
    }

    ctx->keys_nr=success;

    AIRPDCAP_DEBUG_TRACE_END("AirPDcapSetKeys");
    return success;
}

static void
AirPDcapCleanKeys(
    PAIRPDCAP_CONTEXT ctx)
{
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapCleanKeys");

    if (ctx==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapCleanKeys", "NULL context", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapCleanKeys");
        return;
    }

    memset(ctx->keys, 0, sizeof(AIRPDCAP_KEY_ITEM) * AIRPDCAP_MAX_KEYS_NR);

    ctx->keys_nr=0;

    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapCleanKeys", "Keys collection cleaned!", AIRPDCAP_DEBUG_LEVEL_5);
    AIRPDCAP_DEBUG_TRACE_END("AirPDcapCleanKeys");
}

INT AirPDcapGetKeys(
    const PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_KEY_ITEM keys[],
    const size_t keys_nr)
{
    UINT i;
    UINT j;
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapGetKeys");

    if (ctx==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapGetKeys", "NULL context", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapGetKeys");
        return 0;
    } else if (keys==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapGetKeys", "NULL keys array", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapGetKeys");
        return (INT)ctx->keys_nr;
    } else {
        for (i=0, j=0; i<ctx->keys_nr && i<keys_nr && i<AIRPDCAP_MAX_KEYS_NR; i++) {
            memcpy(&keys[j], &ctx->keys[i], sizeof(keys[j]));
            j++;
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapGetKeys", "Got a key", AIRPDCAP_DEBUG_LEVEL_5);
        }

        AIRPDCAP_DEBUG_TRACE_END("AirPDcapGetKeys");
        return j;
    }
}

/
INT AirPDcapSetLastSSID(
    PAIRPDCAP_CONTEXT ctx,
    CHAR *pkt_ssid,
    size_t pkt_ssid_len)
{
    if (!ctx || !pkt_ssid || pkt_ssid_len < 1 || pkt_ssid_len > WPA_SSID_MAX_SIZE)
        return AIRPDCAP_RET_UNSUCCESS;

    memcpy(ctx->pkt_ssid, pkt_ssid, pkt_ssid_len);
    ctx->pkt_ssid_len = pkt_ssid_len;

    return AIRPDCAP_RET_SUCCESS;
}

INT AirPDcapInitContext(
    PAIRPDCAP_CONTEXT ctx)
{
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapInitContext");

    if (ctx==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapInitContext", "NULL context", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapInitContext");
        return AIRPDCAP_RET_UNSUCCESS;
    }

    AirPDcapCleanKeys(ctx);

    ctx->first_free_index=0;
    ctx->index=-1;
    ctx->sa_index=-1;
    ctx->pkt_ssid_len = 0;

    memset(ctx->sa, 0, AIRPDCAP_MAX_SEC_ASSOCIATIONS_NR * sizeof(AIRPDCAP_SEC_ASSOCIATION));

    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapInitContext", "Context initialized!", AIRPDCAP_DEBUG_LEVEL_5);
    AIRPDCAP_DEBUG_TRACE_END("AirPDcapInitContext");
    return AIRPDCAP_RET_SUCCESS;
}

INT AirPDcapDestroyContext(
    PAIRPDCAP_CONTEXT ctx)
{
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapDestroyContext");

    if (ctx==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapDestroyContext", "NULL context", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapDestroyContext");
        return AIRPDCAP_RET_UNSUCCESS;
    }

    AirPDcapCleanKeys(ctx);

    ctx->first_free_index=0;
    ctx->index=-1;
    ctx->sa_index=-1;

    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapDestroyContext", "Context destroyed!", AIRPDCAP_DEBUG_LEVEL_5);
    AIRPDCAP_DEBUG_TRACE_END("AirPDcapDestroyContext");
    return AIRPDCAP_RET_SUCCESS;
}

#ifdef __cplusplus
}
