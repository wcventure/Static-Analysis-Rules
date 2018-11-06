int main(int argc, char **argv)
{
    int ret;

    g_test_init(&argc, &argv, NULL);
    qtest_add_func("/spapr-phb/device", test_phb_device);

    qtest_start("-device " TYPE_SPAPR_PCI_HOST_BRIDGE ",index=100");

    ret = g_test_run();

    qtest_end();

    return ret;
}
