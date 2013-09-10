#include "android_sys_render.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/time.h>
#ifdef __cplusplus
}
#endif


static void android_snd_write_cb(int event, void *user, void * p_info)
{
	AndroidSndWriteData *ad=(AndroidSndWriteData*)user;
	
	if (event==AudioTrack::EVENT_MORE_DATA)
	{
		AudioTrack::Buffer *info=reinterpret_cast<AudioTrack::Buffer *>(p_info);
		int avail;
		int ask;

		pthread_mutex_lock(&ad->mutex);
		ask = info->size;
		avail = av_buffer_get_bytes_size(ad->bf);
		/* Drop the samples accumulated before the first callback asking for data. */
		if ((ad->nbufs == 0) && (avail > (ask * 2))) 
		{
			av_buffer_skip_bytes(ad->bf,avail - (ask * 2));
		}
		if (avail != 0) 
		{
			if ((ad->minBufferFilling == -1)) 
			{
				ad->minBufferFilling = avail;
			} 
			else if (avail < ad->minBufferFilling) 
			{
				ad->minBufferFilling = avail;
			}
		}
		info->size = MIN(avail, ask);
#ifdef TRACE_SND_WRITE_TIMINGS
		{
			__android_log_print(ANDROID_LOG_INFO,"android_sys_render"," AudioTrack ask %d, given %d, available %d [%f ms]",
				ask, info->size, avail, (avail / (2 * ad->nchannels)) / (ad->rate / 1000.0));
		}
#endif
		if (info->size > 0)
		{
			av_buffer_read(ad->bf,(uint8_t*)info->raw,info->size);
			info->frameCount = info->size / 2;
		}
		pthread_mutex_unlock(&ad->mutex);
		ad->nbufs++;
		ad->nFramesRequested+=info->frameCount;
		/*
		if (ad->nbufs %100)
		{
			uint32_t pos;
			if (ad->tr->getPosition(&pos)==0)
			{
				ms_message("Requested frames: %i, playback position: %i, diff=%i",ad->nFramesRequested,pos,ad->nFramesRequested-pos);
			}
		}
		*/
	}
	else if (event==AudioTrack::EVENT_UNDERRUN)
	{
		pthread_mutex_lock(&ad->mutex);
#ifdef TRACE_SND_WRITE_TIMINGS
		{
			__android_log_print(ANDROID_LOG_INFO,"android_sys_render","PCM playback underrun: available %d", av_buffer_get_bytes_size(ad->bf));
		}
#else

#ifdef WTITE_LOG
		__android_log_print(ANDROID_LOG_INFO,"android_sys_render","PCM playback underrun: available %d", av_buffer_get_bytes_size(ad->bf));
#endif

#endif
		pthread_mutex_unlock(&ad->mutex);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Untracked event %i",event);

	}
}

android_sys_render::android_sys_render()
{
	m_support = false;
	m_writedata = NULL;
	m_sndcarddata = NULL;
	m_libmedia = Library::load("/system/lib/libmedia.so");
	m_libutils = Library::load("/system/lib/libutils.so");
	if (m_libmedia && m_libutils)
	{
		/*perform initializations in order rather than in a if statement so that all missing symbols are shown in logs*/
		bool audio_record_loaded=AudioRecordImpl::init(m_libmedia);
		bool audio_track_loaded=AudioTrackImpl::init(m_libmedia);
		bool audio_system_loaded=AudioSystemImpl::init(m_libmedia);
		bool string8_loaded=String8Impl::init(m_libutils);
		if (audio_record_loaded && audio_track_loaded && audio_system_loaded && string8_loaded)
		{
#ifdef WTITE_LOG
			__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Native android sound support available.");
#endif
			m_support = true;
			m_sndcarddata = new AndroidNativeSndCardData();
			m_writedata = new AndroidSndWriteData();
			m_writedata->setCard(m_sndcarddata);
			return;
		}
	}
#ifdef WTITE_LOG
	__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Native android sound support is NOT available.");
#endif
}

android_sys_render::~android_sys_render()
{
	if(m_sndcarddata) 
	{
		delete m_sndcarddata;
	}
	if(m_writedata)
	{
		delete m_writedata;
	}
}

bool android_sys_render::init_audio(void* ctx, int channels, int bits_per_sample, int sample_rate, int format)
{
	if(!m_support)return false;
	int play_buf_size;
	status_t s;
	m_writedata->nchannels = channels;
	m_writedata->rate = sample_rate;
	m_sleep_in = 1000000.0/(2*channels*sample_rate);
	__android_log_print(ANDROID_LOG_INFO,"android_sys_render","%d  %d",channels,sample_rate);
	int notify_frames=(int)(audio_buf_ms*(float)m_writedata->rate);
	
	m_writedata->mCard->enableVoipMode();
	m_writedata->nFramesRequested=0;
	
	if (AudioTrack::getMinFrameCount(&play_buf_size,m_writedata->stype,m_writedata->rate)==0)
	{
#ifdef WTITE_LOG
		__android_log_print(ANDROID_LOG_INFO,"android_sys_render","AudioTrack: min frame count is %i",play_buf_size);
#endif
	}
	else
	{
#ifdef WTITE_LOG
		__android_log_print(ANDROID_LOG_INFO,"android_sys_render","AudioTrack::getMinFrameCount() error");
#endif
		return false;
	}
	
	m_writedata->tr=new AudioTrack(m_writedata->stype,
                     	m_writedata->rate,
                     	AUDIO_FORMAT_PCM_16_BIT,
                     	audio_channel_out_mask_from_count(m_writedata->nchannels),
                     	play_buf_size,
                     	AUDIO_OUTPUT_FLAG_NONE, // AUDIO_OUTPUT_FLAG_NONE,
                     	android_snd_write_cb, m_writedata,notify_frames,0);
	s=m_writedata->tr->initCheck();
	if (s!=0) 
	{
#ifdef WTITE_LOG
		__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Problem setting up AudioTrack: %s",strerror(-s));
#endif
		delete m_writedata->tr;
		m_writedata->tr=NULL;
		return false;
	}
	m_writedata->nbufs=0;
#ifdef WTITE_LOG
	__android_log_print(ANDROID_LOG_INFO,"android_sys_render","AudioTrack latency estimated to %i ms",m_writedata->tr->latency());
#endif
	m_writedata->mStarted=false;
	m_writedata->flowControlStart = av_gettime();
	m_writedata->minBufferFilling = -1;
	return true;
}

void android_sys_render::destory_audio()
{
	if (!m_writedata->tr) return;
#ifdef WTITE_LOG
	__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Stopping sound playback");
#endif
	m_writedata->tr->stop();
	m_writedata->tr->flush();
#ifdef WTITE_LOG
	__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Sound playback stopped");
#endif
	delete m_writedata->tr;
	m_writedata->tr=NULL;
	m_writedata->mCard->disableVoipMode();
	m_writedata->mStarted=false;
}

int android_sys_render::play_audio(uint8_t* data, uint32_t size)
{
	
	if (!m_writedata->tr) 
	{
		return 0;
	}
#ifdef WTITE_LOG
		__android_log_print(ANDROID_LOG_INFO,"android_sys_render","audio play one frame %d\n",size);
#endif
	if (!m_writedata->mStarted)
		m_writedata->tr->start();
	pthread_mutex_lock(&m_writedata->mutex);
#ifdef TRACE_SND_WRITE_TIMINGS
	{
		int prev_size = m_writedata->bf->size;
		mbk * m = new mbk();
		m->b_dptr = data;
		m->b_rptr = m->b_dptr;
		m->b_wptr = m->b_dptr + size;
		av_buffer_put(m_writedata->bf,m);

		__android_log_print(ANDROID_LOG_INFO,"android_sys_render","enqueue buffer %d",m_writedata->bf->size - prev_size);


	}
#else
	mbk * m = new mbk();
	m->b_dptr = data;
	m->b_rptr = data;
	m->b_wptr = data + size;
	av_buffer_put(m_writedata->bf,m);
#endif
	int64_t curtime = av_gettime();
	if (((uint32_t)(curtime - m_writedata->flowControlStart)) >= flowControlIntervalMs) 
	{
		int threshold = (flowControlThresholdMs * m_writedata->nchannels * 2 * m_writedata->rate) / 1000;
		if (m_writedata->minBufferFilling > threshold) 
		{
#ifdef WTITE_LOG
			__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Too many samples waiting in sound writer, dropping %i bytes", threshold);
#endif
			//av_buffer_skip_bytes(m_writedata->bf,threshold);
		}
		m_writedata->flowControlStart = curtime;
		m_writedata->minBufferFilling = -1;
	}
	pthread_mutex_unlock(&m_writedata->mutex);
	if (m_writedata->tr->stopped()) 
	{
#ifdef WTITE_LOG
			__android_log_print(ANDROID_LOG_INFO,"android_sys_render","AudioTrack stopped unexpectedly, needs to be restarted");
#endif
		m_writedata->tr->start();
	}
	unsigned int sleep_time = size * m_sleep_in;
#ifdef WTITE_LOG
	__android_log_print(ANDROID_LOG_INFO,"android_sys_render","%d",sleep_time);
#endif
	av_usleep(sleep_time);
	return size;
}

void android_sys_render::audio_control(int cmd, void* arg)
{

}























