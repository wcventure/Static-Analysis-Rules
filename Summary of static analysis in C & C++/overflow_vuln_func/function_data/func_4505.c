static inline void expand_category(COOKContext *q, int *category,
                                   int *category_index)
{
    int i;
    for (i = 0; i < q->num_vectors; i++)
        ++category[category_index[i]];
}
