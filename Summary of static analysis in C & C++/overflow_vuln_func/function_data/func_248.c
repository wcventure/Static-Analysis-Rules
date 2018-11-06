PUBLIC int
#ifdef __STDC__
vsnprintf(char *string, size_t length, const char * format, va_list args)
#else
vsnprintf(string, length, format, args)
char *string;
size_t length;
char * format;
va_list args;
#endif
{
  struct DATA data;
  char conv_field[MAX_FIELD];
  double d; /
  int state;
  int i;

  data.length = length - 1; /
  data.holder = string;
  data.pf = format;
  data.counter = 0;


/
  if (length < 1)
    return -1;


  for (; *data.pf && (data.counter < data.length); data.pf++) {
    if ( *data.pf == '%' ) { /
      conv_flag((char *)0, &data); /
      for (state = 1; *data.pf && state;) {
        switch (*(++data.pf)) {
          case '\0': /
            *data.holder = '\0';
            return data.counter;
            break;
          case 'f':  /
            STAR_ARGS(&data);
            d = va_arg(args, double);
            floating(&data, d);  
            state = 0;
            break;
          case 'g': 
          case 'G':
            STAR_ARGS(&data);
            DEF_PREC(&data);
            d = va_arg(args, double);
            i = log_10(d);
            /
            if (-4 < i && i < data.precision)
              floating(&data, d);
            else
              exponent(&data, d);
            state = 0;
            break;
          case 'e':
          case 'E':  /
            STAR_ARGS(&data);
            d = va_arg(args, double);
            exponent(&data, d);
            state = 0;
            break;
          case 'u':
          case 'd':  /
            STAR_ARGS(&data);
            if (data.a_long == FOUND)
              d = va_arg(args, long);
            else
              d = va_arg(args, int);
            decimal(&data, d);
            state = 0;
            break;
          case 'o':  /
            STAR_ARGS(&data);
            if (data.a_long == FOUND)
              d = va_arg(args, long);
            else
              d = va_arg(args, int);
            octal(&data, d);
            state = 0;
            break;
          case 'x': 
          case 'X':  /
            STAR_ARGS(&data);
            if (data.a_long == FOUND)
              d = va_arg(args, long);
            else
              d = va_arg(args, int);
            hexa(&data, d);
            state = 0;
            break;
          case 'c': /
            d = va_arg(args, int);
            PUT_CHAR(d, &data);
            state = 0;
            break;
          case 's':  /
            STAR_ARGS(&data);
            strings(&data, va_arg(args, char *));
            state = 0;
            break;
          case 'n':
             *(va_arg(args, int *)) = data.counter; /
             state = 0;
             break;
          case 'l':
            data.a_long = FOUND;
            break;
          case 'h':
            break;
          case '%':  /
            PUT_CHAR('%', &data);
            state = 0;
            break;
          case '#': case ' ': case '+': case '*':
          case '-': case '.': case '0': case '1': 
          case '2': case '3': case '4': case '5':
          case '6': case '7': case '8': case '9':
           /
            for (i = 0; isflag(*data.pf); i++, data.pf++) 
              if (i < MAX_FIELD - 1)
                conv_field[i] = *data.pf;
            conv_field[i] = '\0';
            conv_flag(conv_field, &data);
            data.pf--;   /
            break;
          default:
            /
            state = 0;
            break;
        } /
      } /
    } else { /
      PUT_CHAR(*data.pf, &data);  /
    }
  }

  *data.holder = '\0'; /

  return data.counter;
}
