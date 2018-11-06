int ff_find_unused_picture(MpegEncContext *s, int shared){
    int i;

    if(shared){
        for(i=0; i<MAX_PICTURE_COUNT; i++){
            if(s->picture[i].data[0]==NULL && s->picture[i].type==0) return i;
        }
    }else{
        for(i=0; i<MAX_PICTURE_COUNT; i++){
            if(s->picture[i].data[0]==NULL && s->picture[i].type!=0) return i; //FIXME
        }
        for(i=0; i<MAX_PICTURE_COUNT; i++){
            if(s->picture[i].data[0]==NULL) return i;
        }
    }

    assert(0);
    return -1;
}
