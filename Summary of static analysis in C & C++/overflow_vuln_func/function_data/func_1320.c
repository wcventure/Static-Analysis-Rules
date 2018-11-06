static void
print_value(ASN1_SCK *asn,proto_tree *pres_tree,tvbuff_t *tvb,int 
*offset,int item_len)
{
	  gint    start = *offset;
	  char    tmp[MAXSTRING];
		*offset = asn->offset;  /
		string_to_hex(tvb_get_ptr(tvb,*offset,item_len),tmp,item_len);
		proto_tree_add_text(pres_tree, tvb, *offset, item_len, tmp);
		(*offset)=start+item_len;
		asn->offset = (*offset);
}
