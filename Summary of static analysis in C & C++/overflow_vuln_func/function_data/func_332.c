static void
reassemble_message(struct rlc_channel *ch, struct rlc_sdu *sdu, struct rlc_frag *frag)
{
	struct rlc_frag *temp;
	guint16 offs = 0;

	if (!sdu || !ch || !sdu->frags) return;

	if (sdu->data) return; /

	if (frag)
		sdu->reassembled_in = frag;
	else
		sdu->reassembled_in = sdu->last;

	sdu->data = se_alloc(sdu->len);

	temp = sdu->frags;
	while (temp) {
		memcpy(sdu->data + offs, temp->data, temp->len);
		/
		g_hash_table_insert(reassembled_table, temp, sdu);

		offs += temp->len;
		temp = temp->next;
	}
	g_hash_table_remove(fragment_table, ch);
}
