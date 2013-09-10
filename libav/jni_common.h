
#ifndef msjava_h
#define msjava_h

/* Helper routines for filters that use a jvm with upcalls to perform some processing */

#include <jni.h>

#ifdef __cplusplus
extern "C"{
#endif

void av_set_jvm(JavaVM *vm);

JavaVM *av_get_jvm(void);

JNIEnv *av_get_jni_env(void);

#ifdef __cplusplus
}
#endif


#endif

