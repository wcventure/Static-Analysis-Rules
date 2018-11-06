static guint 
rsvp_hash (gconstpointer k)
{
    const struct rsvp_request_key *key = (const struct rsvp_request_key*) k;
    return key->conversation;
}
