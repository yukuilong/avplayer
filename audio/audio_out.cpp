#include "globals.h"
#include "audio_out.h"

#ifdef ANDROID
#include "android_sys_render.h"
#endif


#ifdef  __cplusplus
extern "C" {
#endif

#ifdef WIN32
#include "internal.h"
#include "wave_render.h"
#include "dsound_render.h"

EXPORT_API int wave_init_audio(struct ao_context *ctx, uint32_t channels,
	uint32_t bits_per_sample, uint32_t sample_rate, int format)
{
	wave_render* wave = NULL;
	ctx->priv = (void*)(wave = new wave_render);
	return wave->init_audio((void*)ctx->priv, channels, bits_per_sample, sample_rate, format) ? 0 : -1;
}

EXPORT_API int wave_play_audio(struct ao_context *ctx, uint8_t* data, uint32_t size)
{
	wave_render* wave = (wave_render*)ctx->priv;
	return wave->play_audio(data, size);
}

EXPORT_API void wave_audio_control(struct ao_context *ctx, double l, double r)
{
	wave_render* wave = (wave_render*)ctx->priv;
	control_vol_t ctrl_vol = { l, r };
	wave->audio_control(CONTROL_SET_VOLUME, &ctrl_vol);
}

EXPORT_API void wave_mute_set(struct ao_context *ctx, int s)
{
	wave_render* wave = (wave_render*)ctx->priv;
	control_vol_t ctrl_vol;
	ctrl_vol.mute = s;
	wave->audio_control(CONTROL_MUTE_SET, &ctrl_vol);
}

EXPORT_API void wave_destory_audio(struct ao_context *ctx)
{
	wave_render* wave = (wave_render*)ctx->priv;
	if (wave)
	{
		wave->destory_audio();
		delete wave;
		ctx->priv = NULL;
	}
}


EXPORT_API int dsound_init_audio(struct ao_context *ctx, uint32_t channels,
	uint32_t bits_per_sample, uint32_t sample_rate, int format)
{
	dsound_render* dsound = NULL;
	ctx->priv = (void*)(dsound = new dsound_render);
	return dsound->init_audio((void*)dsound, channels, bits_per_sample, sample_rate, format) ? 0 : -1;
}

EXPORT_API int dsound_play_audio(struct ao_context *ctx, uint8_t* data, uint32_t size)
{
	dsound_render* dsound = (dsound_render*)ctx->priv;
	return dsound->play_audio(data, size);
}

EXPORT_API void dsound_audio_control(struct ao_context *ctx, double l, double r)
{
	dsound_render* dsound = (dsound_render*)ctx->priv;
	control_vol_t ctrl_vol = { l, r };
	dsound->audio_control(CONTROL_SET_VOLUME, &ctrl_vol);
}

EXPORT_API void dsound_mute_set(struct ao_context *ctx, int s)
{
	dsound_render* dsound = (dsound_render*)ctx->priv;
	control_vol_t ctrl_vol;
	ctrl_vol.mute = s;
	dsound->audio_control(CONTROL_MUTE_SET, &ctrl_vol);
}

EXPORT_API void dsound_destory_audio(struct ao_context *ctx)
{
	dsound_render* dsound = (dsound_render*)ctx->priv;
	if (dsound)
	{
		dsound->destory_audio();
		delete dsound;
		ctx->priv = NULL;
	}
}

#endif

#ifdef ANDROID

EXPORT_API int android_init_audio(struct ao_context *ctx, uint32_t channels,
	uint32_t bits_per_sample, uint32_t sample_rate, int format)
{
	android_sys_render* android_sound = NULL;
	ctx->priv = (void*)(android_sound = new android_sys_render());
	return android_sound->init_audio((void*)android_sound, channels, bits_per_sample, sample_rate, format) ? 0 : -1;
	return 0;
}
EXPORT_API int android_play_audio(struct ao_context *ctx, uint8_t *data, uint32_t size)
{
	android_sys_render* android_sound = (android_sys_render*)ctx->priv;
	return android_sound->play_audio(data, size);
}
EXPORT_API void android_audio_control(struct ao_context *ctx, double l, double r)
{
	android_sys_render* android_sound = (android_sys_render*)ctx->priv;
	control_vol_t ctrl_vol = { l, r };
	android_sound->audio_control(CONTROL_SET_VOLUME, &ctrl_vol);
}
EXPORT_API void android_mute_set(struct ao_context *ctx, int s)
{
	android_sys_render* android_sound = (android_sys_render*)ctx->priv;
	control_vol_t ctrl_vol;
	ctrl_vol.mute = s;
	android_sound->audio_control(CONTROL_MUTE_SET, &ctrl_vol);
}
EXPORT_API void android_destory_audio(struct ao_context *ctx)
{
	android_sys_render* android_sound = (android_sys_render*)ctx->priv;
	if (android_sound)
	{
		android_sound->destory_audio();
		delete android_sound;
		ctx->priv = NULL;
	}
}
#endif

#ifdef  __cplusplus
}
#endif

