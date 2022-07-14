#include "QtFFplay.h"
#include <QApplication>
#include <QMenuBar>
#include <QLabel>
#include <QGroupBox>
#include <QPainter>
#include <QFileDialog>
#include "../Core/FFPlayCore.h"
#include "SysInfoTabWidget.h"
#include "TableView.h"
#include <QScrollBar>

QtFFplay::QtFFplay(QWidget *parent)
    : QMainWindow(parent), m_currentVideoState(nullptr), m_playTime(0), m_sysInfoWidget(nullptr)
{
	initUI();
	m_ffpaly = FFPlayCore::instance();
	initConnect();
}

QtFFplay::~QtFFplay()
{
	if (m_sysInfoWidget)
		delete m_sysInfoWidget;
	if (m_vedioAnalysis)
		delete m_vedioAnalysis;
	if (m_audioAnalysis)
		delete m_audioAnalysis;
}

void QtFFplay::paintEvent(QPaintEvent* event)
{
	__super::paintEvent(event);

	QString inputType, inputformat, inputBitRate, inputTime, playTime;
	QString outputFmt, codecName, frameRate, size;
	QString sampleRate, audioCodec, channelNum;
	if (m_currentVideoState)
	{
		AVFormatContext* pFormatCtx = m_currentVideoState->ic;
		int video_stream = m_currentVideoState->video_stream;
		int audio_stream = m_currentVideoState->audio_stream;
		AVCodecParameters* pCodecCtx = pFormatCtx->streams[video_stream]->codecpar;
		AVCodecParameters* pCodecCtx_au = pFormatCtx->streams[audio_stream]->codecpar;
		if (pFormatCtx->pb != NULL) {
			URLContext* uc = (URLContext*)pFormatCtx->pb->opaque;
			URLProtocol* up = (URLProtocol*)uc->prot;
			//输入文件的协议----------
			inputType = QString::fromStdString(up->name);

		}
		inputformat = QString::fromStdString(pFormatCtx->iformat->long_name);
		inputBitRate = QString::number(pFormatCtx->bit_rate / 1000.0);

		//转换成hh:mm:ss形式
		int tns, thh, tmm, tss;
		tns = (pFormatCtx->duration) / 1000000;
		thh = tns / 3600;
		tmm = (tns % 3600) / 60;
		tss = (tns % 60);
		inputTime = QString("%1:%2:%3").arg(thh, 2, 10, QLatin1Char('0'))
			.arg(tmm, 2, 10, QLatin1Char('0')).arg(tss, 2, 10, QLatin1Char('0'));

		switch (pCodecCtx->format) {
		case 0:
			outputFmt = "YUV420P"; break;
		case 1:
			outputFmt = "YUYV422"; break;
		case 2:
			outputFmt = "RGB24"; break;
		case 3:
			outputFmt = "BGR24"; break;
		case 12:
			outputFmt = "PIX_FMT_YUVJ420P"; break;
		default:
			outputFmt = "UNKNOWN";
		}

		codecName = QString::number((int)pCodecCtx->codec_id);
		frameRate = QString::number((pFormatCtx->streams[video_stream]->r_frame_rate.num) / (pFormatCtx->streams[video_stream]->r_frame_rate.den));
		size = QString("%1 x %2").arg(pCodecCtx->width).arg(pCodecCtx->height);

		sampleRate = QString::number(pCodecCtx_au->sample_rate);
		audioCodec = QString::number((int)pCodecCtx_au->codec_id);
		channelNum = QString::number(pCodecCtx_au->channels);

		AVDictionaryEntry* m = NULL;
		QString meta;
		m_metaText->clear();
		while (m = av_dict_get(pFormatCtx->metadata, "", m, AV_DICT_IGNORE_SUFFIX)) {
			meta = (QString("%1:%2").arg(m->key).arg(m->value));
			m_metaText->append(meta);
		}
	
		thh = (int)m_playTime / 3600;
		tmm = ((int)m_playTime % 3600) / 60;
		tss = ((int)m_playTime % 60);
		playTime = QString("%1:%2:%3").arg(thh, 2, 10, QLatin1Char('0'))
			.arg(tmm, 2, 10, QLatin1Char('0')).arg(tss, 2, 10, QLatin1Char('0'));

		m_slider->setMaximum(m_currentVideoState->ic->duration / 1000000);
		m_slider->setValue((int)m_playTime);
	}
	else
	{
		m_metaText->clear();
	}

	QPainter painter(this);
	painter.drawText(QRect(32, 145, 220, 15), QString("%1%2").arg(tr(QStringLiteral("输入类型：").toStdString().c_str())).arg(inputType));
	painter.drawText(QRect(32, 170, 220, 15), QString("%1%2").arg(tr(QStringLiteral("封装格式：").toStdString().c_str())).arg(inputformat));
	painter.drawText(QRect(32, 195, 220, 15), QString("%1%2").arg(tr(QStringLiteral("比特率：").toStdString().c_str())).arg(inputBitRate));
	painter.drawText(QRect(32, 220, 220, 15), QString("%1%2").arg(tr(QStringLiteral("时长：").toStdString().c_str())).arg(inputTime));
	painter.drawText(QRect(425, 396, 100, 15), inputTime);
	painter.drawText(QRect(25, 396, 100, 15), playTime);

	painter.drawText(QRect(32, 265, 220, 15), QString("%1%2").arg(tr(QStringLiteral("输出像素格式：").toStdString().c_str())).arg(outputFmt));
	painter.drawText(QRect(32, 287, 220, 15), QString("%1%2").arg(tr(QStringLiteral("编码方式：").toStdString().c_str())).arg(codecName));
	painter.drawText(QRect(32, 309, 220, 15), QString("%1%2").arg(tr(QStringLiteral("帧率：").toStdString().c_str())).arg(frameRate));
	painter.drawText(QRect(32, 331, 220, 15), QString("%1%2").arg(tr(QStringLiteral("画面大小：").toStdString().c_str())).arg(size));

	painter.drawText(QRect(282, 265, 220, 15), QString("%1%2").arg(tr(QStringLiteral("采样率：").toStdString().c_str())).arg(sampleRate));
	painter.drawText(QRect(282, 287, 220, 15), QString("%1%2").arg(tr(QStringLiteral("编码方式：").toStdString().c_str())).arg(audioCodec));
	painter.drawText(QRect(282, 309, 220, 15), QString("%1%2").arg(tr(QStringLiteral("声道数：").toStdString().c_str())).arg(channelNum));

}

void QtFFplay::initUI()
{
	QString iconPath = QString(QApplication::applicationDirPath() + "/../source/qtffplay.ico");
	setWindowIcon(QIcon(iconPath));
	setWindowIconText("QtFFplay");
	setWindowFlags(Qt::Dialog);
	setFixedSize(515, 510);
	initMenu();
	initControls();
	initSysInfo();
	initAnalysis();
}

void QtFFplay::initMenu()
{
	QMenu* file = menuBar()->addMenu(tr(QStringLiteral("文件").toStdString().c_str()));
	addAction(file, "", QStringLiteral("打开本地文件"), SLOT(slot_openLocalFile()));

	QMenu* playWindow = menuBar()->addMenu(tr(QStringLiteral("播放窗口").toStdString().c_str()));
	addAction(playWindow, "", QStringLiteral("显示视频画面"), SLOT(slot_windowYuv()));
	addAction(playWindow, "", QStringLiteral("显示音频波形"), SLOT(slot_windowWave()));
	addAction(playWindow, "", QStringLiteral("显示音频DFT变化"), SLOT(slot_windowRdft()));
	QMenu* size = playWindow->addMenu(QIcon(""), tr(QStringLiteral("大小").toStdString().c_str()));
	size->addAction(QIcon(""), "50%");
	size->addAction(QIcon(""), "75%");
	size->addAction(QIcon(""), "100%");
	size->addAction(QIcon(""), "125%");
	size->addAction(QIcon(""), "150%");
	size->addAction(QIcon(""), "200%");
	size->addAction(QIcon(""), "400%");
	connect(size, SIGNAL(triggered(QAction*)), this, SLOT(slot_sizeMenu_clicked(QAction*)));
	QMenu* aspect = playWindow->addMenu(QIcon(""), tr(QStringLiteral("纵横比").toStdString().c_str()));
	aspect->addAction(QIcon(""), "1:1");
	aspect->addAction(QIcon(""), "4:3");
	aspect->addAction(QIcon(""), "16:9");
	aspect->addAction(QIcon(""), "16:10");
	aspect->addAction(QIcon(""), "2.35:1");
	connect(aspect, SIGNAL(triggered(QAction*)), this, SLOT(slot_aspectMenu_clicked(QAction*)));
	QMenu* stretch = playWindow->addMenu(QIcon(""), tr(QStringLiteral("窗口拉伸").toStdString().c_str()));
	stretch->addAction(QIcon(""), tr(QStringLiteral("保持纵横比").toStdString().c_str()));
	stretch->addAction(QIcon(""), tr(QStringLiteral("填充屏幕").toStdString().c_str()));
	connect(stretch, SIGNAL(triggered(QAction*)), this, SLOT(slot_stretchMenu_clicked(QAction*)));

	QMenu* viewer = menuBar()->addMenu(tr(QStringLiteral("视图").toStdString().c_str()));
	addAction(viewer, "", QStringLiteral("视频解码分析"), SLOT(slot_vedioAnalysis()));
	addAction(viewer, "", QStringLiteral("音频解码分析"), SLOT(slot_audioAnalysis()));
	addAction(viewer, "", QStringLiteral("支持信息"), SLOT(slot_sysInfo()));

	/*QMenu* languae = menuBar()->addMenu(tr(QStringLiteral("语言").toStdString().c_str()));
	addAction(languae, "", QStringLiteral("简体中文"), SLOT(slot_openLocalFile()));
	addAction(languae, "", QStringLiteral("English"), SLOT(slot_openLocalFile()));*/
}

void QtFFplay::initControls()
{
	int y_offset = 30;
	int x_offset = 17;
	QLabel* logoLabel = new QLabel(this);
	logoLabel->setGeometry(17, y_offset, 480, 67);
	logoLabel->setStyleSheet(QString("background-image:url(%1);")
		.arg(QApplication::applicationDirPath() + "/../source/ffplaymfc.bmp"));

	y_offset = y_offset + logoLabel->height() + 5;
	x_offset = 17;
	QLabel* labelUrl = new QLabel(this);
	labelUrl->setGeometry(x_offset, y_offset, 35, 20);
	labelUrl->setText("URL:");
	x_offset += labelUrl->width();
	m_urlLineEdit = new QLineEdit(this);
	m_urlLineEdit->setGeometry(x_offset + 2, y_offset, 390, 20);
	m_urlLineEdit->setFocusPolicy(Qt::ClickFocus);
	x_offset += m_urlLineEdit->width();
	QPushButton* openBtn = new QPushButton(this);
	openBtn->setGeometry(x_offset + 10, y_offset -2, 45, 25);
	openBtn->setText(tr(QStringLiteral("文件").toStdString().c_str()));
	connect(openBtn, SIGNAL(clicked()), this, SLOT(slot_openLocalFile()));

	y_offset = openBtn->height() + y_offset;
	x_offset = 17;
	QGroupBox* formatGroupBox = new QGroupBox(this);
	formatGroupBox->setTitle(tr(QStringLiteral("封装格式参数").toStdString().c_str()));
	formatGroupBox->setGeometry(x_offset, y_offset, 480, 115);

	y_offset = y_offset + 20;
	x_offset = 17 + 240;
	QLabel* metaData = new QLabel(this);
	metaData->setGeometry(x_offset, y_offset, 85, 15);
	metaData->setText("MetaData:");
	m_metaText = new QTextEdit(this);
	m_metaText->setGeometry(x_offset, y_offset + metaData->height() + 2, 215, 80);
	m_metaText->setFocusPolicy(Qt::NoFocus);
	m_metaText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_metaText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_metaText->setWordWrapMode(QTextOption::NoWrap);

	y_offset = y_offset + 100;
	x_offset = 17;
	QGroupBox* vedioGroupBox = new QGroupBox(this);
	vedioGroupBox->setTitle(tr(QStringLiteral("视频参数").toStdString().c_str()));
	vedioGroupBox->setGeometry(x_offset, y_offset, 230, 140);
	QGroupBox* audioGroupBox = new QGroupBox(this);
	x_offset = vedioGroupBox->width() + 17 + 20;
	audioGroupBox->setTitle(tr(QStringLiteral("音频参数").toStdString().c_str()));
	audioGroupBox->setGeometry(x_offset, y_offset, 230, 140);

	y_offset = y_offset + 105;
	x_offset = 37;
	QPushButton* vedioAnalysis = new QPushButton(tr(QStringLiteral("解码分析").toStdString().c_str()), this);
	vedioAnalysis->setGeometry(x_offset, y_offset, 60, 25);
	x_offset = 37 + 240;
	QPushButton* audioAnalysis = new QPushButton(tr(QStringLiteral("解码分析").toStdString().c_str()),  this);
	audioAnalysis->setGeometry(x_offset, y_offset, 60, 25);
	connect(vedioAnalysis, &QPushButton::clicked, this, &QtFFplay::slot_vedioAnalysis);
	connect(audioAnalysis, &QPushButton::clicked, this, &QtFFplay::slot_audioAnalysis);

	y_offset = y_offset + 20 + audioAnalysis->height();
	x_offset = 17 + 70;
	m_slider = new QSlider(Qt::Horizontal, this);
	m_slider->setGeometry(x_offset, y_offset, 325, 20);
	m_slider->setEnabled(false);

	y_offset = y_offset + m_slider->height() + 10;
	m_startBtn =  new QPushButton(tr(QStringLiteral("开始").toStdString().c_str()), this);
	m_startBtn->setGeometry(17, y_offset, 50, 50);
	m_backBtn = new QPushButton(tr(QStringLiteral("后退").toStdString().c_str()), this);
	m_backBtn->setGeometry(17 + 70, y_offset, 50, 50);
	m_backBtn->setEnabled(false);
	m_pauseBtn = new QPushButton(tr(QStringLiteral("暂停/\n继续").toStdString().c_str()), this);
	m_pauseBtn->setGeometry(17 + 140, y_offset, 50, 50);
	m_pauseBtn->setEnabled(false);
	m_forwardBtn = new QPushButton(tr(QStringLiteral("前进").toStdString().c_str()), this);
	m_forwardBtn->setGeometry(17 + 210, y_offset, 50, 50);
	m_forwardBtn->setEnabled(false);
	m_stopBtn = new QPushButton(tr(QStringLiteral("停止").toStdString().c_str()), this);
	m_stopBtn->setGeometry(17 +280, y_offset, 50, 50);
	m_stopBtn->setEnabled(false);
	m_perFrameBtn = new QPushButton(tr(QStringLiteral("逐帧").toStdString().c_str()), this);
	m_perFrameBtn->setGeometry(17 + 350, y_offset, 50, 50);
	m_perFrameBtn->setEnabled(false);
	m_fullScreenBtn = new QPushButton(tr(QStringLiteral("全屏").toStdString().c_str()), this);
	m_fullScreenBtn->setGeometry(17 + 420, y_offset, 50, 50);
	m_fullScreenBtn->setEnabled(false);

}

void QtFFplay::initConnect()
{
	connect(m_startBtn, SIGNAL(clicked()), this, SLOT(slot_ffplay_play()));
	connect(FFPlayCore::instance(), &FFPlayCore::signal_currentVideoState, this, &QtFFplay::slot_currentVideoState);
	connect(FFPlayCore::instance(), &FFPlayCore::signal_playTime, this, [&](double playTime) {m_playTime = playTime; });
	connect(&m_updateTimer, &QTimer::timeout, this, [&]() {update(); });
	connect(m_backBtn, &QPushButton::clicked, this, [&]() {FFPlayCore::instance()->ffplay_seek(-60);});
	connect(m_forwardBtn, &QPushButton::clicked, this, [&]() {FFPlayCore::instance()->ffplay_seek(60);});
	connect(m_stopBtn, &QPushButton::clicked, this, [&]() {FFPlayCore::instance()->ffplay_quit(m_currentVideoState); });
	connect(m_perFrameBtn, &QPushButton::clicked, this, [&]() {FFPlayCore::instance()->ffplay_seek_step(); });
	connect(m_fullScreenBtn, &QPushButton::clicked, this, [&]() {FFPlayCore::instance()->ffplay_play_fullcreen(); });
	connect(m_pauseBtn, &QPushButton::clicked, this, [&]() {FFPlayCore::instance()->ffplay_pause(); });
	connect(FFPlayCore::instance(), &FFPlayCore::signal_vedioFrame, this, &QtFFplay::slot_vedioFrame);
	connect(FFPlayCore::instance(), &FFPlayCore::signal_audioFrame, this, &QtFFplay::slot_audioFrame);
}

void QtFFplay::initSysInfo()
{
	m_sysInfoWidget = new SysInfoTabWidget;
	m_sysInfoWidget->setFixedSize(650, 650);
	QVector<QStringList> protocalInfo, ifInfo, vedioInfo, audioInfo, otherInfo;
	FFPlayCore::instance()->getSysTemInfo(protocalInfo, ifInfo, vedioInfo, audioInfo, otherInfo);
	QStringList horizontal;
	horizontal << QStringLiteral("序号") << QStringLiteral("简称");
	addInitTab(horizontal, protocalInfo, QStringLiteral("协议列表"));
	horizontal.clear();
	horizontal << QStringLiteral("序号") << QStringLiteral("简称")<< QStringLiteral("全称") << QStringLiteral("扩展名")
		<< QStringLiteral("私有数据大小");
	addInitTab(horizontal, ifInfo, QStringLiteral("封装格式列表"));
	horizontal.clear();
	horizontal << QStringLiteral("序号") << QStringLiteral("简称") << QStringLiteral("全称") << QStringLiteral("支持帧率")
		<< QStringLiteral("支持像素") << QStringLiteral("私有数据大小");
	addInitTab(horizontal, vedioInfo, QStringLiteral("视频解码器列表"));
	horizontal.clear();
	horizontal << QStringLiteral("序号") << QStringLiteral("简称") << QStringLiteral("全称") << QStringLiteral("支持采样率")
		<< QStringLiteral("支持采样格式") << QStringLiteral("私有数据大小");
	addInitTab(horizontal, audioInfo, QStringLiteral("音频解码器列表"));
	horizontal.clear();
	horizontal << QStringLiteral("序号") << QStringLiteral("简称") << QStringLiteral("全称") << QStringLiteral("私有数据大小");
	addInitTab(horizontal, otherInfo, QStringLiteral("其他解码器列表"));
}

void QtFFplay::initAnalysis()
{
	m_vedioAnalysis = new TableView;
	m_vedioAnalysis->setWindowTitle(QStringLiteral("视频解码分析"));
	QStringList horizontal;
	horizontal << QStringLiteral("序号") << QStringLiteral("帧类型") << QStringLiteral("关键帧") << QStringLiteral("码流") << "PTS";
	m_vedioAnalysis->setHorizontal(horizontal);
	m_vedioAnalysis->setFixedSize(600, 800);

	m_audioAnalysis = new TableView;
	m_audioAnalysis->setWindowTitle(QStringLiteral("音频解码分析"));
	horizontal.clear();
	horizontal << QStringLiteral("序号") << QStringLiteral("大小") << "PTS" << "DTS";
	m_audioAnalysis->setHorizontal(horizontal);
	m_audioAnalysis->setFixedSize(600, 800);
}

void QtFFplay::addInitTab(const QStringList& header, const QVector<QStringList>& data, const QString& tabName)
{
	TableView* tab = new TableView(m_sysInfoWidget);
	tab->setHorizontal(header);
	tab->setInfo(data);
	m_sysInfoWidget->addTab(tab, tabName);
}

void QtFFplay::addAction(QMenu* menu, const QString& icon, const QString& text, const char* member)
{
	QAction* action = new QAction(QIcon(icon), tr(text.toStdString().c_str()), this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), this, member);
}

void QtFFplay::slot_currentVideoState(VideoState* is)
{
	m_currentVideoState = is;

	m_startBtn->setEnabled(!((bool)m_currentVideoState));
	m_backBtn->setEnabled((bool)m_currentVideoState);
	m_pauseBtn->setEnabled((bool)m_currentVideoState);
	m_forwardBtn->setEnabled((bool)m_currentVideoState);
	m_stopBtn->setEnabled((bool)m_currentVideoState);
	m_perFrameBtn->setEnabled((bool)m_currentVideoState);
	m_fullScreenBtn->setEnabled((bool)m_currentVideoState);
	m_slider->setEnabled((bool)m_currentVideoState);

	if (m_currentVideoState)
	{
		m_updateTimer.start(1000);
	}
	else
	{
		m_updateTimer.stop();
		m_slider->setValue(0);
		m_vedioAnalysis->clear();
		m_audioAnalysis->clear();
	}

}

void QtFFplay::slot_openLocalFile()
{
	
	//文件过滤字符串。够长
	QString strfilter;
	strfilter.append("Common media formats|*.avi;*.wmv;*.wmp;*.wm;*.asf;*.rm;*.ram;*.rmvb;*.ra;*.mpg;*.mpeg;*.mpe;*.m1v;*.m2v;*.mpv2;");
	strfilter.append("*.mp2v;*.dat;*.mp4;*.m4v;*.m4p;*.vob;*.ac3;*.dts;*.mov;*.qt;*.mr;*.3gp;*.3gpp;*.3g2;*.3gp2;*.swf;*.ogg;*.wma;*.wav;");
	strfilter.append("*.mid;*.midi;*.mpa;*.mp2;*.mp3;*.m1a;*.m2a;*.m4a;*.aac;*.mkv;*.ogm;*.m4b;*.tp;*.ts;*.tpr;*.pva;*.pss;*.wv;*.m2ts;*.evo;");
	strfilter.append("*.rpm;*.realpix;*.rt;*.smi;*.smil;*.scm;*.aif;*.aiff;*.aifc;*.amr;*.amv;*.au;*.acc;*.dsa;*.dsm;*.dsv;*.dss;*.pmp;*.smk;*.flic|;;");
	strfilter.append("Windows Media Video(*.avi;*wmv;*wmp;*wm;*asf)|*.avi;*.wmv;*.wmp;*.wm;*.asf|;;");
	strfilter.append("Windows Media Audio(*.wma;*wav;*aif;*aifc;*aiff;*mid;*midi;*rmi)|*.wma;*.wav;*.aif;*.aifc;*.aiff;*.mid;*.midi;*.rmi|;;");
	strfilter.append("Real(*.rm;*ram;*rmvb;*rpm;*ra;*rt;*rp;*smi;*smil;*.scm)|*.rm;*.ram;*.rmvb;*.rpm;*.ra;*.rt;*.rp;*.smi;*.smil;*.scm|;;");
	strfilter.append("MPEG Video(*.mpg;*mpeg;*mpe;*m1v;*m2v;*mpv2;*mp2v;*dat;*mp4;*m4v;*m4p;*m4b;*ts;*tp;*tpr;*pva;*pss;*.wv;)|");
	strfilter.append("*.mpg;*.mpeg;*.mpe;*.m1v;*.m2v;*.mpv2;*.mp2v;*.dat;*.mp4;*.m4v;*.m4p;*.m4b;*.ts;*.tp;*.tpr;*.pva;*.pss;*.wv;|;;");
	strfilter.append("MPEG Audio(*.mpa;*mp2;*m1a;*m2a;*m4a;*aac;*.m2ts;*.evo)|*.mpa;*.mp2;*.m1a;*.m2a;*.m4a;*.aac;*.m2ts;*.evo|;;");
	strfilter.append("DVD(*.vob;*ifo;*ac3;*dts)|*.vob;*.ifo;*.ac3;*.dts|MP3(*.mp3)|*.mp3|CD Tracks(*.cda)|*.cda|;;");
	strfilter.append("Quicktime(*.mov;*qt;*mr;*3gp;*3gpp;*3g2;*3gp2)|*.mov;*.qt;*.mr;*.3gp;*.3gpp;*.3g2;*.3gp2|;;");
	strfilter.append("Flash Files(*.flv;*swf;*.f4v)|*.flv;*.swf;*.f4v|Playlist(*.smpl;*.asx;*m3u;*pls;*wvx;*wax;*wmx;*mpcpl)|*.smpl;*.asx;*.m3u;*.pls;*.wvx;*.wax;*.wmx;*.mpcpl|;;");
	strfilter.append("Others(*.ivf;*au;*snd;*ogm;*ogg;*fli;*flc;*flic;*d2v;*mkv;*pmp;*mka;*smk;*bik;*ratdvd;*roq;*drc;*dsm;*dsv;*dsa;*dss;*mpc;*divx;*vp6;*.ape;*.flac;*.tta;*.csf)");
	strfilter.append("|*.ivf;*.au;*.snd;*.ogm;*.ogg;*.fli;*.flc;*.flic;*.d2v;*.mkv;*.pmp;*.mka;*.smk;*.bik;*.ratdvd;*.roq;*.drc;*.dsm;*.dsv;*.dsa;*.dss;*.mpc;*.divx;*.vp6;*.ape;*.amr;*.flac;*.tta;*.csf|;;");
	strfilter.append("All Files(*.*)|*.*||");


	QString filePathName = QFileDialog::getOpenFileName(NULL, "Open File", "./", strfilter);
	if (!filePathName.isEmpty())
	{
		m_urlLineEdit->setText(filePathName);
	}
	
}

void QtFFplay::slot_ffplay_play()
{
	m_ffpaly->ffplay_play(m_urlLineEdit->text());
}

void QtFFplay::slot_sizeMenu_clicked(QAction* act)
{
	if (act->text() == "50%")
		FFPlayCore::instance()->ffplay_size(m_currentVideoState, 50);
	else if (act->text() == "75%")
		FFPlayCore::instance()->ffplay_size(m_currentVideoState, 75);
	else if (act->text() == "100%")
		FFPlayCore::instance()->ffplay_size(m_currentVideoState, 100);
	else if (act->text() == "125%")
		FFPlayCore::instance()->ffplay_size(m_currentVideoState, 125);
	else if (act->text() == "150%")
		FFPlayCore::instance()->ffplay_size(m_currentVideoState, 150);
	else if (act->text() == "200%")
		FFPlayCore::instance()->ffplay_size(m_currentVideoState, 200);
	else if (act->text() == "400%")
		FFPlayCore::instance()->ffplay_size(m_currentVideoState, 400);
}

void QtFFplay::slot_aspectMenu_clicked(QAction* act)
{
	if (act->text() == "1:1")
		FFPlayCore::instance()->ffplay_aspectratio(m_currentVideoState, 1, 1);
	else if (act->text() == "4:3")
		FFPlayCore::instance()->ffplay_aspectratio(m_currentVideoState, 4, 3);
	else if (act->text() == "16:9")
		FFPlayCore::instance()->ffplay_aspectratio(m_currentVideoState, 16, 9);
	else if (act->text() == "16:10")
		FFPlayCore::instance()->ffplay_aspectratio(m_currentVideoState, 16, 10);
	else if (act->text() == "2.35:1")
		FFPlayCore::instance()->ffplay_aspectratio(m_currentVideoState, 235, 100);
}

void QtFFplay::slot_stretchMenu_clicked(QAction* act)
{
	if (act->text() == QStringLiteral("保持纵横比"))
	{
		FFPlayCore::instance()->ffplay_stretch(0);
	}
	else
	{
		FFPlayCore::instance()->ffplay_stretch(1);
	}
}

void QtFFplay::slot_windowYuv()
{
	FFPlayCore::instance()->ffplay_audio_display(0);
}

void QtFFplay::slot_windowWave()
{
	FFPlayCore::instance()->ffplay_audio_display(1);
}

void QtFFplay::slot_windowRdft()
{
	FFPlayCore::instance()->ffplay_audio_display(2);
}

void QtFFplay::slot_sysInfo()
{
	m_sysInfoWidget->show();
}

void QtFFplay::slot_vedioAnalysis()
{
	m_vedioAnalysis->show();
}

void QtFFplay::slot_audioAnalysis()
{
	m_audioAnalysis->show();
}

void QtFFplay::slot_vedioFrame(AVFrame* frame)
{
	QStringList info;
	QString keyFrame, picType, dts, codednum;
	switch (frame->key_frame) {
	case 0:
		keyFrame = "No"; break;
	case 1:
		keyFrame = "Yes"; break;
	default:
		keyFrame = "Unknown";
	}

	switch (frame->pict_type) {
	case 0:
		picType = "Unknown"; break;
	case 1:
		picType = "I"; break;
	case 2:
		picType = "P"; break;
	case 3:
		picType = "B"; break;
	case 4:
		picType = "S"; break;
	case 5:
		picType = "SI"; break;
	case 6:
		picType = "SP"; break;
	case 7:
		picType = "BI"; break;
	default:
		picType = "Unknown";
	}

	dts = QString::number(frame->pkt_dts);
	codednum = QString::number(frame->coded_picture_number);
	info << QString::number(m_vedioAnalysis->rowCount()) << picType << keyFrame << dts << codednum;
	m_vedioAnalysis->addInfo(info);
	m_vedioAnalysis->verticalScrollBar()->setValue(m_vedioAnalysis->verticalScrollBar()->maximum());
}

void QtFFplay::slot_audioFrame(AVFrame* frame)
{
	QStringList info;
	QString index, size, dts, pts;
	
	size = QString::number(frame->pkt_size);
	//dts = QString::number(frame->pkt_dts);
	pts = QString::number(frame->pts);
	info << QString::number(m_audioAnalysis->rowCount()) << size << pts << dts;
	m_audioAnalysis->addInfo(info);
	m_audioAnalysis->verticalScrollBar()->setValue(m_audioAnalysis->verticalScrollBar()->maximum());
}
