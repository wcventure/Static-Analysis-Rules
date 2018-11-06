static int compare_ocl_device_desc(const void *a, const void *b)
{
    return ((const OpenCLDeviceBenchmark*)a)->runtime - ((const OpenCLDeviceBenchmark*)b)->runtime;
}
