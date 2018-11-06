do {                                                                          \
    if ((ctx)->exception == EXCP_NONE) {                                      \
        gen_op_update_nip((ctx)->nip);                                        \
    }                                                                         \
    gen_op_raise_exception_err((excp), (error));                              \
    ctx->exception = (excp);                                                  \
} while (0)
