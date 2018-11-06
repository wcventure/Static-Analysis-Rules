static void term_print_cmdline (const char *cmdline)
{
    term_show_prompt();
    term_printf(cmdline);
    term_flush();
}
