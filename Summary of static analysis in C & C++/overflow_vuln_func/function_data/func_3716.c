static int find_start_code(const uint8_t **pbuf_ptr, const uint8_t *buf_end)
{
    const uint8_t *buf_ptr= *pbuf_ptr;

    buf_ptr++; //gurantees that -1 is within the array
    buf_end -= 2; // gurantees that +2 is within the array

    while (buf_ptr < buf_end) {
        if(*buf_ptr==0){
            while(buf_ptr < buf_end && buf_ptr[1]==0)
                buf_ptr++;

            if(buf_ptr[-1] == 0 && buf_ptr[1] == 1){
                *pbuf_ptr = buf_ptr+3;
                return buf_ptr[2] + 0x100;
            }
        }
        buf_ptr += 2;
    }
    buf_end += 2; //undo the hack above

    *pbuf_ptr = buf_end;
    return -1;
}
