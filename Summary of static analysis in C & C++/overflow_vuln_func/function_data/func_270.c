guint oid_string2subid(const char* str, guint32** subids_p) {
	const char* r = str;
	guint32* subids;
	guint n = check_num_oid(str);
	
	D(6,("oid_string2subid: str='%s'",str));

	if (!n) {
		*subids_p = NULL;
		return 0;
	}

	D(7,("\toid_string2subid: n=%d",n));

	*subids_p = subids = ep_alloc_array(guint32,n);
	
	do switch(*r) {
		case '.':
			D(7,("\toid_string2subid: subid: %p %u",subids,*subids));
			subids++;
			continue;
		case '1' : case '2' : case '3' : case '4' : case '5' : 
		case '6' : case '7' : case '8' : case '9' : case '0' :
			*(subids) *= 10;
			*(subids) += *r - '0';
			continue;
		case '\0':
			break;
		default:
			return 0;
	} while(*r++);
	
	D(7,("\toid_string2subid: ret %u",n));
	return n;
}
