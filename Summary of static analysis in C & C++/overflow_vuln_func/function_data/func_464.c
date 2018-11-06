static gboolean
fragment_add_work(fragment_data *fd_head, tvbuff_t *tvb, int offset,
	     packet_info *pinfo, guint32 frag_offset,
	     guint32 frag_data_len, gboolean more_frags)
{
	fragment_data *fd;
	fragment_data *fd_i;
	guint32 max, dfpos;
	unsigned char *old_data;

	/
	fd = g_mem_chunk_alloc(fragment_data_chunk);
	fd->next = NULL;
	fd->flags = 0;
	fd->frame = pinfo->fd->num;
	if (fd->frame > fd_head->frame)
	    fd_head->frame = fd->frame;
	fd->offset = frag_offset;
	fd->len  = frag_data_len;
	fd->data = NULL;

	/
	if(fd_head->flags & FD_DEFRAGMENTED && (frag_offset+frag_data_len) >= fd_head->datalen &&
		fd_head->flags & FD_PARTIAL_REASSEMBLY){
		for(fd_i=fd_head->next; fd_i; fd_i=fd_i->next){
			if( !fd_i->data ) {
				fd_i->data = fd_head->data + fd_i->offset;
				fd_i->flags |= FD_NOT_MALLOCED;
			}
			fd_i->flags &= (~FD_TOOLONGFRAGMENT) & (~FD_MULTIPLETAILS);
		}
		fd_head->flags &= ~(FD_DEFRAGMENTED|FD_PARTIAL_REASSEMBLY|FD_DATALEN_SET);
		fd_head->flags &= (~FD_TOOLONGFRAGMENT) & (~FD_MULTIPLETAILS);
		fd_head->datalen=0;
		fd_head->reassembled_in=0;
	}

	if (!more_frags) {
		/
		if (fd_head->flags & FD_DATALEN_SET) {
			/
			if (fd_head->datalen != (fd->offset + fd->len) ){
				/
				fd->flags      |= FD_MULTIPLETAILS;
				fd_head->flags |= FD_MULTIPLETAILS;
			}
		} else {
			/
			fd_head->datalen = fd->offset + fd->len;
			fd_head->flags |= FD_DATALEN_SET;
		}
	}




	/
	if (fd_head->flags & FD_DEFRAGMENTED) {
		fd->flags      |= FD_OVERLAP;
		fd_head->flags |= FD_OVERLAP;
		/
		if (fd->offset + fd->len > fd_head->datalen) {
			fd->flags      |= FD_TOOLONGFRAGMENT;
			fd_head->flags |= FD_TOOLONGFRAGMENT;
		}
		/
		else if ( memcmp(fd_head->data+fd->offset,
			tvb_get_ptr(tvb,offset,fd->len),fd->len) ){
			fd->flags      |= FD_OVERLAPCONFLICT;
			fd_head->flags |= FD_OVERLAPCONFLICT;
		}
		/
		LINK_FRAG(fd_head,fd);
		return TRUE;
	}



	/
	fd->data = g_malloc(fd->len);
	tvb_memcpy(tvb, fd->data, offset, fd->len);
	LINK_FRAG(fd_head,fd);


	if( !(fd_head->flags & FD_DATALEN_SET) ){
		/
		return FALSE;
	}


	/
	max = 0;
	for (fd_i=fd_head->next;fd_i;fd_i=fd_i->next) {
		if ( ((fd_i->offset)<=max) &&
		    ((fd_i->offset+fd_i->len)>max) ){
			max = fd_i->offset+fd_i->len;
		}
	}

	if (max < (fd_head->datalen)) {
		/
		return FALSE;
	}


	if (max > (fd_head->datalen)) {
		/
		/
		/
		fd->flags      |= FD_TOOLONGFRAGMENT;
		fd_head->flags |= FD_TOOLONGFRAGMENT;
	}

	/
	/
	old_data=fd_head->data;
	fd_head->data = g_malloc(max);

	/
	for (dfpos=0,fd_i=fd_head;fd_i;fd_i=fd_i->next) {
		if (fd_i->len) {
			/
			/
			/
			/
			/
			    
			if ( fd_i->offset+fd_i->len > dfpos ) {
				if (fd_i->offset+fd_i->len > max)
					g_warning("Reassemble error in frame %u: offset %u + len %u > max %u",
					    pinfo->fd->num, fd_i->offset,
					    fd_i->len, max);
				else if (dfpos < fd_i->offset)
					g_warning("Reassemble error in frame %u: dfpos %u < offset %u",
					    pinfo->fd->num, dfpos, fd_i->offset);
				else if (dfpos-fd_i->offset > fd_i->len)
					g_warning("Reassemble error in frame %u: dfpos %u - offset %u > len %u",
					    pinfo->fd->num, dfpos, fd_i->offset,
					    fd_i->len);
				else {
					if (fd_i->offset < dfpos) {
						fd_i->flags    |= FD_OVERLAP;
						fd_head->flags |= FD_OVERLAP;
						if ( memcmp(fd_head->data+fd_i->offset,
							    fd_i->data,
							    MIN(fd_i->len,(dfpos-fd_i->offset))
							     ) ) {
							fd_i->flags    |= FD_OVERLAPCONFLICT;
							fd_head->flags |= FD_OVERLAPCONFLICT;
						}
					}
					memcpy(fd_head->data+dfpos,
					    fd_i->data+(dfpos-fd_i->offset),
					    fd_i->len-(dfpos-fd_i->offset));
				}
			} else {
				if (fd_i->offset+fd_i->len < fd_i->offset)
					g_warning("Reassemble error in frame %u: offset %u + len %u < offset",
					    pinfo->fd->num, fd_i->offset,
					    fd_i->len);
			}
			if( fd_i->flags & FD_NOT_MALLOCED )
				fd_i->flags &= ~FD_NOT_MALLOCED;
			else
				g_free(fd_i->data);
			fd_i->data=NULL;

			dfpos=MAX(dfpos,(fd_i->offset+fd_i->len));
		}
	}

	g_free(old_data);
	/
	fd_head->flags |= FD_DEFRAGMENTED;
	fd_head->reassembled_in=pinfo->fd->num;

	return TRUE;
}
