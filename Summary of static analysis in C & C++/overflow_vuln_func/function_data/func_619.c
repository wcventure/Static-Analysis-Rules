static gint  asp_equal (gconstpointer v, gconstpointer v2)
{
	asp_request_key *val1 = (asp_request_key*)v;
	asp_request_key *val2 = (asp_request_key*)v2;

	if (val1->conversation == val2->conversation &&
			val1->seq == val2->seq) {
		return 1;
	}
	return 0;
}
