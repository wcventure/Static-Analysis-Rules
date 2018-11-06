int
codec_g722_decode(void *ctx _U_, const void *input, int inputSizeBytes, void *output,
        int *outputSizeBytes)
{
    *outputSizeBytes = g722_decode(&state, output, input, inputSizeBytes);
    return 0;
}
