#pragma once

#include <QtWidgets/QMainWindow>
#include <QMenu>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTimer>

class FFPlayCore;
struct VideoState;
class SysInfoTabWidget;
class TableView;
struct AVFrame;

class QtFFplay : public QMainWindow
{
    Q_OBJECT

public:
    QtFFplay(QWidget *parent = Q_NULLPTR);
    ~QtFFplay();
protected:
    void paintEvent(QPaintEvent* event);
private:
    void initUI();
    void initMenu();
    void initControls();
    void initConnect();
    void initSysInfo();
    void initAnalysis();
    void addInitTab(const QStringList& header,const QVector<QStringList>& data, const QString& tabName);
    void addAction(QMenu* menu, const QString& icon, const QString& text, const char* member);
    Q_SLOT void slot_currentVideoState(VideoState* is);
    Q_SLOT void slot_openLocalFile();
    Q_SLOT void slot_ffplay_play();
    Q_SLOT void slot_sizeMenu_clicked(QAction* act);
    Q_SLOT void slot_aspectMenu_clicked(QAction* act);
    Q_SLOT void slot_stretchMenu_clicked(QAction* act);
    Q_SLOT void slot_windowYuv();
    Q_SLOT void slot_windowWave();
    Q_SLOT void slot_windowRdft();
    Q_SLOT void slot_sysInfo();
    Q_SLOT void slot_vedioAnalysis();
    Q_SLOT void slot_audioAnalysis();
    Q_SLOT void slot_vedioFrame(AVFrame *frame);
    Q_SLOT void slot_audioFrame(AVFrame* frame);
private:
    FFPlayCore* m_ffpaly;
    QTextEdit* m_metaText;
    QSlider* m_slider;
    QLineEdit* m_urlLineEdit;
    QPushButton* m_startBtn;
    QPushButton* m_backBtn;
    QPushButton* m_pauseBtn;
    QPushButton* m_forwardBtn;
    QPushButton* m_stopBtn;
    QPushButton* m_perFrameBtn;
    QPushButton* m_fullScreenBtn;
    VideoState*  m_currentVideoState;
    double m_playTime;
    QTimer m_updateTimer;
    SysInfoTabWidget* m_sysInfoWidget;
    TableView* m_vedioAnalysis;
    TableView* m_audioAnalysis;
};
