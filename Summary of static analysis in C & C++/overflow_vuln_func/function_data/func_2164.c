void OPPROTO op_set_Rc0 (void)
{
    env->crf[0] = T0 | xer_ov;
    RETURN();
}
