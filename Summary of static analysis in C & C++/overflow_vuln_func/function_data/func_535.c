static void
write_pref(gpointer data, gpointer user_data)
{
	pref_t *pref = data;
	write_pref_arg_t *arg = user_data;
	const enum_val_t *enum_valp;
	const char *val_string;

	if (pref->type == PREF_OBSOLETE) {
		/
		return;
	}

	fprintf(arg->pf, "\n# %s\n", pref->description);

	switch (pref->type) {

	case PREF_UINT:
		switch (pref->info.base) {

		case 10:
			fprintf(arg->pf, "# A decimal number.\n");
			fprintf(arg->pf, "%s.%s: %u\n", arg->module->name,
			    pref->name, *pref->varp.uint);
			break;

		case 8:
			fprintf(arg->pf, "# An octal number.\n");
			fprintf(arg->pf, "%s.%s: %#o\n", arg->module->name,
			    pref->name, *pref->varp.uint);
			break;

		case 16:
			fprintf(arg->pf, "# A hexadecimal number.\n");
			fprintf(arg->pf, "%s.%s: %#x\n", arg->module->name,
			    pref->name, *pref->varp.uint);
			break;
		}
		break;

	case PREF_BOOL:
		fprintf(arg->pf, "# TRUE or FALSE (case-insensitive).\n");
		fprintf(arg->pf, "%s.%s: %s\n", arg->module->name, pref->name,
		    *pref->varp.boolp ? "TRUE" : "FALSE");
		break;

	case PREF_ENUM:
		/
		fprintf(arg->pf, "# One of: ");
		enum_valp = pref->info.enum_info.enumvals;
		val_string = NULL;
		while (enum_valp->name != NULL) {
			if (enum_valp->value == *pref->varp.enump)
				val_string = enum_valp->description;
			fprintf(arg->pf, "%s", enum_valp->description);
			enum_valp++;
			if (enum_valp->name == NULL)
				fprintf(arg->pf, "\n");
			else
				fprintf(arg->pf, ", ");
		}
		fprintf(arg->pf, "# (case-insensitive).\n");
		fprintf(arg->pf, "%s.%s: %s\n", arg->module->name,
		    pref->name, val_string);
		break;

	case PREF_STRING:
		fprintf(arg->pf, "# A string.\n");
		fprintf(arg->pf, "%s.%s: %s\n", arg->module->name, pref->name,
		    *pref->varp.string);
		break;

	case PREF_RANGE:
	{
		char range_string[MAXRANGESTRING];

		fprintf(arg->pf, "# A string denoting an positive integer range (e.g., \"1-20,30-40\").\n");
		fprintf(arg->pf, "%s.%s: %s\n", arg->module->name, pref->name,
			range_convert_range(*pref->varp.range, range_string));
		break;
	}

	case PREF_OBSOLETE:
		g_assert_not_reached();
		break;
	}
}
