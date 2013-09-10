#include <jni.h>
#include <android/log.h>

#include "player_impl.h"

player_impl *g_avplay = NULL;

JNIEXPORT jint JNICALL  JNI_OnLoad(JavaVM *ajvm, void *reserved)
{
	av_set_jvm(ajvm);
	__android_log_print(ANDROID_LOG_INFO,"avplayer_jni","JNI_OnLoad");
	return JNI_VERSION_1_2;
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayInit(JNIEnv * env, jclass obj)
{
	if(g_avplay == NULL)
	{
		g_avplay = new player_impl();
		//__android_log_print(ANDROID_LOG_INFO,"avplayer_jni","%d \n",g_avplay);
	}
}

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayUninit(JNIEnv * env, jclass obj)
{
	if(g_avplay)
	{
		delete g_avplay;
		g_avplay = NULL;
	}
}

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplaySetWindowHandle(JNIEnv * env, jclass obj, jobject window)
{
	g_avplay->set_window_handle((unsigned long)window);
}

JNIEXPORT jboolean JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayOpen(JNIEnv * env , jclass obj,jstring movie, jint media_type, jint render_type)
{
	const char *str = env->GetStringUTFChars(movie, NULL);
	return (unsigned char)g_avplay->open(str, media_type, render_type);
}

JNIEXPORT jboolean JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvpalyPlay(JNIEnv * env, jclass obj,jdouble fact, jint index)
{
	return (unsigned char)g_avplay->play(fact, index);
}

JNIEXPORT jboolean JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayPause(JNIEnv * env, jclass obj)
{
	return (unsigned char)g_avplay->pause();
}

JNIEXPORT jboolean JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayResume(JNIEnv * env, jclass obj)
{
	return (unsigned char)g_avplay->resume();
}

JNIEXPORT jboolean JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayWaitForCompletion(JNIEnv * env, jclass obj)
{
	return (unsigned char)g_avplay->wait_for_completion();
}

JNIEXPORT jboolean JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayStop(JNIEnv * env, jclass obj)
{
	return (unsigned char)g_avplay->stop();
}

JNIEXPORT jboolean JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayClose(JNIEnv * env, jclass obj)
{
	return (unsigned char)g_avplay->close();
}

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplaySeekTo(JNIEnv * env, jclass obj,jdouble fact)
{
	g_avplay->seek_to(fact);
}

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayVolume(JNIEnv * env, jclass obj,jdouble l, jdouble r)
{
	g_avplay->volume(l, r);
}

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayMuteSet(JNIEnv * env, jclass obj,jboolean s)
{
	g_avplay->mute_set(s);
}

JNIEXPORT jint JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayDownloadRate(JNIEnv * env, jclass obj)
{
	return g_avplay->download_rate();
}

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplaySetDownloadRate(JNIEnv * env, jclass obj,jint k)
{
	g_avplay->set_download_rate(k);
}

JNIEXPORT jdouble JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayCurrPlayTime(JNIEnv * env, jclass obj)
{
	return g_avplay->curr_play_time();
}

JNIEXPORT jdouble JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayDuration(JNIEnv * env, jclass obj)
{
	return g_avplay->duration();
}

JNIEXPORT jint JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayVideoWidth(JNIEnv * env, jclass obj)
{
	return g_avplay->video_width();
}

JNIEXPORT jint JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayVideoHeight(JNIEnv * env, jclass obj)
{
	return g_avplay->video_height();
}

JNIEXPORT jdouble JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayBuffering(JNIEnv * env, jclass obj)
{
	return g_avplay->buffering();
}

JNIEXPORT jint JNICALL Java_org_avplayer_avplayer_AvplayerNative_AvplayMediaCount(JNIEnv * env, jclass obj)
{
	return g_avplay->play_list().size();
}

JNIEXPORT jint JNICALL Java_AvplayMediaList(JNIEnv * env, jclass obj,char ***list, jint *size)
{
	std::map<std::string, std::string> mlist = g_avplay->play_list();

	*size = mlist.size();
	char **temp = new char*[*size];

	int n = 0;
	for (std::map<std::string, std::string>::iterator i = mlist.begin();
		i != mlist.end(); i++)
	{
		char *file_name = strdup(i->second.c_str());
		temp[n] = file_name;
	}
	*list = temp;

	return*size;
}

JNIEXPORT void JNICALL Java_AvplayFreeMediaList(JNIEnv * env, jclass obj,char **list, jint size)
{
	for (int i = 0; i < size; i++)
		free(list[i]);
	delete[] list;
}

#ifdef __cplusplus
}
#endif







































