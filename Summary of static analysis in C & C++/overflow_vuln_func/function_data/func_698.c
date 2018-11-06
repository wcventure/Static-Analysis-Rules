#define AMQP_INCREMENT(offset, addend, bound) {\
    int tmp;\
    tmp = offset;\
    offset += (addend);\
    DISSECTOR_ASSERT(offset >= tmp && offset <= bound);\
}
