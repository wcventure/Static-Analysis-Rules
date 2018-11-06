static const char *full_name_nth(QObjectInputVisitor *qiv, const char *name,
                                 int n)
{
    StackObject *so;
    char buf[32];

    if (qiv->errname) {
        g_string_truncate(qiv->errname, 0);
    } else {
        qiv->errname = g_string_new("");
    }

    QSLIST_FOREACH(so , &qiv->stack, node) {
        if (n) {
            n--;
        } else if (qobject_type(so->obj) == QTYPE_QDICT) {
            g_string_prepend(qiv->errname, name ?: "<anonymous>");
            g_string_prepend_c(qiv->errname, '.');
        } else {
            snprintf(buf, sizeof(buf), "[%u]", so->index);
            g_string_prepend(qiv->errname, buf);
        }
        name = so->name;
    }
    assert(!n);

    if (name) {
        g_string_prepend(qiv->errname, name);
    } else if (qiv->errname->str[0] == '.') {
        g_string_erase(qiv->errname, 0, 1);
    } else if (!qiv->errname->str[0]) {
        return "<anonymous>";
    }

    return qiv->errname->str;
}
