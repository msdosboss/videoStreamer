#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <stdlib.h>

enum CoderState { ENCODER, DECODER };

typedef struct {
  AVCodecContext *decoder;
  AVCodecContext *encoder;
} CodecPair;

typedef struct {
  AVFormatContext *fmtCtx;
  int audioIndex;
  int videoIndex;
} AVHandle;
