#ifndef ANDROID_OPENGLES_RENDER_
#define ANDROID_OPENGLES_RENDER_

#include "internal.h"
#include "globals.h"
#include "video_render.h"
#include "shaders.h"
#include <android/bitmap.h>
#include <dlfcn.h>
#include <pthread.h>
#include <jni.h>
#include <android/log.h>



//#define CHECK_GL_ERROR

#ifdef CHECK_GL_ERROR
	#define GL_OPERATION(x)	\
		(x); \
		check_GL_errors(#x);
#else
	#define GL_OPERATION(x) \
		(x);
#endif

enum ImageType {
	REMOTE_IMAGE = 0,
	PREVIEW_IMAGE,
	MAX_IMAGE
};

enum {
	UNIFORM_PROJ_MATRIX = 0,
	UNIFORM_ROTATION,
	UNIFORM_TEXTURE_Y,
	UNIFORM_TEXTURE_U,
	UNIFORM_TEXTURE_V,
	NUM_UNIFORMS
};

enum {
	ATTRIB_VERTEX = 0,
	ATTRIB_UV,
	NUM_ATTRIBS
};

enum {
	Y,
	U,
	V
};

struct VideoSize
{
	int width,height;
};

struct Picture
{
	int w,h;
	uint8_t *planes[4];
	int strides[4];
};

#define TEXTURE_BUFFER_SIZE 3


class android_gles_render : public video_render
{
public:
	android_gles_render();
	virtual ~android_gles_render();
public:
	// 初始render.
	virtual bool init_render(void* ctx, int w, int h, int pix_fmt);

	// 渲染一帧.
	virtual bool render_one_frame(AVFrame* data, int pix_fmt);

	// 调整大小.
	virtual void re_size(int width, int height);

	// 设置宽高比.
	virtual void aspect_ratio(int srcw, int srch, bool enable_aspect);

	// 撤销render.
	virtual void destory_render();
    	virtual bool use_overlay();
public:
	void ogl_display_set_size(int width, int height);
	void ogl_display_init(int width, int height);
	void ogl_display_uninit(bool freeGLresources);
	void ogl_display_set_yuv_to_display(uint8_t *yuv);
	void ogl_display_set_preview_yuv_to_display(uint8_t *yuv);
	void ogl_display_render(int orientation);
	void ogl_display_zoom(float* params);
private:
	void ogl_display_set_yuv(uint8_t *yuv, enum ImageType type);
	void ogl_display_render_type(enum ImageType type, bool clear, float vpx, float vpy, float vpw, float vph, int orientation);
private:
	void check_GL_errors(const char* context);
	bool load_shaders(GLuint* program, GLint* uniforms);
	void allocate_gl_textures(int w, int h, enum ImageType type);
	void load_orthographic_matrix(float left, float right, float bottom, float top, float near, float far, float* mat);
	unsigned int align_on_power_of_2(unsigned int value);
	bool update_textures_with_yuv(enum ImageType type);
	void yuv_buf_init(Picture *buf, int w, int h, uint8_t *ptr);
private:
	/* input: yuv image to display */
	pthread_mutex_t m_yuv_mutex;
	uint8_t *m_yuv[MAX_IMAGE];
	bool m_new_yuv_image[TEXTURE_BUFFER_SIZE][MAX_IMAGE];

	/* GL resources */
	bool m_glResourcesInitialized;
	GLuint m_program, m_textures[TEXTURE_BUFFER_SIZE][MAX_IMAGE][3];
	GLint m_uniforms[NUM_UNIFORMS];
	VideoSize m_allocatedTexturesSize[MAX_IMAGE];

	int m_texture_index;
	
	/* GL view size */
	GLint m_backingWidth;
	GLint m_backingHeight;

	/* runtime data */
	float m_uvx[MAX_IMAGE], m_uvy[MAX_IMAGE];
	VideoSize m_yuv_size[MAX_IMAGE];

	/* coordinates of for zoom-in */
	float m_zoom_factor;
	float m_zoom_cx;
	float m_zoom_cy;

	int m_img_w,m_img_h;
private:
	/* about JNI */
	jobject m_android_video_window;
	jboolean m_ogl_ready;
	jmethodID m_set_opengles_display_id;
	jmethodID m_request_render_id;
};




#endif //ANDROID_OPENGLES_RENDER_
