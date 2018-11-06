int opt_opencl_bench(void *optctx, const char *opt, const char *arg)
{
    int i, j, nb_devices = 0, count = 0;
    int64_t score = 0;
    AVOpenCLDeviceList *device_list;
    AVOpenCLDeviceNode *device_node = NULL;
    OpenCLDeviceBenchmark *devices = NULL;
    cl_platform_id platform;

    av_opencl_get_device_list(&device_list);
    for (i = 0; i < device_list->platform_num; i++)
        nb_devices += device_list->platform_node[i]->device_num;
    if (!nb_devices) {
        av_log(NULL, AV_LOG_ERROR, "No OpenCL device detected!\n");
        return AVERROR(EINVAL);
    }
    if (!(devices = av_malloc_array(nb_devices, sizeof(OpenCLDeviceBenchmark)))) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate buffer\n");
        return AVERROR(ENOMEM);
    }

    for (i = 0; i < device_list->platform_num; i++) {
        for (j = 0; j < device_list->platform_node[i]->device_num; j++) {
            device_node = device_list->platform_node[i]->device_node[j];
            platform = device_list->platform_node[i]->platform_id;
            score = av_opencl_benchmark(device_node, platform, run_opencl_bench);
            if (score > 0) {
                devices[count].platform_idx = i;
                devices[count].device_idx = j;
                devices[count].runtime = score;
                strcpy(devices[count].device_name, device_node->device_name);
                count++;
            }
        }
    }
    qsort(devices, count, sizeof(OpenCLDeviceBenchmark), compare_ocl_device_desc);
    fprintf(stderr, "platform_idx\tdevice_idx\tdevice_name\truntime\n");
    for (i = 0; i < count; i++)
        fprintf(stdout, "%d\t%d\t%s\t%"PRId64"\n",
                devices[i].platform_idx, devices[i].device_idx,
                devices[i].device_name, devices[i].runtime);

    av_opencl_free_device_list(&device_list);
    av_free(devices);
    return 0;
}
