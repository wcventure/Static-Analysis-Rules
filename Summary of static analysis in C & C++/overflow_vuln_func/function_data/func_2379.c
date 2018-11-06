static inline void gen_bcond(DisasContext *ctx, int type) 
{                                                                             
    target_ulong target = 0;
    target_ulong li;
    uint32_t bo = BO(ctx->opcode);                                            
    uint32_t bi = BI(ctx->opcode);                                            
    uint32_t mask;                                                            

    if ((bo & 0x4) == 0)
        gen_op_dec_ctr();                                                     
    switch(type) {
    case BCOND_IM:
        li = (target_long)((int16_t)(BD(ctx->opcode)));
        if (likely(AA(ctx->opcode) == 0)) {
            target = ctx->nip + li - 4;
        } else {
            target = li;
        }
        break;
    case BCOND_CTR:
        gen_op_movl_T1_ctr();
        break;
    default:
    case BCOND_LR:
        gen_op_movl_T1_lr();
        break;
    }
    if (LK(ctx->opcode)) {                                        
        gen_op_setlr(ctx->nip);
    }
    if (bo & 0x10) {
        /                                                 
        switch (bo & 0x6) {                                                   
        case 0:                                                               
            gen_op_test_ctr();
            break;
        case 2:                                                               
            gen_op_test_ctrz();
            break;                                                            
        default:
        case 4:                                                               
        case 6:                                                               
            if (type == BCOND_IM) {
                gen_goto_tb(ctx, 0, target);
            } else {
                gen_op_b_T1();
                gen_op_reset_T0();
            }
            goto no_test;
        }
    } else {                                                                  
        mask = 1 << (3 - (bi & 0x03));                                        
        gen_op_load_crf_T0(bi >> 2);                                          
        if (bo & 0x8) {                                                       
            switch (bo & 0x6) {                                               
            case 0:                                                           
                gen_op_test_ctr_true(mask);
                break;                                                        
            case 2:                                                           
                gen_op_test_ctrz_true(mask);
                break;                                                        
            default:                                                          
            case 4:                                                           
            case 6:                                                           
                gen_op_test_true(mask);
                break;                                                        
            }                                                                 
        } else {                                                              
            switch (bo & 0x6) {                                               
            case 0:                                                           
                gen_op_test_ctr_false(mask);
                break;                                                        
            case 2:                                                           
                gen_op_test_ctrz_false(mask);
                break;                                                        
            default:
            case 4:                                                           
            case 6:                                                           
                gen_op_test_false(mask);
                break;                                                        
            }                                                                 
        }                                                                     
    }                                                                         
    if (type == BCOND_IM) {
        int l1 = gen_new_label();
        gen_op_jz_T0(l1);
        gen_goto_tb(ctx, 0, target);
        gen_set_label(l1);
        gen_goto_tb(ctx, 1, ctx->nip);
    } else {
        gen_op_btest_T1(ctx->nip);
        gen_op_reset_T0();
    }
 no_test:
    if (ctx->singlestep_enabled)
        gen_op_debug();
    gen_op_exit_tb();
    ctx->exception = EXCP_BRANCH;                                             
}
