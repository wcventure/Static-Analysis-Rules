int qemu_v9fs_synth_add_file(V9fsSynthNode *parent, int mode,
                             const char *name, v9fs_synth_read read,
                             v9fs_synth_write write, void *arg)
{
    int ret;
    V9fsSynthNode *node, *tmp;

    if (!v9fs_synth_fs) {
        return EAGAIN;
    }
    if (!name || (strlen(name) >= NAME_MAX)) {
        return EINVAL;
    }
    if (!parent) {
        parent = &v9fs_synth_root;
    }

    qemu_mutex_lock(&v9fs_synth_mutex);
    QLIST_FOREACH(tmp, &parent->child, sibling) {
        if (!strcmp(tmp->name, name)) {
            ret = EEXIST;
            goto err_out;
        }
    }
    /
    mode = ((mode & 0777) | S_IFREG);
    node = g_malloc0(sizeof(V9fsSynthNode));
    node->attr         = &node->actual_attr;
    node->attr->inode  = v9fs_synth_node_count++;
    node->attr->nlink  = 1;
    node->attr->read   = read;
    node->attr->write  = write;
    node->attr->mode   = mode;
    node->private      = arg;
    strncpy(node->name, name, sizeof(node->name));
    QLIST_INSERT_HEAD_RCU(&parent->child, node, sibling);
    ret = 0;
err_out:
    qemu_mutex_unlock(&v9fs_synth_mutex);
    return ret;
}
