typedef struct {
    const AVClass *class;
    int x, y;                   ///< position of overlayed picture

    int allow_packed_rgb;
    uint8_t main_is_packed_rgb;
    uint8_t main_rgba_map[4];
    uint8_t main_has_alpha;
    uint8_t overlay_is_packed_rgb;
    uint8_t overlay_rgba_map[4];
    uint8_t overlay_has_alpha;

    AVFilterBufferRef *overpicref, *overpicref_next;

    int main_pix_step[4];       ///< steps per pixel for each plane of the main output
    int overlay_pix_step[4];    ///< steps per pixel for each plane of the overlay
    int hsub, vsub;             ///< chroma subsampling values

    char *x_expr, *y_expr;
} OverlayContext;
