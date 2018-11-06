static QObject *parse_literal(JSONParserContext *ctxt)
{
    QObject *token, *obj;
    JSONParserContext saved_ctxt = parser_context_save(ctxt);

    token = parser_context_pop_token(ctxt);
    if (token == NULL) {
        goto out;
    }

    switch (token_get_type(token)) {
    case JSON_STRING:
        obj = QOBJECT(qstring_from_escaped_str(ctxt, token));
        break;
    case JSON_INTEGER:
        obj = QOBJECT(qint_from_int(strtoll(token_get_value(token), NULL, 10)));
        break;
    case JSON_FLOAT:
        /
        obj = QOBJECT(qfloat_from_double(strtod(token_get_value(token), NULL)));
        break;
    default:
        goto out;
    }

    return obj;

out:
    parser_context_restore(ctxt, saved_ctxt);

    return NULL;
}
