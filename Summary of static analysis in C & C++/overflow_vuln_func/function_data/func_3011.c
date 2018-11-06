CharDriverState *chr_testdev_init(void)
{
    TestdevCharState *testdev;
    CharDriverState *chr;

    testdev = g_malloc0(sizeof(TestdevCharState));
    testdev->chr = chr = g_malloc0(sizeof(CharDriverState));

    chr->opaque = testdev;
    chr->chr_write = testdev_write;
    chr->chr_close = testdev_close;

    return chr;
}
