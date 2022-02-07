#ifndef ToolAudio_H
#define ToolAudio_H

#include <QWidget>
#include <QFile>
#include <QPushButton>
#include <QAudioInput>
#include <QBuffer>
#include <QAudioOutput>
#include <QDebug>
#include <QIODevice>
#include <thread>

#define FRAME_LEN_60ms 960 //每一个语音帧长度是960字节
//#define TOFILE 1
class ToolAudio : public QObject
{
    Q_OBJECT
    //input
    const int sampleRate = 16600;
    QAudioFormat audioFormat;
    QAudioDeviceInfo devInfo;
    QAudioInput *_input = nullptr;
    QIODevice *inputDevice = nullptr;
    QFile outFile;
    QByteArray databuff;
    //output
    QAudioOutput *_output = nullptr;
    QIODevice *outputDevice = nullptr;
public:
    ToolAudio(QWidget *parent = 0);
    ~ToolAudio();
    void startRecord();
    void stopRecord();
public slots:
    void soltPlay(QByteArray data);
signals:
    void signalPushSound(QByteArray data);

private:
    void audioInit();

    void startPlay(QByteArray data);
    void startPlay(QFile &file);
    void stopPlay();
    void setPlayVolumn(qreal volumn){
        _output->setVolume(volumn);
    }
    void onReadyRead();
private slots:
    void outStateChanged(QAudio::State);


};
struct WAVHEADER
{
    // RIFF 头
    char RiffName[4];
    unsigned long nRiffLength;

    // 数据类型标识符
    char WavName[4];

    // 格式块中的块头
    char FmtName[4];
    unsigned long nFmtLength;

    // 格式块中的块数据
    unsigned short nAudioFormat;
    unsigned short nChannleNumber;
    unsigned long nSampleRate;
    unsigned long nBytesPerSecond;
    unsigned short nBytesPerSample;
    unsigned short nBitsPerSample;

    // 数据块中的块头
    char    DATANAME[4];
    unsigned long   nDataLength;
};


#endif // ToolAudio_H
