int
codec_g726_decode(void *ctx _U_, const void *input, int inputSizeBytes, void *output,
        int *outputSizeBytes)
{
    *outputSizeBytes = 2 * g726_decode(&state, output, (void*) input, inputSizeBytes);
    return 0;
}
