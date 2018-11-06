static void tmu2_start(MilkymistTMU2State *s)
{
    int pbuffer_attrib[6] = {
        GLX_PBUFFER_WIDTH,
        0,
        GLX_PBUFFER_HEIGHT,
        0,
        GLX_PRESERVED_CONTENTS,
        True
    };

    GLXPbuffer pbuffer;
    GLuint texture;
    void *fb;
    hwaddr fb_len;
    void *mesh;
    hwaddr mesh_len;
    float m;

    trace_milkymist_tmu2_start();

    /
    pbuffer_attrib[1] = s->regs[R_DSTHRES];
    pbuffer_attrib[3] = s->regs[R_DSTVRES];
    pbuffer = glXCreatePbuffer(s->dpy, s->glx_fb_config, pbuffer_attrib);
    glXMakeContextCurrent(s->dpy, pbuffer, pbuffer, s->glx_context);

    /
    glPixelStorei(GL_UNPACK_SWAP_BYTES, 1);
    glPixelStorei(GL_PACK_SWAP_BYTES, 1);

    /
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    glPixelStorei(GL_PACK_ALIGNMENT, 2);

    /
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    fb_len = 2ULL * s->regs[R_TEXHRES] * s->regs[R_TEXVRES];
    fb = cpu_physical_memory_map(s->regs[R_TEXFBUF], &fb_len, 0);
    if (fb == NULL) {
        glDeleteTextures(1, &texture);
        glXMakeContextCurrent(s->dpy, None, None, NULL);
        glXDestroyPbuffer(s->dpy, pbuffer);
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 3, s->regs[R_TEXHRES], s->regs[R_TEXVRES],
            0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, fb);
    cpu_physical_memory_unmap(fb, fb_len, 0, fb_len);

    /
    /
    if ((s->regs[R_TEXHMASK] & 0x3f) > 0x20) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    if ((s->regs[R_TEXHMASK] >> 6) & s->regs[R_TEXHRES]) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    }
    if ((s->regs[R_TEXVMASK] >> 6) & s->regs[R_TEXVRES]) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    /
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m = (float)(s->regs[R_BRIGHTNESS] + 1) / 64.0f;
    glColor4f(m, m, m, (float)(s->regs[R_ALPHA] + 1) / 64.0f);

    /
    fb_len = 2ULL * s->regs[R_DSTHRES] * s->regs[R_DSTVRES];
    fb = cpu_physical_memory_map(s->regs[R_DSTFBUF], &fb_len, 0);
    if (fb == NULL) {
        glDeleteTextures(1, &texture);
        glXMakeContextCurrent(s->dpy, None, None, NULL);
        glXDestroyPbuffer(s->dpy, pbuffer);
        return;
    }

    glDrawPixels(s->regs[R_DSTHRES], s->regs[R_DSTVRES], GL_RGB,
            GL_UNSIGNED_SHORT_5_6_5, fb);
    cpu_physical_memory_unmap(fb, fb_len, 0, fb_len);
    glViewport(0, 0, s->regs[R_DSTHRES], s->regs[R_DSTVRES]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, s->regs[R_DSTHRES], 0.0, s->regs[R_DSTVRES], -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    /
    mesh_len = MESH_MAXSIZE*MESH_MAXSIZE*sizeof(struct vertex);
    mesh = cpu_physical_memory_map(s->regs[R_VERTICESADDR], &mesh_len, 0);
    if (mesh == NULL) {
        glDeleteTextures(1, &texture);
        glXMakeContextCurrent(s->dpy, None, None, NULL);
        glXDestroyPbuffer(s->dpy, pbuffer);
        return;
    }

    tmu2_gl_map((struct vertex *)mesh,
        s->regs[R_TEXHRES], s->regs[R_TEXVRES],
        s->regs[R_HMESHLAST], s->regs[R_VMESHLAST],
        s->regs[R_DSTHOFFSET], s->regs[R_DSTVOFFSET],
        s->regs[R_DSTSQUAREW], s->regs[R_DSTSQUAREH]);
    cpu_physical_memory_unmap(mesh, mesh_len, 0, mesh_len);

    /
    fb_len = 2 * s->regs[R_DSTHRES] * s->regs[R_DSTVRES];
    fb = cpu_physical_memory_map(s->regs[R_DSTFBUF], &fb_len, 1);
    if (fb == NULL) {
        glDeleteTextures(1, &texture);
        glXMakeContextCurrent(s->dpy, None, None, NULL);
        glXDestroyPbuffer(s->dpy, pbuffer);
        return;
    }

    glReadPixels(0, 0, s->regs[R_DSTHRES], s->regs[R_DSTVRES], GL_RGB,
            GL_UNSIGNED_SHORT_5_6_5, fb);
    cpu_physical_memory_unmap(fb, fb_len, 1, fb_len);

    /
    glDeleteTextures(1, &texture);
    glXMakeContextCurrent(s->dpy, None, None, NULL);
    glXDestroyPbuffer(s->dpy, pbuffer);

    s->regs[R_CTL] &= ~CTL_START_BUSY;

    trace_milkymist_tmu2_pulse_irq();
    qemu_irq_pulse(s->irq);
}
