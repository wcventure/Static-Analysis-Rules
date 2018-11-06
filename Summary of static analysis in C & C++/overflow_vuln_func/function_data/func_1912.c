static void blkdebug_refresh_filename(BlockDriverState *bs, QDict *options)
{
    BDRVBlkdebugState *s = bs->opaque;
    QDict *opts;
    const QDictEntry *e;
    bool force_json = false;

    for (e = qdict_first(options); e; e = qdict_next(options, e)) {
        if (strcmp(qdict_entry_key(e), "config") &&
            strcmp(qdict_entry_key(e), "x-image"))
        {
            force_json = true;
            break;
        }
    }

    if (force_json && !bs->file->bs->full_open_options) {
        /
        return;
    }

    if (!force_json && bs->file->bs->exact_filename[0]) {
        snprintf(bs->exact_filename, sizeof(bs->exact_filename),
                 "blkdebug:%s:%s", s->config_file ?: "",
                 bs->file->bs->exact_filename);
    }

    opts = qdict_new();
    qdict_put_str(opts, "driver", "blkdebug");

    QINCREF(bs->file->bs->full_open_options);
    qdict_put(opts, "image", bs->file->bs->full_open_options);

    for (e = qdict_first(options); e; e = qdict_next(options, e)) {
        if (strcmp(qdict_entry_key(e), "x-image")) {
            qobject_incref(qdict_entry_value(e));
            qdict_put_obj(opts, qdict_entry_key(e), qdict_entry_value(e));
        }
    }

    bs->full_open_options = opts;
}
