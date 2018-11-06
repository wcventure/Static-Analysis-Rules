static int
get_nbns_name(const u_char *pd, int offset, int nbns_data_offset,
    char *name_ret, int *name_type_ret)
{
	int name_len;
	char name[MAXDNAME];
	char nbname[NETBIOS_NAME_LEN];
	char *pname, *pnbname, cname, cnbname;
	int name_type;

	name_len = get_dns_name(pd, offset, nbns_data_offset, name,
	    sizeof(name));

	/
	pname = &name[0];
	pnbname = &nbname[0];
	for (;;) {
		/
		cname = *pname;
		if (cname == '\0')
			break;		/
		if (cname == '.')
			break;		/
		if (cname < 'A' || cname > 'Z') {
			/
			strcpy(nbname,
			    "Illegal NetBIOS name (character not between A and Z in first-level encoding)");
			goto bad;
		}
		cname -= 'A';
		cnbname = cname << 4;
		pname++;

		cname = *pname;
		if (cname == '\0' || cname == '.') {
			/
			strcpy(nbname,
			    "Illegal NetBIOS name (odd number of bytes)");
			goto bad;
		}
		if (cname < 'A' || cname > 'Z') {
			/
			strcpy(nbname,
			    "Illegal NetBIOS name (character not between A and Z in first-level encoding)");
			goto bad;
		}
		cname -= 'A';
		cnbname |= cname;
		pname++;

		/
		if (pnbname < &nbname[NETBIOS_NAME_LEN]) {
			/
			*pnbname = cnbname;
		}

		/
		pnbname++;
	}

	/
	if (pnbname - nbname != NETBIOS_NAME_LEN) {
		/
		sprintf(nbname, "Illegal NetBIOS name (%ld bytes long)",
		    (long)(pnbname - nbname));
		goto bad;
	}

	/
	name_type = process_netbios_name(nbname, name_ret);
	name_ret += strlen(name_ret);
	sprintf(name_ret, "<%02x>", name_type);
	name_ret += 4;
	if (cname == '.') {
		/
		strcpy(name_ret, pname);
	}
	if (name_type_ret != NULL)
		*name_type_ret = name_type;
	return name_len;

bad:
	if (name_type_ret != NULL)
		*name_type_ret = -1;
	strcpy (name_ret, nbname);
	return name_len;
}
