DisplaySurface *qemu_create_displaysurface_guestmem(int width, int height,
                                                    pixman_format_code_t format,
                                                    int linesize, uint64_t addr)
{
    DisplaySurface *surface;
    hwaddr size;
    void *data;

    if (linesize == 0) {
        linesize = width * PIXMAN_FORMAT_BPP(format) / 8;
    }

    size = linesize * height;
    data = cpu_physical_memory_map(addr, &size, 0);
    if (size != linesize * height) {
        cpu_physical_memory_unmap(data, size, 0, 0);
        return NULL;
    }

    surface = qemu_create_displaysurface_from
        (width, height, format, linesize, data);
    pixman_image_set_destroy_function
        (surface->image, qemu_unmap_displaysurface_guestmem, NULL);

    return surface;
}
