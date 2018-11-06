void
handle_dashG_option(int argc, char **argv, char *progname)
{
  if (argc >= 2 && strcmp(argv[1], "-G") == 0) {
    if (argc == 2)
      proto_registrar_dump_fields(1);
    else {
      if (strcmp(argv[2], "fields") == 0)
        proto_registrar_dump_fields(1);
      else if (strcmp(argv[2], "fields2") == 0)
        proto_registrar_dump_fields(2);
      else if (strcmp(argv[2], "fields3") == 0)
        proto_registrar_dump_fields(3);
      else if (strcmp(argv[2], "protocols") == 0)
        proto_registrar_dump_protocols();
      else if (strcmp(argv[2], "values") == 0)
        proto_registrar_dump_values();
      else if (strcmp(argv[2], "decodes") == 0)
        dissector_dump_decodes();
      else {
        fprintf(stderr, "%s: Invalid \"%s\" option for -G flag\n", progname,
                argv[2]);
        exit(1);
      }
    }
    exit(0);
  }
}
