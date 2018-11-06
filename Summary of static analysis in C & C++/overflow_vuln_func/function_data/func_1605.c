static void dissect_r3_cmd_manageuser (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo, proto_tree *tree)
{
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  guint32 offset = 0;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  while (offset < tvb_reported_length (payload_tvb))
  {
    guint32 paramLength = tvb_get_guint8 (payload_tvb, offset + 0);
    guint32 paramType = tvb_get_guint8 (payload_tvb, offset + 1);
    guint32 dataLength = paramLength - 2;
    proto_tree *mu_tree = NULL;
    proto_item *len_field = NULL;
    const gchar *auptn = NULL;

    auptn = val_to_str_ext_const (paramType, &r3_adduserparamtypenames_ext, "[Unknown Field]");

    mu_tree = proto_item_add_subtree (proto_tree_add_none_format (tree, hf_r3_adduserparamtype, payload_tvb, offset + 0, paramLength, "Manage User Field: %s (%u)", auptn, paramType), ett_r3manageuser);

    len_field = proto_tree_add_item (mu_tree, hf_r3_adduserparamtypelength, payload_tvb, offset + 0, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (mu_tree, hf_r3_adduserparamtypetype, payload_tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);

    if (paramLength < 2)
    {
      dataLength = 0;
      expert_add_info_format (pinfo, len_field, PI_UNDECODED, PI_WARN, "Malformed length value -- all fields are at least 2 octets.");
    }

    offset += 2;

    switch (paramType)
    {
      case ADDUSERPARAMTYPE_DISPOSITION :
      case ADDUSERPARAMTYPE_ACCESSALWAYS :
      case ADDUSERPARAMTYPE_ACCESSMODE :
      case ADDUSERPARAMTYPE_CACHED :
      case ADDUSERPARAMTYPE_USERTYPE :
      case ADDUSERPARAMTYPE_PRIMARYFIELDTYPE :
      case ADDUSERPARAMTYPE_AUXFIELDTYPE :
      case ADDUSERPARAMTYPE_USECOUNT :
      case ADDUSERPARAMTYPE_EXCEPTIONGROUP :
        if (dataLength != 1)
          expert_add_info_format (pinfo, mu_tree, PI_UNDECODED, PI_WARN, "Malformed field -- expected 1 octet");
        else
          proto_tree_add_item (mu_tree, hf_r3_adduserparamtypearray [paramType], payload_tvb, offset, dataLength, ENC_LITTLE_ENDIAN);
        break;

      case ADDUSERPARAMTYPE_USERNO :
        if (dataLength != 2)
          expert_add_info_format (pinfo, mu_tree, PI_UNDECODED, PI_WARN, "Malformed field -- expected 2 octets");
        else
          proto_tree_add_item (mu_tree, hf_r3_adduserparamtypearray [paramType], payload_tvb, offset, dataLength, ENC_LITTLE_ENDIAN);
        break;

      case ADDUSERPARAMTYPE_PRIMARYFIELD :
      case ADDUSERPARAMTYPE_AUXFIELD :
        proto_tree_add_item (mu_tree, hf_r3_adduserparamtypearray [paramType], payload_tvb, offset, dataLength, ENC_NA);
        break;

      case ADDUSERPARAMTYPE_EXPIREON :
        {
          proto_item *expireon_item = NULL;
          proto_tree *expireon_tree = NULL;

          if (dataLength != 3)
            expert_add_info_format (pinfo, mu_tree, PI_UNDECODED, PI_WARN, "Malformed expiration field -- expected 3 octets");
          else
          {
            expireon_item = proto_tree_add_text (mu_tree, payload_tvb, offset, 3, "Expire YY/MM/DD: %02u/%02u/%02u",
                tvb_get_guint8 (payload_tvb, offset + 2), tvb_get_guint8 (payload_tvb, offset + 0), tvb_get_guint8 (payload_tvb, offset + 1));
            expireon_tree = proto_item_add_subtree (expireon_item, ett_r3expireon);

            proto_tree_add_item (expireon_tree, hf_r3_expireon_month, payload_tvb, offset + 0, 1, ENC_LITTLE_ENDIAN);
            proto_tree_add_item (expireon_tree, hf_r3_expireon_day, payload_tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
            proto_tree_add_item (expireon_tree, hf_r3_expireon_year, payload_tvb, offset + 2, 1, ENC_LITTLE_ENDIAN);
          }
        }
        break;

      case ADDUSERPARAMTYPE_TIMEZONE :
        {
          guint32 i;
          guint32 tz;
          proto_item *timezone_item = NULL;
          proto_tree *timezone_tree = NULL;

          if (dataLength != 4)
            expert_add_info_format (pinfo, mu_tree, PI_UNDECODED, PI_WARN, "Malformed timezone field -- expected 4 octets");
          else
          {
            tz = tvb_get_letohl (payload_tvb, offset);
            timezone_item = proto_tree_add_item (mu_tree, hf_r3_upstreamfieldarray [paramType], payload_tvb, offset, 4, ENC_LITTLE_ENDIAN);
            timezone_tree = proto_item_add_subtree (timezone_item, ett_r3timezone);

            for (i = 0; i < 32; i++)
              proto_tree_add_boolean (timezone_tree, hf_r3_timezonearray [i], payload_tvb, offset, 4, tz);
          }
        }
        break;

      default :
        proto_tree_add_string (mu_tree, hf_r3_upstreamfielderror, payload_tvb, offset, dataLength, "Unknown Field Type");
        break;
    }

    offset += dataLength;
  }
}
