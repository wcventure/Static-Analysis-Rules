void sdl_display_init(DisplayState *ds, int full_screen, int no_frame)
{
    int flags;
    uint8_t data = 0;
    const SDL_VideoInfo *vi;
    char *filename;

#if defined(__APPLE__)
    /
    if (!keyboard_layout)
        keyboard_layout = "en-us";
#endif
    if(keyboard_layout) {
        kbd_layout = init_keyboard_layout(name2keysym, keyboard_layout);
        if (!kbd_layout)
            exit(1);
    }

    if (no_frame)
        gui_noframe = 1;

    if (!full_screen) {
        setenv("SDL_VIDEO_ALLOW_SCREENSAVER", "1", 0);
    }
#ifdef __linux__
    /
    setenv("SDL_VIDEODRIVER", "x11", 0);
#endif

    /
    setenv("SDL_DISABLE_LOCK_KEYS", "1", 1);

    flags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;
    if (SDL_Init (flags)) {
        fprintf(stderr, "Could not initialize SDL(%s) - exiting\n",
                SDL_GetError());
        exit(1);
    }
    vi = SDL_GetVideoInfo();
    host_format = *(vi->vfmt);

    /
    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, "qemu-icon.bmp");
    if (filename) {
        SDL_Surface *image = SDL_LoadBMP(filename);
        if (image) {
            uint32_t colorkey = SDL_MapRGB(image->format, 255, 255, 255);
            SDL_SetColorKey(image, SDL_SRCCOLORKEY, colorkey);
            SDL_WM_SetIcon(image, NULL);
        }
        g_free(filename);
    }

    if (full_screen) {
        gui_fullscreen = 1;
        sdl_grab_start();
    }

    dcl = g_malloc0(sizeof(DisplayChangeListener));
    dcl->ops = &dcl_ops;
    register_displaychangelistener(dcl);

    mouse_mode_notifier.notify = sdl_mouse_mode_change;
    qemu_add_mouse_mode_change_notifier(&mouse_mode_notifier);

    sdl_update_caption();
    SDL_EnableKeyRepeat(250, 50);
    gui_grab = 0;

    sdl_cursor_hidden = SDL_CreateCursor(&data, &data, 8, 1, 0, 0);
    sdl_cursor_normal = SDL_GetCursor();

    atexit(sdl_cleanup);
}
