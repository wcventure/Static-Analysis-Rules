static pana_avp_types
pana_avp_get_type(guint16 avp_code, guint32 vendor_id)
{
       if(vendor_id == 0) {
               switch(avp_code) {
                       case 1: return PANA_OCTET_STRING;       /
                       case 2: return PANA_EAP;                /
                       case 3: return PANA_UNSIGNED32;         /
                       case 4: return PANA_INTEGER32;          /
                       case 5: return PANA_OCTET_STRING;       /
                       case 6: return PANA_UNSIGNED32;         /
                       case 7: return PANA_RESULT_CODE;        /
                       case 8: return PANA_UNSIGNED32;         /
                       case 9: return PANA_ENUMERATED;         /
                       default: return PANA_OCTET_STRING;
               }
       } else {
               return PANA_OCTET_STRING;
       }
}
