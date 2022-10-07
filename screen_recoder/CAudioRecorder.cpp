#include <fstream>
#include <regex>
#include "CAudioRecorder.h"
#include "Clogger.h"
#include "Ctool.h"

#define MAX_ERR_BUF_SIZE	AV_ERROR_MAX_STRING_SIZE

CAudioRecorder::CAudioRecorder()
	: m_inputFormatCtx(NULL)
	, m_encoderCtx(NULL)
	, m_resampleCtx(NULL)
{
	m_errBuf = new char[MAX_ERR_BUF_SIZE];
	/* register all devicde and codec */
	avdevice_register_all();
	avcodec_register_all();

	return;
}

CAudioRecorder::~CAudioRecorder()
{
	delete[] m_errBuf;
}

void CAudioRecorder::setDevice(const std::string& devicename)
{
	char utf8str[1024];
	Ctool::getInstance()->convertANSIToUTF8(devicename.c_str(), utf8str);
	m_devicename = utf8str;
}

void CAudioRecorder::setDumpPCM(const std::string& filepath)
{
	m_pcm_filename = filepath;
}

void CAudioRecorder::setDumpAudioData(const std::string& filepath)
{
	m_audio_filename = filepath;
}

std::vector<std::string> CAudioRecorder::getDeviceList()
{
	int ret = 0;
	AVFormatContext* tmpFormatCtx = NULL;
	AVInputFormat* inputFormatCtx = av_find_input_format("dshow");
	AVDictionary* tmpDict = NULL;
	std::vector<std::string> logVec;
	bool findAudioFlag = false;

	if (ret = av_dict_set(&tmpDict, "list_devices", "true", 0) < 0) {
		dumpErr(ret);
		ELOG_E("av_dict_set failed: [%d](%s)", AVERROR(ret), m_errBuf);
		goto free_resource;
	}

	Clogger::getInstance()->setFFmpegLogHook([&findAudioFlag](const char* log, std::vector<std::string>* logList)->void
		{
			bool ret = false;
			/* ͨ��������ʽ����־�е���Ƶ�豸�����˳�����
			   �ⲿ��������Ҫ�Ķ�Դ��libavdevice/dshow.c����֪�� */

			/* libavdevice/dshow.c:dshow_read_header���ڴ�ӡ��Ƶ�����豸ǰ���ȴ�ӡ"DirectShow audio devices\n" */
			std::regex reg_audio_device_start("DirectShow audio devices\n");
			/* libavdevice/dshow.c:dshow_cycle_devices������������ʽ�Ǹ�����־��ӡ���ݱ�д�ģ�ƥ��"�豸�� (�豸����)"��ʽ���ַ��� */
			std::regex reg_audio_device_name("^\\s*\\\"(.+\\(.+\\))\\\"\n$");
			std::cmatch match_list;	/* ����洢ΪC�ַ������� */
			if (!findAudioFlag) {
				ret = std::regex_match(log, reg_audio_device_start);
				if (ret)
					findAudioFlag = true;
			} else {
				ret = std::regex_match(log, match_list, reg_audio_device_name);
				if (ret)
					logList->push_back(std::string(match_list[1]));
			}
		}, 
		&logVec);
	(void)avformat_open_input(&tmpFormatCtx, NULL, inputFormatCtx, &tmpDict);
	/* ��Ƶ�����豸�б��ȡ�����չ��Ӻ��� */
	Clogger::getInstance()->setFFmpegLogHook(NULL, NULL);

free_resource:
	if (tmpFormatCtx)
		avformat_free_context(tmpFormatCtx);
	if (tmpDict)
		av_dict_free(&tmpDict);

	return logVec;
}

int CAudioRecorder::startRecord()
{
	openDevice();

	return 0;
}

void CAudioRecorder::dumpErr(int err)
{
	av_strerror(err, m_errBuf, MAX_ERR_BUF_SIZE);
}

int CAudioRecorder::openDevice()
{

	return ERR_SUCCESS;
}


