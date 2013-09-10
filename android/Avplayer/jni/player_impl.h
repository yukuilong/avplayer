//
// player_impl.h
// ~~~~~~~~~~~~~
//
// Copyright (c) 2011 Jack (jack.wgm@gmail.com)
//
#ifndef __PLAYER_IMPL_H__
#define __PLAYER_IMPL_H__

#include "avplay.h"
#include "audio_out.h"
#include "video_out.h"
#include "source.h"
#include "demux.h"
#include "globals.h"

#include <stdio.h>
#include <math.h>
//#include <tchar.h>

#include <string>
#include <map>

#pragma once

#define MAX_PATH 255


// 打开媒体类型.
#define MEDIA_TYPE_FILE	0
#define MEDIA_TYPE_BT	1
#define MEDIA_TYPE_HTTP 2
#define MEDIA_TYPE_RTSP 3
#define MEDIA_TYPE_YK	4

// 渲染模式.
#define RENDER_ANDROID_OGL      0


// 日志类.
class avplay_logger
{
public:
	avplay_logger() { ::logger_to_file("avplayer.log"); }
	~avplay_logger() { ::close_logger_file(); }
};

class player_impl
{
public:
	player_impl(void);
	~player_impl(void);

public:
	// 打开一个媒体文件, movie是文件名, media_type可以是MEDIA_TYPE_FILE,
	// 也可以是MEDIA_TYPE_BT, 注意, 这个函数只打开文件, 但并不播放.
	// 重新打开文件前, 必须关闭之前的媒体文件, 否则可能产生内存泄漏!
	// 另外, 在播放前, avplayer必须拥有一个窗口.
	bool open(const char *movie, int media_type, int render_type);

	// 开始播放视频.
	// fact 表示播放视频的起始位置, 按视频百分比计算, 默认从文件头开始播放.
	// index 播放索引为index的文件, index表示在播放列表中的位置计数, 从0开始计
	// 算, index主要用于播放多文件的bt文件, 单个文件播放可以使用直接默认为0而不
	// 需要填写参数.
	bool play(double fact = 0.0f, int index = 0);

	// 暂停播放.
	bool pause();

	// 继续播放.
	bool resume();

	// 停止播放.
	bool stop();

	// 等待播放直到完成.
	bool wait_for_completion();

	// 关闭媒体, 如果打开的是一个bt文件, 那么
	// 在这个bt文件中的所有视频文件将被关闭.
	bool close();

	// seek到某个时间播放, 按视频时长的百分比.
	void seek_to(double fact);

	// 设置声音音量大小.
	void volume(double l, double r);

	// 静音切换.
	void toggle_mute();

	// 静音设置.
	void mute_set(bool s);

	// 当前下载速率, 单位kB/s.
	int download_rate();

	// 限制下载速率.
	void set_download_rate(int k);

	// 返回当前播放时间.
	double curr_play_time();

	// 当前播放视频的时长, 单位秒.
	double duration();

	// 当前播放视频的高, 单位像素.
	int video_width();

	// 当前播放视频的宽, 单位像素.
	int video_height();

	void set_window_handle(unsigned long window);
	unsigned int get_window_handle();

	// 当前缓冲进度, 单位百分比.
	double buffering();

	// 返回当前播放列表, key对应的是打开的媒体文件名.
	// value是打开的媒体文件下的视频文件.
	// 比如说打开一个bt种子文件名为avtest.torrent, 在这
	// 个avtest.torrent种子里包括了2个视频文件, 假如为:
	// av1.mp4, av2.mp4, 那么这个列表应该为:
	// avtest.torrent->av1.mp4
	// avtest.torrent->av2.mp4
	std::map<std::string, std::string>& play_list();


    	// 解析yk相关视频文件(暂不支持视频组)
    	void parse_yk_videos(const std::string& vid);

private:
	// 播放器相关的函数.
	void init_file_source(source_context *sc);
	void init_torrent_source(source_context *sc);
	void init_yk_source(source_context *sc);
	void init_audio(ao_context *ao);
	void init_video(vo_context *vo, int render_type);

	// 实时处理视频渲染的视频数据, 在这里完成比较加字幕, 加水印等操作.
	static int draw_frame(struct vo_context *ctx, AVFrame* data, int pix_fmt, double pts);

private:
	avplay_logger m_log;
	
	// 播放器主要由下面几个部件组成.
	// avplay 是整合source_context和ao_context,vo_context的大框架.
	// 由 source_context实现数据读入视频数据.
	// 由 avplay负责分离音视并解码.
	// 最后由ao_context和vo_context分别负责输出音频和视频.
	avplay *m_avplay;
	source_context *m_source;
	ao_context *m_audio;
	vo_context *m_video;

	int (*m_draw_frame)(struct vo_context *ctx, AVFrame* data, int pix_fmt, double pts);

	// 媒体文件信息.
	std::map<std::string, std::string> m_media_list;
	int m_cur_index;

	// 视频宽高.
	int m_video_width;
	int m_video_height;

	unsigned long m_window;

	// 同步一些操作.
	pthread_mutex_t m_mutex;

	// 声音选项.
	bool m_mute;
};

#endif // __PLAYER_IMPL_H__
