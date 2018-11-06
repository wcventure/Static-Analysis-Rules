static void
dissect_rsvp_label_request (proto_item *ti, proto_tree *rsvp_object_tree,
			    tvbuff_t *tvb,
			    int offset, int obj_length,
			    int class _U_, int type)
{
    int offset2 = offset + 4;

    switch(type) {
    case 1: {
	unsigned short l3pid = tvb_get_ntohs(tvb, offset2+2);
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 2,
			    "L3PID: %s (0x%04x)",
			    val_to_str(l3pid, etype_vals, "Unknown"),
			    l3pid);
	proto_item_set_text(ti, "LABEL REQUEST: Basic: L3PID: %s (0x%04x)",
			    val_to_str(l3pid, etype_vals, "Unknown"),
			    l3pid);
	break;
    }

    case 2: {
	unsigned short l3pid = tvb_get_ntohs(tvb, offset2+2);
	unsigned short min_vpi, min_vci, max_vpi, max_vci;
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 2 (Label Request with ATM label Range)");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 2,
			    "L3PID: %s (0x%04x)",
			    val_to_str(l3pid, etype_vals, "Unknown"),
			    l3pid);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 1, 
			    "M: %s Merge in Data Plane",
			    (tvb_get_guint8(tvb, offset2+4) & 0x80) ? 
			    "1: Can" : "0: Cannot");
	min_vpi = tvb_get_ntohs(tvb, offset2+4) & 0x7f;
	min_vci = tvb_get_ntohs(tvb, offset2+6);
	max_vpi = tvb_get_ntohs(tvb, offset2+8) & 0x7f;
	max_vci = tvb_get_ntohs(tvb, offset2+10);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 2,
			    "Min VPI: %d", min_vpi);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+6, 2,
			    "Min VCI: %d", min_vci);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+8, 2,
			    "Max VPI: %d", max_vpi);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+10, 2,
			    "Max VCI: %d", max_vci);
	proto_item_set_text(ti, "LABEL REQUEST: ATM: L3PID: %s (0x%04x). VPI/VCI: Min: %d/%d, Max: %d/%d. %s Merge. ",
			    val_to_str(l3pid, etype_vals, "Unknown"), l3pid,
			    min_vpi, min_vci, max_vpi, max_vci, 
			    (tvb_get_guint8(tvb, offset2+4) & 0x80) ? "Can" : "Cannot");
	break;
    }

    case 3: {
	guint16 l3pid = tvb_get_ntohs(tvb, offset2+2);
	guint32 min_dlci, max_dlci, dlci_len, dlci_len_code;
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 2 (Label Request with ATM label Range)");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 2,
			    "L3PID: %s (0x%04x)",
			    val_to_str(l3pid, etype_vals, "Unknown"),
			    l3pid);
	dlci_len_code = (tvb_get_ntohs(tvb, offset2+4) & 0x0180) >> 7; 
	min_dlci = tvb_get_ntohl(tvb, offset2+4) & 0x7fffff;
	max_dlci = tvb_get_ntohl(tvb, offset2+8) & 0x7fffff;
	switch(dlci_len_code) {
	case 0: 
	    /
	    dlci_len = 10;
	    min_dlci &= 0x3ff;
	    max_dlci &= 0x3ff;
	case 2:
	    dlci_len = 23;
	default:
	    dlci_len = 0;
	    min_dlci = 0;
	    max_dlci = 0;
	}
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 2, 
			    "DLCI Length: %s (%d)", 
			    dlci_len==10 ? "10 bits" : 
			    dlci_len==23 ? "23 bits" : 
			    "INVALID", dlci_len_code);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+5, 3,
			    "Min DLCI: %d", min_dlci);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+8, 2,
			    "Max DLCI: %d", max_dlci);
	proto_item_set_text(ti, "LABEL REQUEST: Frame: L3PID: %s (0x%04x). DLCI Len: %s. Min DLCI: %d. Max DLCI: %d",
			    val_to_str(l3pid, etype_vals, "Unknown"), l3pid,
			    dlci_len==10 ? "10 bits" : 
			    dlci_len==23 ? "23 bits" : 
			    "INVALID", min_dlci, max_dlci);
	break;
    }
    case 4: {
	unsigned short l3pid = tvb_get_ntohs(tvb, offset2+2);
	unsigned char  lsp_enc = tvb_get_guint8(tvb,offset2);
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 4 (Generalized Label Request)");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 1,
			    "LSP Encoding Type: %s",
			    val_to_str(lsp_enc, gmpls_lsp_enc_str, "Unknown (%d)"));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+1, 1,
			    "Switching Type: %s",
			    val_to_str(tvb_get_guint8(tvb,offset2+1),
				       gmpls_switching_type_str, "Unknown (%d)"));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 2,
			    "G-PID: %s (0x%0x)",
			    val_to_str(l3pid, gmpls_gpid_str,
				       val_to_str(l3pid, etype_vals,
						  "Unknown G-PID(0x%04x)")),
			    l3pid);
	proto_item_set_text(ti, "LABEL REQUEST: Generalized: LSP Encoding=%s, "
			    "Switching Type=%s, G-PID=%s ",
			    val_to_str(lsp_enc, gmpls_lsp_enc_str, "Unknown (%d)"),
			    val_to_str(tvb_get_guint8(tvb,offset2+1),
				       gmpls_switching_type_str, "Unknown (%d)"),
			    val_to_str(l3pid, gmpls_gpid_str,
				       val_to_str(l3pid, etype_vals,
						  "Unknown (0x%04x)")));
	break;
    }

    default: {
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)",
			    type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
	break;
    }
    }
}
