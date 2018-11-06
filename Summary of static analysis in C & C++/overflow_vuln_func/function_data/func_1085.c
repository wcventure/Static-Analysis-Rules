static gboolean
get_file_time_stamp(gchar *linebuff, time_t *secs, guint32 *usecs)
{
    struct tm tm;
    #define MAX_MONTH_LETTERS 9
    char month[MAX_MONTH_LETTERS+1];

    int day, year, hour, minute, second;
    int scan_found;

    /
    if (strlen(linebuff) > MAX_TIMESTAMP_LINE_LENGTH) {
        return FALSE;
    }

    /
    /
    scan_found = sscanf(linebuff, "%10s %2d, %4d     %2d:%2d:%2d.%4u",
                        month, &day, &year, &hour, &minute, &second, usecs);
    if (scan_found != 7) {
        /
        return FALSE;
    }

    if      (strcmp(month, "January"  ) == 0)  tm.tm_mon = 0;
    else if (strcmp(month, "February" ) == 0)  tm.tm_mon = 1;
    else if (strcmp(month, "March"    ) == 0)  tm.tm_mon = 2;
    else if (strcmp(month, "April"    ) == 0)  tm.tm_mon = 3;
    else if (strcmp(month, "May"      ) == 0)  tm.tm_mon = 4;
    else if (strcmp(month, "June"     ) == 0)  tm.tm_mon = 5;
    else if (strcmp(month, "July"     ) == 0)  tm.tm_mon = 6;
    else if (strcmp(month, "August"   ) == 0)  tm.tm_mon = 7;
    else if (strcmp(month, "September") == 0)  tm.tm_mon = 8;
    else if (strcmp(month, "October"  ) == 0)  tm.tm_mon = 9;
    else if (strcmp(month, "November" ) == 0)  tm.tm_mon = 10;
    else if (strcmp(month, "December" ) == 0)  tm.tm_mon = 11;
    else {
        /
        return FALSE;
    }

    /
    /
    tm.tm_year = year - 1900;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1;    /

    /
    *secs = mktime(&tm);

    /
    *usecs = *usecs * 100;

    return TRUE;
}
