#include "jni_common.h"
#include <stddef.h>
static JavaVM *av2_vm=NULL;

#ifndef WIN32
#include <pthread.h>

static pthread_key_t jnienv_key;

void _android_key_cleanup(void *data)
{
	JNIEnv* env=(JNIEnv*)pthread_getspecific(jnienv_key);
	if (env != NULL) 
	{
		(*av2_vm)->DetachCurrentThread(av2_vm);
		pthread_setspecific(jnienv_key,NULL);
	}
}
#endif



void av_set_jvm(JavaVM *vm)
{
	av2_vm=vm;
#ifndef WIN32
	pthread_key_create(&jnienv_key,_android_key_cleanup);
#endif
}

JavaVM *av_get_jvm(void)
{
	return av2_vm;
}

JNIEnv *av_get_jni_env(void)
{
	JNIEnv *env=NULL;
	if (av2_vm==NULL)
	{
		//ms_fatal("Calling ms_get_jni_env() while no jvm has been set using ms_set_jvm().");
	}
	else
	{
#ifndef WIN32
		env=(JNIEnv*)pthread_getspecific(jnienv_key);
		if (env==NULL)
		{
			if ((*av2_vm)->AttachCurrentThread(av2_vm,&env,NULL)!=0)
			{
				//ms_fatal("AttachCurrentThread() failed !");
				return NULL;
			}
			pthread_setspecific(jnienv_key,env);
		}
#else
		//ms_fatal("ms_get_jni_env() not implemented on windows.");
#endif
	}
	return env;
}



