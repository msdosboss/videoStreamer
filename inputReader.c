#include <libavformat/avformat.h>
#include <stdio.h>

int main(){
    AVFormatContext *fmtCtx = NULL;
    if(avformat_open_input(&fmtCtx, "input.mp4", NULL, NULL) < 0){
        fprintf(stderr, "Could not open input file\n");
    }
}
