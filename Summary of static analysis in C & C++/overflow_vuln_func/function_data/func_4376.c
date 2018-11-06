void avpriv_do_elbg(int *points, int dim, int numpoints, int *codebook,
                int numCB, int max_steps, int *closest_cb,
                AVLFG *rand_state)
{
    int dist;
    elbg_data elbg_d;
    elbg_data *elbg = &elbg_d;
    int i, j, k, last_error, steps=0;
    int *dist_cb = av_malloc(numpoints*sizeof(int));
    int *size_part = av_malloc(numCB*sizeof(int));
    cell *list_buffer = av_malloc(numpoints*sizeof(cell));
    cell *free_cells;
    int best_dist, best_idx = 0;

    elbg->error = INT_MAX;
    elbg->dim = dim;
    elbg->numCB = numCB;
    elbg->codebook = codebook;
    elbg->cells = av_malloc(numCB*sizeof(cell *));
    elbg->utility = av_malloc(numCB*sizeof(int));
    elbg->nearest_cb = closest_cb;
    elbg->points = points;
    elbg->utility_inc = av_malloc(numCB*sizeof(int));
    elbg->scratchbuf = av_malloc(5*dim*sizeof(int));

    elbg->rand_state = rand_state;

    do {
        free_cells = list_buffer;
        last_error = elbg->error;
        steps++;
        memset(elbg->utility, 0, numCB*sizeof(int));
        memset(elbg->cells, 0, numCB*sizeof(cell *));

        elbg->error = 0;

        /
        for (i=0; i < numpoints; i++) {
            best_dist = distance_limited(elbg->points + i*elbg->dim, elbg->codebook + best_idx*elbg->dim, dim, INT_MAX);
            for (k=0; k < elbg->numCB; k++) {
                dist = distance_limited(elbg->points + i*elbg->dim, elbg->codebook + k*elbg->dim, dim, best_dist);
                if (dist < best_dist) {
                    best_dist = dist;
                    best_idx = k;
                }
            }
            elbg->nearest_cb[i] = best_idx;
            dist_cb[i] = best_dist;
            elbg->error += dist_cb[i];
            elbg->utility[elbg->nearest_cb[i]] += dist_cb[i];
            free_cells->index = i;
            free_cells->next = elbg->cells[elbg->nearest_cb[i]];
            elbg->cells[elbg->nearest_cb[i]] = free_cells;
            free_cells++;
        }

        do_shiftings(elbg);

        memset(size_part, 0, numCB*sizeof(int));

        memset(elbg->codebook, 0, elbg->numCB*dim*sizeof(int));

        for (i=0; i < numpoints; i++) {
            size_part[elbg->nearest_cb[i]]++;
            for (j=0; j < elbg->dim; j++)
                elbg->codebook[elbg->nearest_cb[i]*elbg->dim + j] +=
                    elbg->points[i*elbg->dim + j];
        }

        for (i=0; i < elbg->numCB; i++)
            vect_division(elbg->codebook + i*elbg->dim,
                          elbg->codebook + i*elbg->dim, size_part[i], elbg->dim);

    } while(((last_error - elbg->error) > DELTA_ERR_MAX*elbg->error) &&
            (steps < max_steps));

    av_free(dist_cb);
    av_free(size_part);
    av_free(elbg->utility);
    av_free(list_buffer);
    av_free(elbg->cells);
    av_free(elbg->utility_inc);
    av_free(elbg->scratchbuf);
}
