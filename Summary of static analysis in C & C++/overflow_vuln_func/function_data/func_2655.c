static size_t pdu_unmarshal(V9fsPDU *pdu, size_t offset, const char *fmt, ...)
{
    size_t old_offset = offset;
    va_list ap;
    int i;

    va_start(ap, fmt);
    for (i = 0; fmt[i]; i++) {
        switch (fmt[i]) {
        case 'b': {
            uint8_t *valp = va_arg(ap, uint8_t *);
            offset += pdu_unpack(valp, pdu, offset, sizeof(*valp));
            break;
        }
        case 'w': {
            uint16_t val, *valp;
            valp = va_arg(ap, uint16_t *);
            offset += pdu_unpack(&val, pdu, offset, sizeof(val));
            *valp = le16_to_cpu(val);
            break;
        }
        case 'd': {
            uint32_t val, *valp;
            valp = va_arg(ap, uint32_t *);
            offset += pdu_unpack(&val, pdu, offset, sizeof(val));
            *valp = le32_to_cpu(val);
            break;
        }
        case 'q': {
            uint64_t val, *valp;
            valp = va_arg(ap, uint64_t *);
            offset += pdu_unpack(&val, pdu, offset, sizeof(val));
            *valp = le64_to_cpu(val);
            break;
        }
        case 'v': {
            struct iovec *iov = va_arg(ap, struct iovec *);
            int *iovcnt = va_arg(ap, int *);
            *iovcnt = pdu_copy_sg(pdu, offset, 0, iov);
            break;
        }
        case 's': {
            V9fsString *str = va_arg(ap, V9fsString *);
            offset += pdu_unmarshal(pdu, offset, "w", &str->size);
            /
            str->data = g_malloc(str->size + 1);
            offset += pdu_unpack(str->data, pdu, offset, str->size);
            str->data[str->size] = 0;
            break;
        }
        case 'Q': {
            V9fsQID *qidp = va_arg(ap, V9fsQID *);
            offset += pdu_unmarshal(pdu, offset, "bdq",
                        &qidp->type, &qidp->version, &qidp->path);
            break;
        }
        case 'S': {
            V9fsStat *statp = va_arg(ap, V9fsStat *);
            offset += pdu_unmarshal(pdu, offset, "wwdQdddqsssssddd",
                        &statp->size, &statp->type, &statp->dev,
                        &statp->qid, &statp->mode, &statp->atime,
                        &statp->mtime, &statp->length,
                        &statp->name, &statp->uid, &statp->gid,
                        &statp->muid, &statp->extension,
                        &statp->n_uid, &statp->n_gid,
                        &statp->n_muid);
            break;
        }
        case 'I': {
            V9fsIattr *iattr = va_arg(ap, V9fsIattr *);
            offset += pdu_unmarshal(pdu, offset, "ddddqqqqq",
                        &iattr->valid, &iattr->mode,
                        &iattr->uid, &iattr->gid, &iattr->size,
                        &iattr->atime_sec, &iattr->atime_nsec,
                        &iattr->mtime_sec, &iattr->mtime_nsec);
            break;
        }
        default:
            break;
        }
    }

    va_end(ap);

    return offset - old_offset;
}
