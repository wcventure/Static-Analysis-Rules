#ifdef  __cplusplus
extern "C" {
#endif

/
static INT AirPDcapRsnaPwd2PskStep(
    const guint8 *ppbytes,
    const guint passLength,
    const CHAR *ssid,
    const size_t ssidLength,
    const INT iterations,
    const INT count,
    UCHAR *output)
    ;

/
static INT AirPDcapRsnaPwd2Psk(
    const CHAR *passphrase,
    const CHAR *ssid,
    const size_t ssidLength,
    UCHAR *output)
    ;

static INT AirPDcapRsnaMng(
    UCHAR *decrypt_data,
    guint mac_header_len,
    guint *decrypt_len,
    PAIRPDCAP_KEY_ITEM key,
    AIRPDCAP_SEC_ASSOCIATION *sa,
    INT offset)
    ;

static INT AirPDcapWepMng(
    PAIRPDCAP_CONTEXT ctx,
    UCHAR *decrypt_data,
    guint mac_header_len,
    guint *decrypt_len,
    PAIRPDCAP_KEY_ITEM key,
    AIRPDCAP_SEC_ASSOCIATION *sa,
    INT offset)
    ;

static INT AirPDcapRsna4WHandshake(
    PAIRPDCAP_CONTEXT ctx,
    const UCHAR *data,
    AIRPDCAP_SEC_ASSOCIATION *sa,
    PAIRPDCAP_KEY_ITEM key,
    INT offset)
    ;
/
static INT AirPDcapValidateKey(
    PAIRPDCAP_KEY_ITEM key)
    ;

static INT AirPDcapRsnaMicCheck(
    UCHAR *eapol,
    USHORT eapol_len,
    UCHAR KCK[AIRPDCAP_WPA_KCK_LEN],
    USHORT key_ver)
    ;

/
static INT AirPDcapGetSa(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_SEC_ASSOCIATION_ID *id)
    ;

static INT AirPDcapStoreSa(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_SEC_ASSOCIATION_ID *id)
    ;

static const UCHAR * AirPDcapGetStaAddress(
    const AIRPDCAP_MAC_FRAME_ADDR4 *frame)
    ;

static const UCHAR * AirPDcapGetBssidAddress(
    const AIRPDCAP_MAC_FRAME_ADDR4 *frame)
    ;

static void AirPDcapRsnaPrfX(
    AIRPDCAP_SEC_ASSOCIATION *sa,
    const UCHAR pmk[32],
    const UCHAR snonce[32],
    const INT x,        /
    UCHAR *ptk)
    ;

#ifdef  __cplusplus
}
