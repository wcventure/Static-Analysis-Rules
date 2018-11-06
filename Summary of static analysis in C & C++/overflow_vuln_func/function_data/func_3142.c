static inline int mov_get_stsc_samples(MOVStreamContext *sc, int index)
{
    int chunk_count;

    if (mov_stsc_index_valid(index, sc->stsc_count))
        chunk_count = sc->stsc_data[index + 1].first - sc->stsc_data[index].first;
    else
        chunk_count = sc->chunk_count - (sc->stsc_data[index].first - 1);

    return sc->stsc_data[index].count * chunk_count;
}
