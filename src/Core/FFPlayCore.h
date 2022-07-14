#pragma once
#include <QObject>
#include <QString>
#include "PublicInclude.h"
#include "VedioState.h"

class FFPlayCore : public QObject
{
	Q_OBJECT
public:
	static FFPlayCore* instance();
	Q_SLOT int ffplay_play(const QString& url);
	int ffplay_param_global(VideoState* is);
	VideoState* stream_open(const char* fileName, AVInputFormat* iformat);
	static void stdLogInfo(int error, void* avcl, int level, const char* errInfo);
	Q_SIGNAL void signal_currentVideoState(VideoState* is);
	Q_SIGNAL void signal_playTime(double playTime);
	Q_SIGNAL void signal_vedioFrame(AVFrame* pFrame);
	Q_SIGNAL void signal_audioFrame(AVFrame* pFrame);
	void ffplay_seek(int time);
	void ffplay_pause();
	void ffplay_quit(VideoState* is);
	void ffplay_seek_step();
	void ffplay_play_fullcreen();
	void ffplay_aspectratio(VideoState* is, int num, int den);
	void ffplay_size(VideoState* is, int percentage);
	void ffplay_audio_display(int mode);
	void ffplay_stretch(int stretch);
	void ffplay_vedio_analysis(AVFrame* pFrame);
	void ffplay_audio_analysis(AVFrame* pFrame);
	static int s_stretch;

	void getSysTemInfo(QVector<QStringList>&, QVector<QStringList>&, QVector<QStringList>&
		, QVector<QStringList>&, QVector<QStringList>&);
private:
	FFPlayCore(QObject* parent = nullptr);
	~FFPlayCore();
	void ffplay_resetIndex();
private:
	static FFPlayCore* s_instance;
	AVInputFormat* m_inputFmt;
	QString m_currentInputFileName;
	int m_exitFlag;    // 退出标志
	int m_vframeIndex; // 视频帧索引
	int m_aframeIndex; // 音频帧索引
	int m_packetIndex; // packet索引
};

