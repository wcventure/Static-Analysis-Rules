static int add_candidate_ref(HEVCContext *s, RefPicList *list,
                             int poc, int ref_flag)
{
    HEVCFrame *ref = find_ref_idx(s, poc);

    if (ref == s->ref)
        return AVERROR_INVALIDDATA;

    if (!ref) {
        ref = generate_missing_ref(s, poc);
        if (!ref)
            return AVERROR(ENOMEM);
    }

    list->list[list->nb_refs] = ref->poc;
    list->ref[list->nb_refs]  = ref;
    list->nb_refs++;

    mark_ref(ref, ref_flag);
    return 0;
}
