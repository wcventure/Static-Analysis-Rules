typedef struct StereoWidenContext {
    const AVClass *class;

    float delay;
    float feedback;
    float crossfeed;
    float drymix;

    float *buffer;
    float *write;
    int length;
} StereoWidenContext;
