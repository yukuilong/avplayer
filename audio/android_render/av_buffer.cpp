#include "av_buffer.h"

#include <string.h>

#ifdef  __cplusplus
extern "C" {
#endif

void av_buffer_init(AVbuffer **bf)
{
	(*bf) = new AVbuffer();
	(*bf)->size = 0;
}

void av_buffer_put(AVbuffer *bf,mbk *m)
{
	if(m != 0)
	{
		bf->q.push(m);
		bf->size += (m->b_wptr - m->b_rptr);
	}
}

int av_buffer_read(AVbuffer *bf,unsigned char * out,int outlen)
{
	if(bf->size > 0)
	{
		int sz = 0;
		int cplen = 0;
		mbk *m = bf->q.front();
		while(sz < outlen)
		{
			cplen = MIN(m->b_wptr - m->b_rptr ,outlen - sz);
			memcpy(out+sz,m->b_rptr,cplen);
			sz += cplen;
			m->b_rptr += cplen;
			
			if(m->b_rptr == m->b_wptr)
			{
				bf->q.pop();
				delete [] m->b_dptr;
				delete m;
				m = bf->q.front();
			}
		}
		bf->size -= outlen;
		return outlen;
	}
	return 0;
}

int av_buffer_get_bytes_size(AVbuffer *bf)
{
	return bf->size;
}

void av_buffer_skip_bytes(AVbuffer *bf,int bytes)
{
	unsigned char *tmp = new unsigned char[bytes];
	av_buffer_read(bf,tmp,bytes);
	delete [] tmp;
}

void av_buffer_uninit(AVbuffer *bf)
{
	while(!bf->q.empty())
	{
		mbk * tmp = bf->q.front();
		bf->q.pop();
		delete [] tmp->b_dptr;
		delete tmp;
	}
	bf->size = 0;
}
#ifdef  __cplusplus
}
#endif
