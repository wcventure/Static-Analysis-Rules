static int vmdk_open_vmdk4(BlockDriverState *bs,
                           BlockDriverState *file,
                           int flags)
{
    int ret;
    uint32_t magic;
    uint32_t l1_size, l1_entry_sectors;
    VMDK4Header header;
    VmdkExtent *extent;
    int64_t l1_backup_offset = 0;

    ret = bdrv_pread(file, sizeof(magic), &header, sizeof(header));
    if (ret < 0) {
        return ret;
    }
    if (header.capacity == 0) {
        int64_t desc_offset = le64_to_cpu(header.desc_offset);
        if (desc_offset) {
            return vmdk_open_desc_file(bs, flags, desc_offset << 9);
        }
    }

    if (le64_to_cpu(header.gd_offset) == VMDK4_GD_AT_END) {
        /
        struct {
            struct {
                uint64_t val;
                uint32_t size;
                uint32_t type;
                uint8_t pad[512 - 16];
            } QEMU_PACKED footer_marker;

            uint32_t magic;
            VMDK4Header header;
            uint8_t pad[512 - 4 - sizeof(VMDK4Header)];

            struct {
                uint64_t val;
                uint32_t size;
                uint32_t type;
                uint8_t pad[512 - 16];
            } QEMU_PACKED eos_marker;
        } QEMU_PACKED footer;

        ret = bdrv_pread(file,
            bs->file->total_sectors * 512 - 1536,
            &footer, sizeof(footer));
        if (ret < 0) {
            return ret;
        }

        /
        if (be32_to_cpu(footer.magic) != VMDK4_MAGIC ||
            le32_to_cpu(footer.footer_marker.size) != 0  ||
            le32_to_cpu(footer.footer_marker.type) != MARKER_FOOTER ||
            le64_to_cpu(footer.eos_marker.val) != 0  ||
            le32_to_cpu(footer.eos_marker.size) != 0  ||
            le32_to_cpu(footer.eos_marker.type) != MARKER_END_OF_STREAM)
        {
            return -EINVAL;
        }

        header = footer.header;
    }

    if (le32_to_cpu(header.version) >= 3) {
        char buf[64];
        snprintf(buf, sizeof(buf), "VMDK version %d",
                 le32_to_cpu(header.version));
        qerror_report(QERR_UNKNOWN_BLOCK_FORMAT_FEATURE,
                bs->device_name, "vmdk", buf);
        return -ENOTSUP;
    }

    l1_entry_sectors = le32_to_cpu(header.num_gtes_per_gte)
                        * le64_to_cpu(header.granularity);
    if (l1_entry_sectors == 0) {
        return -EINVAL;
    }
    l1_size = (le64_to_cpu(header.capacity) + l1_entry_sectors - 1)
                / l1_entry_sectors;
    if (le32_to_cpu(header.flags) & VMDK4_FLAG_RGD) {
        l1_backup_offset = le64_to_cpu(header.rgd_offset) << 9;
    }
    extent = vmdk_add_extent(bs, file, false,
                          le64_to_cpu(header.capacity),
                          le64_to_cpu(header.gd_offset) << 9,
                          l1_backup_offset,
                          l1_size,
                          le32_to_cpu(header.num_gtes_per_gte),
                          le64_to_cpu(header.granularity));
    extent->compressed =
        le16_to_cpu(header.compressAlgorithm) == VMDK4_COMPRESSION_DEFLATE;
    extent->has_marker = le32_to_cpu(header.flags) & VMDK4_FLAG_MARKER;
    extent->version = le32_to_cpu(header.version);
    extent->has_zero_grain = le32_to_cpu(header.flags) & VMDK4_FLAG_ZERO_GRAIN;
    ret = vmdk_init_tables(bs, extent);
    if (ret) {
        /
        vmdk_free_last_extent(bs);
    }
    return ret;
}
