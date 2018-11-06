void proto_register_x11(void)
{                 

/
      static hf_register_info hf[] = {
/
#include "x11-register-info.h"
      };

/
      static gint *ett[] = {
	    &ett_x11,
	    &ett_x11_request,
	    &ett_x11_color_flags,
	    &ett_x11_list_of_arc,
	    &ett_x11_arc,
	    &ett_x11_list_of_atom,
	    &ett_x11_list_of_card32,
	    &ett_x11_list_of_color_item,
	    &ett_x11_color_item,
	    &ett_x11_list_of_keycode,
	    &ett_x11_list_of_keysyms,
	    &ett_x11_keysym,
	    &ett_x11_list_of_point,
	    &ett_x11_point,
	    &ett_x11_list_of_rectangle,
	    &ett_x11_rectangle,
	    &ett_x11_list_of_segment,
	    &ett_x11_segment,
	    &ett_x11_list_of_string8,
	    &ett_x11_list_of_text_item,
	    &ett_x11_text_item,
	    &ett_x11_gc_value_mask,
	    &ett_x11_event_mask,
	    &ett_x11_do_not_propagate_mask,
	    &ett_x11_set_of_key_mask,
	    &ett_x11_pointer_event_mask,
	    &ett_x11_window_value_mask,
	    &ett_x11_configure_window_mask,
	    &ett_x11_keyboard_value_mask,
      };

/
      proto_x11 = proto_register_protocol("X11", "X11", "x11");

/
      proto_register_field_array(proto_x11, hf, array_length(hf));
      proto_register_subtree_array(ett, array_length(ett));

      byte_ordering_cache = g_tree_new(compareAddresses);
      address_chunk = g_mem_chunk_new("x11 byte ordering address cache", sizeof(address), 
				      sizeof(address) * 128, G_ALLOC_ONLY);
      ipv4_chunk = g_mem_chunk_new("x11 byte ordering ipv4 address cache", 4, 4 * 128, G_ALLOC_ONLY);
      ipv6_chunk = g_mem_chunk_new("x11 byte ordering ipv6 address cache", 16, 16 * 128, G_ALLOC_ONLY);
};
