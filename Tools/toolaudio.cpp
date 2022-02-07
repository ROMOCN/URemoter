#include "toolaudio.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QDebug>

ToolAudio::ToolAudio(QWidget *parent)
    : QObject(parent)
{
//    _startBtn = new QPushButton(QStringLiteral("开始录音"),this);
//    _endBtn = new QPushButton(QStringLiteral("结束录音"),this);
    audioInit();
}

ToolAudio::~ToolAudio()
{
#ifdef TOFILE
    outFile.close();
#else
    //inputDevice->close();
#endif
}

void ToolAudio::soltPlay(QByteArray data)
{
    startPlay(data);
}

void ToolAudio::audioInit()
{
    audioFormat.setByteOrder(QAudioFormat::LittleEndian); //这个是用来设定高低位的，分为LittleEndian（低位优先）/LargeEndian(高位优先)
    audioFormat.setChannelCount(2); //这个是用来设定声道数目，mono(平声道)的声道数目是1；stero(立体声)的声道数目是2。
    //这个用来设置编码器，"audio/pcm"在所有的平台都支持，也就相当于音频格式的WAV,
    //以线性方式无压缩的记录捕捉到的数据。 如想使用其他编码格式 ，可以通过QAudioDeviceInfo::supportedCodecs()来获取当前平台支持的编码格式。
    audioFormat.setCodec("audio/pcm");
    audioFormat.setSampleRate(sampleRate);
    audioFormat.setSampleSize(16);
    //用来设置采样类型，分为QAudioFormat::SignedInt/QAudioFormat::UnSignedInt/QAudioFormat::Float，
    //这个需要根据采样位数来设定，如果是平声道（8bit），QAudioFormat::UnSignedInt已足够。
    audioFormat.setSampleType(QAudioFormat::SignedInt);

    devInfo = QAudioDeviceInfo::defaultInputDevice();
    if(devInfo.isNull()){
        qDebug() << "未找到录音设备";
    }
    if(!devInfo.isFormatSupported(audioFormat)){ //当前使用设备是否支持
        audioFormat = devInfo.nearestFormat(audioFormat); //转换为最接近格式
    }
    _input = new QAudioInput(devInfo, audioFormat, this);
    _output = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), audioFormat, this);
    connect(_output,SIGNAL(stateChanged(QAudio::State)), this, SLOT(outStateChanged(QAudio::State)));

    outputDevice = _output->start();
    qDebug() << _output->bufferSize();
    qDebug() << QAudioDeviceInfo::defaultOutputDevice().deviceName();
    foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
         qDebug() << "Device name: " << deviceInfo.deviceName();
    }

}

void ToolAudio::startRecord()
{
    databuff.clear();
        #ifdef TOFILE
            outFile.setFileName(qApp->applicationDirPath() + "/test.wav"); //语音原始文件
            outFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
            _input->start(&outFile);
        #else
            //input开始读入输入的音频信号，写入QIODevice，这里是inputDevice
            inputDevice = _input->start();
            //_input->start(&audioBuff);
            //槽函数，当inputDevice收到input写入的音频数据之后,调用onReadyRead函数，发送数据到目标主机上
            connect(inputDevice, &QIODevice::readyRead,this, &ToolAudio::onReadyRead);
        #endif
}

void ToolAudio::stopRecord()
{
    QIODevice *device{nullptr};
    _input->stop();
    #ifdef TOFILE
        device = &outFile;
    #else
        device = inputDevice;
    #endif
    static WAVHEADER wavHeader;
    qstrcpy(wavHeader.RiffName,"RIFF");
    qstrcpy(wavHeader.WavName,"WAVE");
    qstrcpy(wavHeader.FmtName,"fmt ");
    qstrcpy(wavHeader.DATANAME,"data");

    wavHeader.nFmtLength = 16;
    int nAudioFormat = 1;
    wavHeader.nAudioFormat = nAudioFormat;
    wavHeader.nBitsPerSample = 16;
    wavHeader.nChannleNumber = 2;
    wavHeader.nSampleRate = sampleRate;
    wavHeader.nBytesPerSample = wavHeader.nChannleNumber * wavHeader.nBitsPerSample / 8;
    wavHeader.nBytesPerSecond = wavHeader.nSampleRate * wavHeader.nChannleNumber *  wavHeader.nBitsPerSample / 8;
    wavHeader.nRiffLength = device->size() - 8 + sizeof(WAVHEADER);
    wavHeader.nDataLength = device->size();

    device->seek(0);
    device->write(reinterpret_cast<char*>(&wavHeader),sizeof(WAVHEADER));
    outFile.close();
//    int size  = audioData.size();
//    QByteArray b = audioData;
//    startPlay(b);
//    QBuffer buff;
//    buff.setData(audioData);
//    _output->start(&buff);
    //startPlay(outFile);
}

void ToolAudio::startPlay(QByteArray data)
{
    setPlayVolumn(100);
    outputDevice->write(data.data(), data.size());
}

void ToolAudio::startPlay(QFile &file)
{

    setPlayVolumn(100);
    //std::this_thread::sleep_for(std::chrono::seconds(10));
    file.open(QIODevice::ReadWrite);
    //_output->start(&file);
    QByteArray data = file.readAll();
    int size = data.size();
    file.close();

    outputDevice->write(data.data(), size);

    int length = data.size();
    if (length > 0) {
    int readSize = _output->periodSize();
    int chunks = _output->bytesFree() / readSize;
    _output->setBufferSize(readSize);
    while (chunks) {
        QByteArray samples = data.mid(0, readSize);
        int len = samples.size();
        data.remove(0, len);
        int ret = 0;
        if (len){

            ret = outputDevice->write(data.data(), data.size());
        }
        if (len != readSize)
            break;

        chunks--;
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    }

}

void ToolAudio::onReadyRead()
{
    QByteArray dataBuff;
    dataBuff = inputDevice->readAll();
    databuff.append(dataBuff);
    int len = dataBuff.size();
    len = dataBuff.size();
    emit signalPushSound(dataBuff);
    //startPlay(dataBuff);
}

void ToolAudio::outStateChanged(QAudio::State newState)
{
    switch (newState){
        case QAudio::IdleState:
            // Finished playing (no more data)
            //_output->stop();
            //outFile.close();
            qDebug("play end");
            break;
        case QAudio::StoppedState:
            // Stopped for other reasons
            if (_output->error() != QAudio::NoError) {
               qDebug("播放音频出现错误.\n");
            }
            break;
    case QAudio::InterruptedState:
    break;
        case QAudio::ActiveState:
            qDebug("play active");
    break;
        case QAudio::SuspendedState:
        qDebug("play Suspend");
    break;

        default:
            // ... other cases as appropriate
            break;
    }
}
