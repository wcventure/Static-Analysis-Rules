static uint64_t qdev_get_prop_mask64(Property *prop)
{
    assert(prop->info == &qdev_prop_bit);
    return 0x1 << prop->bitnr;
}
