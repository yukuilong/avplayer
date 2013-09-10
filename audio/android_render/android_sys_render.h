#ifndef ANDROID_SYS_RENDER
#define ANDROID_SYS_RENDER

#include "internal.h"
#include "av_buffer.h"
#include <android/log.h>
#include <pthread.h>

#include "AudioTrack.h"
#include "AudioRecord.h"
#include "String8.h"

#include "audio_render.h"

#define WTITE_LOG

#define NATIVE_USE_HARDWARE_RATE 1
#define TRACE_SND_WRITE_TIMINGS

/*notification duration for audio callbacks, in ms*/
static const float audio_buf_ms=0.01;

static const int flowControlIntervalMs = 2500;
static const int flowControlThresholdMs = 80;

static int std_sample_rates[]={
	48000,44100,32000,22050,16000,8000,-1
};

using namespace::fake_android;



struct AndroidNativeSndCardData
{
	AndroidNativeSndCardData(): mVoipMode(0) ,mIoHandle(0)
	{
		/* try to use the same sampling rate as the playback.*/
		int hwrate;
		enableVoipMode();
		if (AudioSystem::getOutputSamplingRate(&hwrate,AUDIO_STREAM_MUSIC/*AUDIO_STREAM_VOICE_CALL*/)==0)
		{
#ifdef WTITE_LOG
			__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Hardware output sampling rate is %i",hwrate);
#endif
		}
		mPlayRate=mRecRate=hwrate;
		for(int i=0;;)
		{
			int stdrate=std_sample_rates[i];
			if (stdrate>mRecRate) 
			{
				i++;
				continue;
			}
			if (AudioRecord::getMinFrameCount(&mRecFrames, mRecRate, AUDIO_FORMAT_PCM_16_BIT,1)==0)
			{
#ifdef WTITE_LOG
				__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Minimal AudioRecord buf frame size at %i Hz is %i",mRecRate,mRecFrames);
#endif
				break;
			}
			else
			{
#ifdef WTITE_LOG
				__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Recording at  %i hz is not supported",mRecRate);
#endif
				i++;
				if (std_sample_rates[i]==-1)
				{
#ifdef WTITE_LOG
					__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Cannot find suitable sampling rate for recording !");
#endif
					return;
				}
				mRecRate=std_sample_rates[i];
			}
		}
		disableVoipMode();
#if 0
		mIoHandle=AudioSystem::getInput(AUDIO_SOURCE_VOICE_COMMUNICATION,mRecRate,AUDIO_FORMAT_PCM_16_BIT,AUDIO_CHANNEL_IN_MONO,(audio_in_acoustics_t)0,0);
		if (mIoHandle==0)
		{
			ms_message("No io handle for AUDIO_SOURCE_VOICE_COMMUNICATION, trying AUDIO_SOURCE_VOICE_CALL");
			mIoHandle=AudioSystem::getInput(AUDIO_SOURCE_VOICE_CALL,mRecRate,AUDIO_FORMAT_PCM_16_BIT,AUDIO_CHANNEL_IN_MONO,(audio_in_acoustics_t)0,0);
			if (mIoHandle==0)
			{
				ms_warning("No io handle for capture.");
			}
		}
#endif
	}
	void enableVoipMode()
	{
		mVoipMode++;
		if (mVoipMode==1)
		{
			//hack for samsung devices
			status_t err;
			String8 params("voip=on");
			if ((err = AudioSystem::setParameters(mIoHandle,params))==0)
			{
#ifdef WTITE_LOG
				__android_log_print(ANDROID_LOG_INFO,"android_sys_render","voip=on is set.");
#endif
			}
			else 	
			{
#ifdef WTITE_LOG
				__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Could not set voip=on: err=%d.", err);
#endif
			}
		}
	}
	void disableVoipMode()
	{
		mVoipMode--;
		if (mVoipMode==0)
		{
			status_t err;
			String8 params("voip=off");
			if ((err = AudioSystem::setParameters(mIoHandle,params))==0)
			{
#ifdef WTITE_LOG
				__android_log_print(ANDROID_LOG_INFO,"android_sys_render","voip=off is set.");
#endif
			}
			else 
			{
#ifdef WTITE_LOG
				__android_log_print(ANDROID_LOG_INFO,"android_sys_render","Could not set voip=off: err=%d.", err);
#endif
			}
		}
	}
	int mVoipMode;
	int mPlayRate;
	int mRecRate;
	int mRecFrames;
	audio_io_handle_t mIoHandle;
};

struct AndroidSndWriteData
{
	AndroidSndWriteData()
	{
		stype=AUDIO_STREAM_MUSIC;//AUDIO_STREAM_VOICE_CALL;
		rate=44100;
		nchannels=2;
		nFramesRequested=0;
		mutex = PTHREAD_MUTEX_INITIALIZER;
		av_buffer_init(&bf);
	}
	~AndroidSndWriteData()
	{
		av_buffer_uninit(bf);
	}
	void setCard(AndroidNativeSndCardData *card)
	{
		mCard=card;
#ifdef NATIVE_USE_HARDWARE_RATE
		rate=card->mPlayRate;
#endif
	}
	AndroidNativeSndCardData *mCard;
	audio_stream_type_t stype;
	int rate;
	int nchannels;
	pthread_mutex_t mutex;
	AVbuffer *bf;
	AudioTrack *tr;
	int nbufs;
	int nFramesRequested;
	bool mStarted;
	uint64_t flowControlStart;
	int minBufferFilling;
};

class android_sys_render
   : public audio_render
{
public:
	android_sys_render();
   	virtual ~android_sys_render();

public:
   	// 初始化音频输出.
   	virtual bool init_audio(void* ctx, int channels, int bits_per_sample, int sample_rate, int format);

   	// 播放音频数据.
   	virtual int play_audio(uint8_t* data, uint32_t size);

   	// 音频播放控制, cmd为CONTROL_开始的宏定义.
   	virtual void audio_control(int cmd, void* arg);

   	// 销毁音频输出组件.
   	virtual void destory_audio();

private:
	AndroidNativeSndCardData *      m_sndcarddata;
	AndroidSndWriteData *		m_writedata;
	Library *			m_libmedia;
	Library *			m_libutils;
	bool     			m_support;
	double                          m_sleep_in;

};

#endif
