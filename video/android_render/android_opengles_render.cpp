#include "android_opengles_render.h"

#define ANDROID_DISPALY_CLASS_NAME "org/avplayer/avplayer/video/AndroidVideoWindowImpl"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_video_display_OpenGLESDisplay_init(JNIEnv * env, jobject obj, jint ptr, jint width, jint height) 
{
	android_gles_render* d = (android_gles_render*) ptr;
	d->ogl_display_init(width, height);
}

JNIEXPORT void JNICALL Java_org_avplayer_avplayer_video_display_OpenGLESDisplay_render(JNIEnv * env, jobject obj, jint ptr) 
{
	android_gles_render* d = (android_gles_render*) ptr;
	d->ogl_display_render(0);
}

#ifdef __cplusplus
}
#endif

android_gles_render::android_gles_render()
{
	m_yuv_mutex = PTHREAD_MUTEX_INITIALIZER;
	memset(m_yuv, 0, sizeof(m_yuv));
	memset(m_new_yuv_image, 0, sizeof(m_new_yuv_image));
	memset(m_textures, 0, sizeof(m_textures));
	memset(m_uniforms, 0, sizeof(m_uniforms));
	memset(m_allocatedTexturesSize, 0, sizeof(m_allocatedTexturesSize));
	memset(m_uvx, 0, sizeof(m_uvx));
	memset(m_uvy, 0, sizeof(m_uvy));
	memset(m_yuv_size, 0, sizeof(m_yuv_size));
	m_glResourcesInitialized = false;
	m_backingWidth = 0;
	m_backingHeight = 0;
	m_zoom_factor = 1;
	m_zoom_cx = m_zoom_cy = 0;
	m_texture_index = 0;
	m_ogl_ready = false;
	m_android_video_window = 0;
	m_set_opengles_display_id = 0;
	m_request_render_id = 0;
}

android_gles_render::~android_gles_render()
{
	
	for(int i=0; i<MAX_IMAGE; i++)
	{
		if (m_yuv[i]) 
		{
			delete m_yuv[i];
			m_yuv[i] = NULL;
		}
	}
}

bool android_gles_render::init_render(void* ctx, int w, int h, int pix_fmt)// w ,h is video size
{	
	JNIEnv *jenv=NULL;
	jclass wc;
	bool res = true;
	unsigned long id=*(unsigned long*)ctx;
	m_android_video_window =(jobject)id;
	if(pix_fmt != PIX_FMT_YUV420P || m_android_video_window == 0)
		return false;
	m_img_w = w;
	m_img_h = h;
	jenv=av_get_jni_env();
	__android_log_print(ANDROID_LOG_INFO,"opengles_display","windonw: %d   %d  %d   %d\n",id,jenv,w,h);
	wc=jenv->FindClass(ANDROID_DISPALY_CLASS_NAME);
	if (wc==0)
	{
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","Could not find %s !\n",ANDROID_DISPALY_CLASS_NAME);
		res = false;
	}
__android_log_print(ANDROID_LOG_INFO,"opengles_display","1");
	m_set_opengles_display_id=jenv->GetMethodID(wc,"setOpenGLESDisplay","(I)V");
	m_request_render_id=jenv->GetMethodID(wc,"requestRender","()V");
	if (m_set_opengles_display_id == 0)
	{
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","Could not find setOpenGLESDisplay method\n");
		res = false;
	}
	if (m_request_render_id == 0)
	{
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","Could not find requestRender method\n");
		res = false;
	}
__android_log_print(ANDROID_LOG_INFO,"opengles_display","2");

	//if (m_android_video_window)
	//	jenv->DeleteGlobalRef( m_android_video_window);
//__android_log_print(ANDROID_LOG_INFO,"opengles_display","3");
	//m_android_video_window=jenv->NewGlobalRef( window);
	
__android_log_print(ANDROID_LOG_INFO,"opengles_display","4");
	unsigned int ptr = (unsigned int)this;
	jenv->CallVoidMethod(m_android_video_window,m_set_opengles_display_id, ptr);


	m_ogl_ready = true;
	__android_log_print(ANDROID_LOG_INFO,"init_render","init_render\n");
	return res;
}

bool android_gles_render::render_one_frame(AVFrame* data, int pix_fmt)
{
	uint8_t *yuv;
	if(pix_fmt != PIX_FMT_YUV420P || !m_ogl_ready)
		return false;
	yuv = data->data[0];

	ogl_display_set_yuv_to_display(yuv);

	JNIEnv *jenv=av_get_jni_env();
	jenv->CallVoidMethod(m_android_video_window,m_request_render_id);
	//__android_log_print(ANDROID_LOG_INFO,"render_one_frame","render\n");
	return true;
}

void android_gles_render::re_size(int width, int height)
{
	ogl_display_set_size(width,height);
}

void android_gles_render::aspect_ratio(int srcw, int srch, bool enable_aspect)
{

}

void android_gles_render::destory_render()
{
	//ogl_display_uninit(true);
	m_ogl_ready = false;
	m_img_w = 0;
	m_img_h = 0;
	for(int i=0; i<MAX_IMAGE; i++) 
	{
		if (m_yuv[i]) 
		{
			delete m_yuv[i];
			m_yuv[i] = NULL;
		}
	}
	//if (m_android_video_window)
	//{
	//	JNIEnv *jenv=av_get_jni_env();
	//	jenv->DeleteGlobalRef( m_android_video_window);
	//}
}
bool android_gles_render::use_overlay()
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

void android_gles_render::ogl_display_set_size(int width, int height) 
{
	m_backingWidth = width;
	m_backingHeight = height;

	//__android_log_print(ANDROID_LOG_INFO,"opengles_display","resize opengles_display (%d x %d, gl initialized:%d)", width, height, m_glResourcesInitialized);
 
	GL_OPERATION(glViewport(0, 0, m_backingWidth, m_backingHeight));
	
	check_GL_errors("ogl_display_set_size");
}

void android_gles_render::ogl_display_init(int width, int height) 
{
	int i,j;
	static bool version_displayed = false;

	__android_log_print(ANDROID_LOG_INFO,"opengles_display","init opengles_display (%d x %d, gl initialized:%d)", width, height, m_glResourcesInitialized);

	GL_OPERATION(glDisable(GL_DEPTH_TEST))
	GL_OPERATION(glClearColor(0, 0, 0, 1))

	ogl_display_set_size(width, height);
	
	if (m_glResourcesInitialized)
		return;

	for(j=0; j<TEXTURE_BUFFER_SIZE; j++) 
	{
		// init textures
		for(i=0; i<MAX_IMAGE; i++) 
		{
			GL_OPERATION(glGenTextures(3, m_textures[j][i]))
			m_allocatedTexturesSize[i].width = m_allocatedTexturesSize[i].height = 0;
		}
	}

	if (!version_displayed) 
	{
		version_displayed = true;
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","OpenGL version string: %s", glGetString(GL_VERSION));
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","OpenGL extensions: %s",glGetString(GL_EXTENSIONS));
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","OpenGL vendor: %s", glGetString(GL_VENDOR));
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","OpenGL renderer: %s", glGetString(GL_RENDERER));
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","OpenGL version: %s", glGetString(GL_VERSION));
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","OpenGL GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
	load_shaders(&m_program, m_uniforms);
	
	GL_OPERATION(glUseProgram(m_program))

	m_glResourcesInitialized = true;
	
	check_GL_errors("ogl_display_init");
}

void android_gles_render::ogl_display_uninit(bool freeGLresources) 
{
	int i,j;
	
	__android_log_print(ANDROID_LOG_INFO,"opengles_display","uninit opengles_display (gl initialized:%d)\n", m_glResourcesInitialized);
	for(i=0; i<MAX_IMAGE; i++) 
	{
		if (m_yuv[i]) 
		{
			delete m_yuv[i];
			m_yuv[i] = NULL;
		}
	}

	if (m_glResourcesInitialized && freeGLresources) 
	{
		// destroy gl resources
		for(j=0; j<TEXTURE_BUFFER_SIZE; j++) 
		{
			for(i=0; i<MAX_IMAGE; i++) 
			{
				GL_OPERATION(glDeleteTextures(3, m_textures[j][i]));
				m_allocatedTexturesSize[i].width = m_allocatedTexturesSize[i].height = 0;
			}
		}
		GL_OPERATION(glDeleteProgram(m_program));
	}

	m_glResourcesInitialized = false;
	
	check_GL_errors("ogl_display_uninit");
}

void android_gles_render::ogl_display_set_yuv(uint8_t *yuv, enum ImageType type) 
{
	pthread_mutex_lock(&m_yuv_mutex);
	if (m_yuv[type])
		delete m_yuv[type];
	m_yuv[type] = yuv;
	int j;
	for(j = 0; j < TEXTURE_BUFFER_SIZE; ++j) 
	{
		m_new_yuv_image[j][type] = true;
	}
	//__android_log_print(ANDROID_LOG_INFO,"ogl_display_set_yuv","(gl initialized:%d)\n", m_glResourcesInitialized);
	pthread_mutex_unlock(&m_yuv_mutex);	
}

void android_gles_render::ogl_display_set_yuv_to_display(uint8_t *yuv) 
{
	ogl_display_set_yuv(yuv, REMOTE_IMAGE);
}

void android_gles_render::ogl_display_set_preview_yuv_to_display(uint8_t *yuv) 
{
	ogl_display_set_yuv(yuv, PREVIEW_IMAGE);
}

void android_gles_render::ogl_display_render_type(enum ImageType type, bool clear, float vpx, float vpy, float vpw, float vph, int orientation) 
{
	if (!m_yuv[type] || !m_glResourcesInitialized) 
	{
		return;
	}
	__android_log_print(ANDROID_LOG_INFO,"ogl_display_render_type","(gl initialized:%d)\n", m_glResourcesInitialized);
	pthread_mutex_lock(&m_yuv_mutex);
	if (m_new_yuv_image[m_texture_index][type]) 
	{
		update_textures_with_yuv(type);
		m_new_yuv_image[m_texture_index][type] = false;
	}
	pthread_mutex_unlock(&m_yuv_mutex);
	
	float uLeft, uRight, vTop, vBottom;

	uLeft = vBottom = 0.0f;
	uRight = m_uvx[type];
	vTop = m_uvy[type]; 

	GLfloat squareUvs[] = {
		uLeft, vTop,
		uRight, vTop,
		uLeft, vBottom,
		uRight, vBottom
	};
	
	if (clear) 
	{
		GL_OPERATION(glClear(GL_COLOR_BUFFER_BIT))
	}
	
	GLfloat squareVertices[8];
	
	// drawing surface dimensions
	int screenW = m_backingWidth;
	int screenH = m_backingHeight;	
	if (orientation == 90 || orientation == 270) 
	{
		screenW = screenH;
		screenH = m_backingWidth;			
	}

	int x,y,w,h;
	// Fill the smallest dimension, then compute the other one using the image ratio
   	if (screenW <= screenH) 
	{
		float ratio = (m_yuv_size[type].height) / (float)(m_yuv_size[type].width);
	  	w = screenW * vpw;
		h = w * ratio;
		if (h > screenH) 
		{
			w *= screenH /(float) h;
			h = screenH;
		}
		x = vpx * m_backingWidth;
		y = vpy * m_backingHeight;
	} 
	else 
	{
		float ratio = m_yuv_size[type].width / (float)m_yuv_size[type].height;
	  	h = screenH * vph;
		w = h * ratio;
		if (w > screenW) 
		{
			h *= screenW / (float)w;
			w = screenW;
		}
		x = vpx * screenW;
		y = vpy * screenH;
	}
	
	squareVertices[0] = (x - w * 0.5) / screenW - 0.;
	squareVertices[1] = (y - h * 0.5) / screenH - 0.;
	squareVertices[2] = (x + w * 0.5) / screenW - 0.;
	squareVertices[3] = (y - h * 0.5) / screenH - 0.;
	squareVertices[4] = (x - w * 0.5) / screenW - 0.;
	squareVertices[5] = (y + h * 0.5) / screenH - 0.;
	squareVertices[6] = (x + w * 0.5) / screenW - 0.;
	squareVertices[7] = (y + h * 0.5) / screenH - 0.;
	
	GLfloat mat[16];
	#define VP_SIZE 1.0f
	if (type == REMOTE_IMAGE) 
	{
		float scale_factor = 1.0 / m_zoom_factor;
		float vpDim = (VP_SIZE * scale_factor) / 2;

		#define ENSURE_RANGE_A_INSIDE_RANGE_B(a, aSize, bMin, bMax) \
		if (2*aSize >= (bMax - bMin)) \
			a = 0; \
		else if ((a - aSize < bMin) || (a + aSize > bMax)) {  \
			float diff; \
			if (a - aSize < bMin) diff = bMin - (a - aSize); \
			else diff = bMax - (a + aSize); \
			a += diff; \
		}
		
		ENSURE_RANGE_A_INSIDE_RANGE_B(m_zoom_cx, vpDim, squareVertices[0], squareVertices[2])
		ENSURE_RANGE_A_INSIDE_RANGE_B(m_zoom_cy, vpDim, squareVertices[1], squareVertices[7])
	   
		load_orthographic_matrix(
			m_zoom_cx - vpDim, 
			m_zoom_cx + vpDim, 
			m_zoom_cy - vpDim, 
			m_zoom_cy + vpDim, 
			0, 0.5, mat);
	} 
	else 
	{
		load_orthographic_matrix(- VP_SIZE * 0.5, VP_SIZE * 0.5, - VP_SIZE * 0.5, VP_SIZE * 0.5, 0, 0.5, mat);
	}
	
	GL_OPERATION(glUniformMatrix4fv(m_uniforms[UNIFORM_PROJ_MATRIX], 1, GL_FALSE, mat))
	
	float rad = (2.0 * 3.14157 * orientation / 360.0); // Convert orientation to radian
	
	GL_OPERATION(glUniform1f(m_uniforms[UNIFORM_ROTATION], rad))
	
	GL_OPERATION(glActiveTexture(GL_TEXTURE0))
	GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[m_texture_index][type][Y]))
	GL_OPERATION(glUniform1i(m_uniforms[UNIFORM_TEXTURE_Y], 0))
	GL_OPERATION(glActiveTexture(GL_TEXTURE1))
	GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[m_texture_index][type][U]))
	GL_OPERATION(glUniform1i(m_uniforms[UNIFORM_TEXTURE_U], 1))
	GL_OPERATION(glActiveTexture(GL_TEXTURE2))
	GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[m_texture_index][type][V]))
	GL_OPERATION(glUniform1i(m_uniforms[UNIFORM_TEXTURE_V], 2))
	
	GL_OPERATION(glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices))
	GL_OPERATION(glEnableVertexAttribArray(ATTRIB_VERTEX))
	GL_OPERATION(glVertexAttribPointer(ATTRIB_UV, 2, GL_FLOAT, 1, 0, squareUvs))
	GL_OPERATION(glEnableVertexAttribArray(ATTRIB_UV))
	
	GL_OPERATION(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4))   
	
	check_GL_errors("ogl_display_render_type");
	
}

void android_gles_render::ogl_display_render(int orientation) 
{
	ogl_display_render_type(REMOTE_IMAGE, true, 0, 0, 1, 1, orientation);
	// preview image already have the correct orientation
	ogl_display_render_type(PREVIEW_IMAGE, false, 0.4f, -0.4f, 0.2f, 0.2f, 0);
	
	m_texture_index = (m_texture_index + 1) % TEXTURE_BUFFER_SIZE;
}

void android_gles_render::ogl_display_zoom(float* params) 
{
	m_zoom_factor = params[0];
	m_zoom_cx = params[1] - 0.5;
	m_zoom_cy = params[2] - 0.5;
}


///////////////////////////////////////////////////////////////////////////////////////////////////


void android_gles_render::check_GL_errors(const char* context) 
{
	int maxIterations=10;
	GLenum error;
	while (((error = glGetError()) != GL_NO_ERROR) && maxIterations > 0)
	{
		switch(error)
		{
			case GL_INVALID_ENUM:  __android_log_print(ANDROID_LOG_INFO,"opengles_display","[%2d]GL error: '%s' -> GL_INVALID_ENUM\n", maxIterations, context); break;
			case GL_INVALID_VALUE: __android_log_print(ANDROID_LOG_INFO,"opengles_display","[%2d]GL error: '%s' -> GL_INVALID_VALUE\n", maxIterations, context); break;
			case GL_INVALID_OPERATION: __android_log_print(ANDROID_LOG_INFO,"opengles_display","[%2d]GL error: '%s' -> GL_INVALID_OPERATION\n", maxIterations, context); break;
			case GL_OUT_OF_MEMORY: __android_log_print(ANDROID_LOG_INFO,"opengles_display","[%2d]GL error: '%s' -> GL_OUT_OF_MEMORY\n", maxIterations, context); break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: __android_log_print(ANDROID_LOG_INFO,"opengles_display","[%2d]GL error: '%s' -> GL_INVALID_FRAMEBUFFER_OPERATION\n", maxIterations, context); break;
			default:
				__android_log_print(ANDROID_LOG_INFO,"opengles_display","[%2d]GL error: '%s' -> %x\n", maxIterations, context, error);
		}
		  maxIterations--;
	}
}

bool android_gles_render::load_shaders(GLuint* program, GLint* uniforms) {
#include "yuv2rgb.vs.h"
#include "yuv2rgb.fs.h"
	yuv2rgb_fs_len = yuv2rgb_fs_len;
	yuv2rgb_vs_len = yuv2rgb_vs_len;
	
	GLuint vertShader, fragShader;
	*program = glCreateProgram();

	if (!compileShader(&vertShader, GL_VERTEX_SHADER, (const char*)yuv2rgb_vs))
		return false;
	if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, (const char*)yuv2rgb_fs))
		return false;

	GL_OPERATION(glAttachShader(*program, vertShader))
	GL_OPERATION(glAttachShader(*program, fragShader))

	GL_OPERATION(glBindAttribLocation(*program, ATTRIB_VERTEX, "position"))
	GL_OPERATION(glBindAttribLocation(*program, ATTRIB_UV, "uv"))

	if (!linkProgram(*program))
		return false;

	uniforms[UNIFORM_PROJ_MATRIX] = glGetUniformLocation(*program, "proj_matrix");
	uniforms[UNIFORM_ROTATION] = glGetUniformLocation(*program, "rotation");
	uniforms[UNIFORM_TEXTURE_Y] = glGetUniformLocation(*program, "t_texture_y");
	uniforms[UNIFORM_TEXTURE_U] = glGetUniformLocation(*program, "t_texture_u");
	uniforms[UNIFORM_TEXTURE_V] = glGetUniformLocation(*program, "t_texture_v");

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
	
	check_GL_errors("load_shaders");
}

void android_gles_render::load_orthographic_matrix(float left, float right, float bottom, float top, float near, float far, float* mat)
{
	float r_l = right - left;
	float t_b = top - bottom;
	float f_n = far - near;
	float tx = - (right + left) / (right - left);
	float ty = - (top + bottom) / (top - bottom);
	float tz = - (far + near) / (far - near);

	mat[0] = (2.0f / r_l);
	mat[1] = mat[2] = mat[3] = 0.0f;

	mat[4] = 0.0f;
	mat[5] = (2.0f / t_b);
	mat[6] = mat[7] = 0.0f;

	mat[8] = mat[9] = 0.0f;
	mat[10] = -2.0f / f_n;
	mat[11] = 0.0f;

	mat[12] = tx;
	mat[13] = ty;
	mat[14] = tz;
	mat[15] = 1.0f;
}

void android_gles_render::allocate_gl_textures(int w, int h, enum ImageType type) 
{
	int j;
	for(j=0; j<TEXTURE_BUFFER_SIZE; j++) 
	{
		GL_OPERATION(glActiveTexture(GL_TEXTURE0))
		GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[j][type][Y]))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
		GL_OPERATION(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0))

		GL_OPERATION(glActiveTexture(GL_TEXTURE1))
		GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[j][type][U]))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
		GL_OPERATION(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w >> 1, h >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0))

		GL_OPERATION(glActiveTexture(GL_TEXTURE2))
		GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[j][type][V]))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
		GL_OPERATION(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
		GL_OPERATION(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w >> 1, h >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0))

	}
	m_allocatedTexturesSize[type].width =  w;
	m_allocatedTexturesSize[type].height =  h;

	 __android_log_print(ANDROID_LOG_INFO,"opengles_display","%s: allocated new textures[%d] (%d x %d)\n", __FUNCTION__, type, w, h);
	
	check_GL_errors("allocate_gl_textures");
}

unsigned int android_gles_render::align_on_power_of_2(unsigned int value) 
{
	int i;
	/* browse all power of 2 value, and find the one just >= value */
	for(i=0; i<32; i++) 
	{
		unsigned int c = 1 << i;
		if (value <= c)
			return c;
	}
	return 0;
}

void android_gles_render::yuv_buf_init(Picture *buf, int w, int h, uint8_t *ptr)
{
	int ysize,usize;
	ysize=w*h;
	usize=ysize/4;
	buf->w=w;
	buf->h=h;
	buf->planes[0]=ptr;
	buf->planes[1]=buf->planes[0]+ysize;
	buf->planes[2]=buf->planes[1]+usize;
	buf->planes[3]=0;
	buf->strides[0]=w;
	buf->strides[1]=w/2;
	buf->strides[2]=buf->strides[1];
	buf->strides[3]=0;
}

bool android_gles_render::update_textures_with_yuv(enum ImageType type) 
{
	unsigned int aligned_yuv_w, aligned_yuv_h;
	Picture yuvbuf;

	yuv_buf_init(&yuvbuf,m_img_w,m_img_h,m_yuv[type]);

	if (yuvbuf.w == 0 || yuvbuf.h == 0) 
	{
		__android_log_print(ANDROID_LOG_INFO,"opengles_display","Incoherent image size: %dx%d\n", yuvbuf.w, yuvbuf.h);
		return false;
	}
	aligned_yuv_w = align_on_power_of_2(yuvbuf.w);
	aligned_yuv_h = align_on_power_of_2(yuvbuf.h);

	/* check if we need to adjust texture sizes */
	if (aligned_yuv_w != m_allocatedTexturesSize[type].width ||
		aligned_yuv_h != m_allocatedTexturesSize[type].height) 
	{
		allocate_gl_textures(aligned_yuv_w, aligned_yuv_h, type);
	}
	m_uvx[type] = yuvbuf.w / (float)(m_allocatedTexturesSize[type].width+1);
	m_uvy[type] = yuvbuf.h / (float)(m_allocatedTexturesSize[type].height+1);

	/* upload Y plane */
	GL_OPERATION(glActiveTexture(GL_TEXTURE0))
	GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[m_texture_index][type][Y]))
	GL_OPERATION(glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0, yuvbuf.w, yuvbuf.h,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvbuf.planes[Y]))
	GL_OPERATION(glUniform1i(m_uniforms[UNIFORM_TEXTURE_Y], 0))

	/* upload U plane */
	GL_OPERATION(glActiveTexture(GL_TEXTURE1))
	GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[m_texture_index][type][U]))
	GL_OPERATION(glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0, yuvbuf.w >> 1, yuvbuf.h >> 1,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvbuf.planes[U]))
	GL_OPERATION(glUniform1i(m_uniforms[UNIFORM_TEXTURE_U], 1))

	/* upload V plane */
	GL_OPERATION(glActiveTexture(GL_TEXTURE2))
	GL_OPERATION(glBindTexture(GL_TEXTURE_2D, m_textures[m_texture_index][type][V]))
	GL_OPERATION(glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0, yuvbuf.w >> 1, yuvbuf.h >> 1,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvbuf.planes[V]))
	GL_OPERATION(glUniform1i(m_uniforms[UNIFORM_TEXTURE_V], 2))

	m_yuv_size[type].width = yuvbuf.w;
	m_yuv_size[type].height = yuvbuf.h;

	check_GL_errors("update_textures_with_yuv");
	
	return true;
}
