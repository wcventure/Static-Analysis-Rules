int ff_mlz_decompression(MLZ* mlz, GetBitContext* gb, int size, unsigned char *buff) {
    MLZDict *dict = mlz->dict;
    unsigned long output_chars;
    int string_code, last_string_code, char_code;

    string_code = 0;
    char_code   = -1;
    last_string_code = -1;
    output_chars = 0;

    while (output_chars < size) {
        string_code = input_code(gb, mlz->dic_code_bit);
        switch (string_code) {
            case FLUSH_CODE:
            case MAX_CODE:
                ff_mlz_flush_dict(mlz);
                char_code = -1;
                last_string_code = -1;
                break;
            case FREEZE_CODE:
                mlz->freeze_flag = 1;
                break;
            default:
                if (string_code > mlz->current_dic_index_max) {
                    av_log(mlz->context, AV_LOG_ERROR, "String code %d exceeds maximum value of %d.\n", string_code, mlz->current_dic_index_max);
                    return output_chars;
                }
                if (string_code == (int) mlz->bump_code) {
                    ++mlz->dic_code_bit;
                    mlz->current_dic_index_max *= 2;
                    mlz->bump_code = mlz->current_dic_index_max - 1;
                } else {
                    if (string_code >= mlz->next_code) {
                        output_chars += decode_string(mlz, &buff[output_chars], last_string_code, &char_code, size - output_chars);
                        output_chars += decode_string(mlz, &buff[output_chars], char_code, &char_code, size - output_chars);
                        set_new_entry_dict(dict, mlz->next_code, last_string_code, char_code);
                        mlz->next_code++;
                    } else {
                        output_chars += decode_string(mlz, &buff[output_chars], string_code, &char_code, size - output_chars);
                        if (output_chars <= size && !mlz->freeze_flag) {
                            if (last_string_code != -1) {
                                set_new_entry_dict(dict, mlz->next_code, last_string_code, char_code);
                                mlz->next_code++;
                            }
                        } else {
                            break;
                        }
                    }
                    last_string_code = string_code;
                }
                break;
        }
    }
    return output_chars;
}
