#pragma once

#include <string>
#include <vector>
#include <functional>
#include <stdarg.h>
#include "elog.h"

/*
* ���ڽ�Ӧ����־�Լ�FFmpeg�ڲ���־�������־�ļ��ͶԻ�����
* ����ģʽ������ģʽ��
*/
class Clogger
{
public:
	/**
	* @pnote: FFmpeg����־�ص���������øù��Ӻ���������־���ݴ��ݸ���һ��������
	* �ڶ��������ɵ������������ã�����ȡ����־����
	*/
	using FFmpegLogHook = std::function<void(const char*, std::vector<std::string>*)>;
public:
	static Clogger* getInstance() {
		return &m_instance;
	}
	static void FFmpegLogCallback(void* ptr, int level, const char* format, va_list vl);
	/**
	 * �������ӹ��Ӻ�������ȡFFmpeg�ڲ�����־
	 *
	 * @param hook ���Ӻ�������
	 * @param logVec ���ڴ����־��vectorָ��
	 * @note	1������ʹ��ʱ��Ҫ����������������Ϊnullptr;
	 *			2���ظ����ûḲ��ԭ��������
	 */
	static void setFFmpegLogHook(FFmpegLogHook hook, std::vector<std::string>* logVec) {
		m_ffmpegLogHook = hook;
		m_logVec = logVec;
	}

	void getLogCacheAndClean(std::string& str);
	void writeLog2Cache(const char* format, ...);

private:
	Clogger();
	~Clogger();
	Clogger(const Clogger& obj) = delete;
	Clogger& operator = (const Clogger& obj) = delete;

	void cleanUpCache();

private:
	char* m_cache_buf;
	char* m_cache_pos;
	static char m_utf8_buf[2048];	/* �����ݴ�ffmpeg�е�utf8��־ */
	static char m_ansi_buf[2048];	/* �����ݴ�ת����� ansi��־ */
	static Clogger m_instance;
	static FFmpegLogHook m_ffmpegLogHook;		/* ffmpeg��־�ص������еĹ��Ӻ��� */
	static std::vector<std::string>* m_logVec;	/* ���ڱ���ffmpeg��־������ */
};

/* ���¶��庯���� */
#define ELOG_A(...)    log_a(__VA_ARGS__), Clogger::getInstance()->writeLog2Cache(__VA_ARGS__)
#define ELOG_E(...)    log_e(__VA_ARGS__), Clogger::getInstance()->writeLog2Cache(__VA_ARGS__)
#define ELOG_W(...)    log_w(__VA_ARGS__), Clogger::getInstance()->writeLog2Cache(__VA_ARGS__)
#define ELOG_I(...)    log_i(__VA_ARGS__), Clogger::getInstance()->writeLog2Cache(__VA_ARGS__)
#define ELOG_D(...)    log_d(__VA_ARGS__), Clogger::getInstance()->writeLog2Cache(__VA_ARGS__)
