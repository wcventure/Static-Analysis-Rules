#undef SPICE_RING_CONS_ITEM
#define SPICE_RING_CONS_ITEM(qxl, r, ret) {                             \
        typeof(r) start = r;                                            \
        typeof(r) end = r + 1;                                          \
        uint32_t cons = (r)->cons & SPICE_RING_INDEX_MASK(r);           \
        typeof(&(r)->items[cons]) m_item = &(r)->items[cons];           \
        if (!((uint8_t*)m_item >= (uint8_t*)(start) && (uint8_t*)(m_item + 1) <= (uint8_t*)(end))) { \
            qxl_set_guest_bug(qxl, "SPICE_RING_CONS_ITEM indices mismatch " \
                          "! %p <= %p < %p", (uint8_t *)start,          \
                          (uint8_t *)m_item, (uint8_t *)end);           \
            ret = NULL;                                                 \
        } else {                                                        \
            ret = &m_item->el;                                          \
        }                                                               \
    }
