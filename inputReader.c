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
        return 5;
    }

    return 0;
}


AVFrame *createFrame(AVHandle *fmtHandle, int downScaleFactor){
    AVFrame *frame = av_frame_alloc();
    if(frame == NULL){
        fprintf(stderr, "frame failed to init\n");
        return NULL;
    }

    frame->width = fmtHandle->fmtCtx->streams[fmtHandle->videoIndex]->codecpar->width / downScaleFactor;
    frame->height = fmtHandle->fmtCtx->streams[fmtHandle->videoIndex]->codecpar->height / downScaleFactor;
    frame->format = fmtHandle->fmtCtx->streams[fmtHandle->videoIndex]->codecpar->format;
    if(av_frame_get_buffer(frame, 0) != 0){
        fprintf(stderr, "frame buffer failed to create\n");
        return NULL;
    }

    return frame;
}


int createCodecContext(AVHandle *fmtHandle, CodecPair *avCtxPair){
    const AVCodec *decoder;
    decoder = avcodec_find_decoder(fmtHandle->fmtCtx->streams[fmtHandle->videoIndex]->codecpar->codec_id);
    if(decoder == NULL){
        fprintf(stderr, "Decoder failed to init\n");
        return 1;
    }
    avCtxPair->decoder = avcodec_alloc_context3(decoder);
    if(avCtxPair->decoder == NULL){
        fprintf(stderr, "Failed to alloc AVCodecContext\n");
        return 2;
    }

    if(avcodec_parameters_to_context(avCtxPair->decoder, fmtHandle->fmtCtx->streams[fmtHandle->videoIndex]->codecpar) != 0){
        fprintf(stderr, "Failed to copy codecpar to context\n");
        return 3;
    }

    if(avcodec_open2(avCtxPair->decoder, decoder, NULL) != 0){
        fprintf(stderr, "avContext failed to init\n");
        return 4;
    }



    return 0;
}




//This function creates different resoultions of video from a single video
int genMultiResolution(AVHandle *fmtHandle){
    CodecPair avCtxPair;
    createCodecContext(fmtHandle, &avCtxPair);

    AVFrame *frame = createFrame(fmtHandle, 1);
    if(frame == NULL){
        fprintf(stderr, "frame failed to init\n");
        return 1;
    }

    AVFrame *downScaleFrame = createFrame(fmtHandle, 2);
    if(downScaleFrame == NULL){
        fprintf(stderr, "downScaleFrame failed to init\n");
        return 2;
    }

    int width = fmtHandle->fmtCtx->streams[fmtHandle->videoIndex]->codecpar->width;
    int height = fmtHandle->fmtCtx->streams[fmtHandle->videoIndex]->codecpar->height;
    int format = fmtHandle->fmtCtx->streams[fmtHandle->videoIndex]->codecpar->format;
    struct SwsContext *swsCtx = sws_getContext(width, height, format, width / 2, height / 2, format, SWS_BICUBIC, NULL, NULL, NULL);

    AVPacket *pkt = av_packet_alloc();
    AVPacket *downScalePkt = av_packet_alloc();
    while(av_read_frame(fmtHandle->fmtCtx, pkt) == 0){
        if(pkt->stream_index == fmtHandle->videoIndex){
            if(avcodec_send_packet(avCtxPair.decoder, pkt) == 0){
                while(avcodec_receive_frame(avCtxPair.decoder, frame) == 0){
                    sws_scale(swsCtx, (uint8_t const* const*)frame->data, frame->linesize, 0, frame->height, downScaleFrame->data, downScaleFrame->linesize);
                    avcodec_send_frame(avCtxPair.encoder, downScaleFrame);
                    avcodec_receive_packet(avCtxPair.encoder, downScalePkt);
                }
            }
        }
        av_packet_unref(pkt);
    }

    av_packet_unref(pkt);
    av_frame_free(&frame);
    av_frame_free(&downScaleFrame);
    avcodec_free_context(&avCtxPair.decoder);
    avcodec_free_context(&avCtxPair.encoder);

    return 0;
}


int main(){
    AVHandle *fmtHandle = malloc(sizeof(AVHandle));
    fmtHandle->fmtCtx = NULL;
    if(parseVideoFile("input.mp4", fmtHandle) != 0){
        fprintf(stderr, "Error occured during parseVideoFile");
        return 1;
    }
    genMultiResolution(fmtHandle);
    avformat_close_input(&(fmtHandle->fmtCtx));
    free(fmtHandle);
    return 0;
}
