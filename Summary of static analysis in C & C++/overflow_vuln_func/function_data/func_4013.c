static void print_track_chunks(FILE *out, struct Tracks *tracks, int main,
                               const char *type)
{
    int i, j;
    struct Track *track = tracks->tracks[main];
    for (i = 0; i < track->chunks; i++) {
        for (j = main + 1; j < tracks->nb_tracks; j++) {
            if (tracks->tracks[j]->is_audio == track->is_audio &&
                track->offsets[i].duration != tracks->tracks[j]->offsets[i].duration)
                fprintf(stderr, "Mismatched duration of %s chunk %d in %s and %s\n",
                        type, i, track->name, tracks->tracks[j]->name);
        }
        fprintf(out, "\t\t<c n=\"%d\" d=\"%d\" />\n",
                i, track->offsets[i].duration);
    }
}
