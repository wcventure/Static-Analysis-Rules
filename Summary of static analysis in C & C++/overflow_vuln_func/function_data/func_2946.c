void qemuio_add_command(const cmdinfo_t *ci)
{
    cmdtab = g_realloc(cmdtab, ++ncmds * sizeof(*cmdtab));
    cmdtab[ncmds - 1] = *ci;
    qsort(cmdtab, ncmds, sizeof(*cmdtab), compare_cmdname);
}
