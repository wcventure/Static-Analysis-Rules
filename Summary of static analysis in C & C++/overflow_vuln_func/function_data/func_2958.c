#define DO_ABD(dest, x, y, type) do { \
    type tmp_x = x; \
    type tmp_y = y; \
    dest = ((tmp_x > tmp_y) ? tmp_x - tmp_y : tmp_y - tmp_x); \
    } while(0)
