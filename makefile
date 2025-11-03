LAGS := $(shell pkg-config --cflags libavformat libavcodec libavutil libswresample libswscale)
LDFLAGS := $(shell pkg-config --libs libavformat libavcodec libavutil libswresample libswscale)

debug:
	gcc $(CFLAGS) -g inputReader.c -o inputReader $(LDFLAGS)

all: inputReader

inputReader: inputReader.c
	gcc $(CFLAGS) inputReader.c -o inputReader $(LDFLAGS)

clean:
	rm inputReader
