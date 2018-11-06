static int qcrypto_ivgen_essiv_calculate(QCryptoIVGen *ivgen,
                                         uint64_t sector,
                                         uint8_t *iv, size_t niv,
                                         Error **errp)
{
    QCryptoIVGenESSIV *essiv = ivgen->private;
    size_t ndata = qcrypto_cipher_get_block_len(ivgen->cipher);
    uint8_t *data = g_new(uint8_t, ndata);

    sector = cpu_to_le64(sector);
    memcpy(data, (uint8_t *)&sector, ndata);
    if (sizeof(sector) < ndata) {
        memset(data + sizeof(sector), 0, ndata - sizeof(sector));
    }

    if (qcrypto_cipher_encrypt(essiv->cipher,
                               data,
                               data,
                               ndata,
                               errp) < 0) {
        g_free(data);
        return -1;
    }

    if (ndata > niv) {
        ndata = niv;
    }
    memcpy(iv, data, ndata);
    if (ndata < niv) {
        memset(iv + ndata, 0, niv - ndata);
    }
    g_free(data);
    return 0;
}
