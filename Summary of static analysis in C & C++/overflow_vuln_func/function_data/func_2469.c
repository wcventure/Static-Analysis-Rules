#undef SPICE_RING_PROD_ITEM
#define SPICE_RING_PROD_ITEM(qxl, r, ret) {                             \
        typeof(r) start = r;                                            \
        typeof(r) end = r + 1;                                          \
        uint32_t prod = (r)->prod & SPICE_RING_INDEX_MASK(r);           \
        typeof(&(r)->items[prod]) m_item = &(r)->items[prod];           \
        if (!((uint8_t*)m_item >= (uint8_t*)(start) && (uint8_t*)(m_item + 1) <= (uint8_t*)(end))) { \
            qxl_set_guest_bug(qxl, "SPICE_RING_PROD_ITEM indices mismatch " \
                          "! %p <= %p < %p", (uint8_t *)start,          \
                          (uint8_t *)m_item, (uint8_t *)end);           \
            ret = NULL;                                                 \
        } else {                                                        \
            ret = &m_item->el;                                          \
        }                                                               \
    }
