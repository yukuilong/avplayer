#ifndef AV_QUEUE_H
#define AV_QUEUE_H

#include <queue>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

using namespace std;

struct mbk
{
	unsigned char * b_rptr;
	unsigned char * b_wptr;
	unsigned char * b_dptr;
};

struct AVbuffer
{
	queue<mbk*> q;
	int size;
};

#ifdef  __cplusplus
extern "C" {
#endif

void av_buffer_init(AVbuffer **bf);

void av_buffer_put(AVbuffer *bf,mbk * m);

int av_buffer_read(AVbuffer *bf,unsigned char * out,int outlen);

int av_buffer_get_bytes_size(AVbuffer *bf);

void av_buffer_skip_bytes(AVbuffer *bf,int bytes);

void av_buffer_uninit(AVbuffer *bf);

#ifdef  __cplusplus
}
#endif

#endif
