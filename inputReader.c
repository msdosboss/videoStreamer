#include "inputReader.h"
//Should always pass fmtCtx as NULL
int parseVideoFile(const char* fileName, AVHandle *fmtHandle){

    if(avformat_open_input(&(fmtHandle->fmtCtx), "input.mp4", NULL, NULL) < 0){
        fprintf(stderr, "Could not open input file\n");
        return 1;
    }
    if(avformat_find_stream_info(fmtHandle->fmtCtx, NULL) < 0){
        fprintf(stderr, "Could not parse stream info.\n");
        return 2;
    }
    fmtHandle->videoIndex = -1;
    fmtHandle->audioIndex = -1;
    for(int i = 0; i < fmtHandle->fmtCtx->nb_streams; i++){
        if(fmtHandle->fmtCtx->streams[i]->nb_frames == 0){
            fprintf(stderr, "Amount of frames is unknown.\n");
            return 3;
        }
        if(fmtHandle->fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            fmtHandle->audioIndex = i;
        }
        if(fmtHandle->fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            fmtHandle->videoIndex = i;
        }
    }
    if(fmtHandle->videoIndex == -1){
        fprintf(stderr, "Failed to find video stream\n");
        return 4;
    }
    if(fmtHandle->audioIndex == -1){
        fprintf(stderr, "Failed to find audio stream\n");
        return 4;
    }

    return 0;
}

//This function creates different resoultions of video from a single video
int genMultiResolution(AVHandle *fmtHandle){
    return 0;
}


int main(){
    AVHandle *fmtHandle = malloc(sizeof(AVHandle));
    fmtHandle->fmtCtx = NULL;
    if(parseVideoFile("input.mp4", fmtHandle) != 0){
        fprintf(stderr, "Error occured during parseVideoFile");
        return 1;
    }
    return 0;
}
