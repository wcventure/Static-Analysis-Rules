typedef struct {
    const AVClass *class;
    uint8_t *fontfile;              ///< font to be used
    uint8_t *text;                  ///< text to be drawn
    uint8_t *expanded_text;         ///< used to contain the strftime()-expanded text
    size_t   expanded_text_size;    ///< size in bytes of the expanded_text buffer
    int ft_load_flags;              ///< flags used for loading fonts, see FT_LOAD_*
    FT_Vector *positions;           ///< positions for each element in the text
    size_t nb_positions;            ///< number of elements of positions array
    char *textfile;                 ///< file with text to be drawn
    unsigned int x;                 ///< x position to start drawing text
    unsigned int y;                 ///< y position to start drawing text
    int shadowx, shadowy;
    unsigned int fontsize;          ///< font size to use
    char *fontcolor_string;         ///< font color as string
    char *boxcolor_string;          ///< box color as string
    char *shadowcolor_string;       ///< shadow color as string
    uint8_t fontcolor[4];           ///< foreground color
    uint8_t boxcolor[4];            ///< background color
    uint8_t shadowcolor[4];         ///< shadow color
    uint8_t fontcolor_rgba[4];      ///< foreground color in RGBA
    uint8_t boxcolor_rgba[4];       ///< background color in RGBA
    uint8_t shadowcolor_rgba[4];    ///< shadow color in RGBA

    short int draw_box;             ///< draw box around text - true or false
    int use_kerning;                ///< font kerning is used - true/false
    int tabsize;                    ///< tab size

    FT_Library library;             ///< freetype font library handle
    FT_Face face;                   ///< freetype font face handle
    struct AVTreeNode *glyphs;      ///< rendered glyphs, stored using the UTF-32 char code
    int hsub, vsub;                 ///< chroma subsampling values
    int is_packed_rgb;
    int pixel_step[4];              ///< distance in bytes between the component of each pixel
    uint8_t rgba_map[4];            ///< map RGBA offsets to the positions in the packed RGBA format
    uint8_t *box_line[4];           ///< line used for filling the box background
    int64_t basetime;               ///< base pts time in the real world for display
} DrawTextContext;
