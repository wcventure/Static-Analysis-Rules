static void vmsvga_fifo_run(struct vmsvga_state_s *s)
{
    uint32_t cmd, colour;
    int args = 0;
    int x, y, dx, dy, width, height;
    struct vmsvga_cursor_definition_s cursor;
    while (!vmsvga_fifo_empty(s))
        switch (cmd = vmsvga_fifo_read(s)) {
        case SVGA_CMD_UPDATE:
        case SVGA_CMD_UPDATE_VERBOSE:
            x = vmsvga_fifo_read(s);
            y = vmsvga_fifo_read(s);
            width = vmsvga_fifo_read(s);
            height = vmsvga_fifo_read(s);
            vmsvga_update_rect_delayed(s, x, y, width, height);
            break;

        case SVGA_CMD_RECT_FILL:
            colour = vmsvga_fifo_read(s);
            x = vmsvga_fifo_read(s);
            y = vmsvga_fifo_read(s);
            width = vmsvga_fifo_read(s);
            height = vmsvga_fifo_read(s);
#ifdef HW_FILL_ACCEL
            vmsvga_fill_rect(s, colour, x, y, width, height);
            break;
#else
            goto badcmd;
#endif

        case SVGA_CMD_RECT_COPY:
            x = vmsvga_fifo_read(s);
            y = vmsvga_fifo_read(s);
            dx = vmsvga_fifo_read(s);
            dy = vmsvga_fifo_read(s);
            width = vmsvga_fifo_read(s);
            height = vmsvga_fifo_read(s);
#ifdef HW_RECT_ACCEL
            vmsvga_copy_rect(s, x, y, dx, dy, width, height);
            break;
#else
            goto badcmd;
#endif

        case SVGA_CMD_DEFINE_CURSOR:
            cursor.id = vmsvga_fifo_read(s);
            cursor.hot_x = vmsvga_fifo_read(s);
            cursor.hot_y = vmsvga_fifo_read(s);
            cursor.width = x = vmsvga_fifo_read(s);
            cursor.height = y = vmsvga_fifo_read(s);
            vmsvga_fifo_read(s);
            cursor.bpp = vmsvga_fifo_read(s);

	    if (SVGA_BITMAP_SIZE(x, y) > sizeof cursor.mask ||
		SVGA_PIXMAP_SIZE(x, y, cursor.bpp) > sizeof cursor.image) {
		    args = SVGA_BITMAP_SIZE(x, y) + SVGA_PIXMAP_SIZE(x, y, cursor.bpp);
		    goto badcmd;
	    }

            for (args = 0; args < SVGA_BITMAP_SIZE(x, y); args ++)
                cursor.mask[args] = vmsvga_fifo_read_raw(s);
            for (args = 0; args < SVGA_PIXMAP_SIZE(x, y, cursor.bpp); args ++)
                cursor.image[args] = vmsvga_fifo_read_raw(s);
#ifdef HW_MOUSE_ACCEL
            vmsvga_cursor_define(s, &cursor);
            break;
#else
            args = 0;
            goto badcmd;
#endif

        /
        case SVGA_CMD_DEFINE_ALPHA_CURSOR:
            vmsvga_fifo_read(s);
            vmsvga_fifo_read(s);
            vmsvga_fifo_read(s);
            x = vmsvga_fifo_read(s);
            y = vmsvga_fifo_read(s);
            args = x * y;
            goto badcmd;
        case SVGA_CMD_RECT_ROP_FILL:
            args = 6;
            goto badcmd;
        case SVGA_CMD_RECT_ROP_COPY:
            args = 7;
            goto badcmd;
        case SVGA_CMD_DRAW_GLYPH_CLIPPED:
            vmsvga_fifo_read(s);
            vmsvga_fifo_read(s);
            args = 7 + (vmsvga_fifo_read(s) >> 2);
            goto badcmd;
        case SVGA_CMD_SURFACE_ALPHA_BLEND:
            args = 12;
            goto badcmd;

        /
        case SVGA_CMD_SURFACE_FILL:
        case SVGA_CMD_SURFACE_COPY:
        case SVGA_CMD_FRONT_ROP_FILL:
        case SVGA_CMD_FENCE:
        case SVGA_CMD_INVALID_CMD:
            break; /

        default:
        badcmd:
            while (args --)
                vmsvga_fifo_read(s);
            printf("%s: Unknown command 0x%02x in SVGA command FIFO\n",
                            __FUNCTION__, cmd);
            break;
        }

    s->syncing = 0;
}
