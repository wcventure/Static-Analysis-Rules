static void push_output_configuration(AACContext *ac) {
    if (ac->oc[1].status == OC_LOCKED || ac->oc[0].status == OC_NONE) {
        ac->oc[0] = ac->oc[1];
    }
    ac->oc[1].status = OC_NONE;
}
