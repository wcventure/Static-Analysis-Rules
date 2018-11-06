void proto_reg_handoff_cms(void) {
  dissector_handle_t content_info_handle;

/
#line 1 "../../asn1/cms/packet-cms-dis-tab.c"
  new_register_ber_oid_dissector("1.2.840.113549.1.9.16.1.6", dissect_ContentInfo_PDU, proto_cms, "id-ct-contentInfo");
  new_register_ber_oid_dissector("1.2.840.113549.1.7.2", dissect_SignedData_PDU, proto_cms, "id-signedData");
  new_register_ber_oid_dissector("1.2.840.113549.1.7.3", dissect_EnvelopedData_PDU, proto_cms, "id-envelopedData");
  new_register_ber_oid_dissector("1.2.840.113549.1.7.5", dissect_DigestedData_PDU, proto_cms, "id-digestedData");
  new_register_ber_oid_dissector("1.2.840.113549.1.7.6", dissect_EncryptedData_PDU, proto_cms, "id-encryptedData");
  new_register_ber_oid_dissector("1.2.840.113549.1.9.16.1.2", dissect_AuthenticatedData_PDU, proto_cms, "id-ct-authenticatedData");
  new_register_ber_oid_dissector("1.2.840.113549.1.9.3", dissect_ContentType_PDU, proto_cms, "id-contentType");
  new_register_ber_oid_dissector("1.2.840.113549.1.9.4", dissect_MessageDigest_PDU, proto_cms, "id-messageDigest");
  new_register_ber_oid_dissector("1.2.840.113549.1.9.5", dissect_SigningTime_PDU, proto_cms, "id-signingTime");
  new_register_ber_oid_dissector("1.2.840.113549.1.9.6", dissect_Countersignature_PDU, proto_cms, "id-counterSignature");
  new_register_ber_oid_dissector("2.6.1.4.18", dissect_ContentInfo_PDU, proto_cms, "id-et-pkcs7");
  new_register_ber_oid_dissector("1.3.6.1.4.1.311.16.4", dissect_IssuerAndSerialNumber_PDU, proto_cms, "ms-oe-encryption-key-preference");
  new_register_ber_oid_dissector("1.2.840.113549.1.9.15", dissect_SMIMECapabilities_PDU, proto_cms, "id-smime-capabilities");
  new_register_ber_oid_dissector("1.2.840.113549.1.9.16.2.11", dissect_SMIMEEncryptionKeyPreference_PDU, proto_cms, "id-encryption-key-preference");
  new_register_ber_oid_dissector("1.2.840.113549.3.2", dissect_RC2CBCParameters_PDU, proto_cms, "id-alg-rc2-cbc");
  new_register_ber_oid_dissector("1.2.840.113549.3.4", dissect_RC2CBCParameters_PDU, proto_cms, "id-alg-rc4");
  new_register_ber_oid_dissector("1.2.840.113549.1.9.16.3.7", dissect_RC2WrapParameter_PDU, proto_cms, "id-alg-cmsrc2-wrap");
  new_register_ber_oid_dissector("2.16.840.1.113730.3.1.40", dissect_SignedData_PDU, proto_cms, "userSMIMECertificate");


/
#line 172 "../../asn1/cms/packet-cms-template.c"

  oid_add_from_string("id-data","1.2.840.113549.1.7.1");
  oid_add_from_string("id-alg-des-ede3-cbc","1.2.840.113549.3.7");
  oid_add_from_string("id-alg-des-cbc","1.3.14.3.2.7");

  content_info_handle = new_create_dissector_handle (dissect_ContentInfo_PDU, proto_cms);
  dissector_add_string("media_type", "application/pkcs7-mime", content_info_handle);
  dissector_add_string("media_type", "application/pkcs7-signature", content_info_handle);
}
