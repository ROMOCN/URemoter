#include "toolSound.h"
DWORD ToolSound::s_dwInstance = 0;
BOOL ToolSound::m_bThreadStart = FALSE;
void ToolSound::Sound_Start()
{
    t_sound =new std::thread(&ToolSound::Run,this);
    qDebug()<<"start"<<t_sound->native_handle();
    t_sound->detach();
    StartThread();
}

void ToolSound::Sound_Stop()
{

    qDebug()<<"delete"<<t_sound->native_handle();

    if(started)
    {
        started = false;
        recurr = FALSE;
        //waveInStop(hWaveIn);
        waveInReset(hWaveIn);
        waveInClose(hWaveIn);
        Sound_Listen();
        StopThread();
    }
}

void ToolSound::Sound_Listen()
{
    HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

    // 播放录音
    whdr_o.lpData = (LPSTR)file;			// 指向buffer
    whdr_o.dwBufferLength = hasRecorded;    // buffer大小
    whdr_o.dwBytesRecorded = hasRecorded;
    whdr_o.dwFlags = 0;
    whdr_o.dwLoops = 1;

    ResetEvent(wait);
    waveOutPrepareHeader(hWaveOut, &whdr_o, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &whdr_o, sizeof(WAVEHDR));
    /*Sleep(5000);*/
    DWORD dw = WaitForSingleObject(wait, INFINITE);
    if (dw == WAIT_OBJECT_0)
    {
        std::cout << "jieshu" << std::endl;
    }

}

void ToolSound::Sound_Listen(BYTE *data, int len)
{
    HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
    WAVEHDR         whdr_o;                //输出音频头
    HWAVEOUT        hWaveOut;		        //输出设备
    WAVEFORMATEX    waveform;	            //定义音频流格式
    waveform.nSamplesPerSec = 44100;												// 采样率
    waveform.wBitsPerSample = 16;												// 采样精度
    waveform.nChannels = 2;                                                     // 声道个数
    waveform.cbSize = 0;														// 额外空间
    waveform.wFormatTag = WAVE_FORMAT_PCM;										// 音频格式
    waveform.nBlockAlign = (waveform.wBitsPerSample * waveform.nChannels) / 8;  // 块对齐
    waveform.nAvgBytesPerSec = waveform.nBlockAlign * waveform.nSamplesPerSec;  // 传输速率
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);
    // 播放录音
    whdr_o.lpData = (LPSTR)data;			// 指向buffer
    whdr_o.dwBufferLength = len;    // buffer大小
    whdr_o.dwBytesRecorded = len;
    whdr_o.dwFlags = 0;
    whdr_o.dwLoops = 1;

    ResetEvent(wait);
    waveOutPrepareHeader(hWaveOut, &whdr_o, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &whdr_o, sizeof(WAVEHDR));
    /*Sleep(5000);*/
    DWORD dw = WaitForSingleObject(wait, INFINITE);
    if (dw == WAIT_OBJECT_0)
    {
        std::cout << "jieshu" << std::endl;
    }

}

void ToolSound::Run()
{
    if(!started)
    {
        started = true;
        // 开启录音
        file = (BYTE*)malloc(sizeof(BYTE) * 512);
        hasRecorded = 0;
        MMRESULT mRet = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)callback, (DWORD_PTR)user, CALLBACK_FUNCTION);
        //MMRESULT mRet = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, m_dwAudioInId, s_dwInstance, CALLBACK_FUNCTION);
        waveInPrepareHeader(hWaveIn, &whdr_i1, sizeof(WAVEHDR));//准备buffer
        waveInPrepareHeader(hWaveIn, &whdr_i2, sizeof(WAVEHDR));//准备buffer
        waveInAddBuffer(hWaveIn, &whdr_i1, sizeof(WAVEHDR));    //添加buffer
        waveInAddBuffer(hWaveIn, &whdr_i2, sizeof(WAVEHDR));    //添加buffer
        waveInStart(hWaveIn);
        while(started)
        {
            //qDebug()<<("running");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        //            CopyMemory(temp_buff_r,whdr_i1.lpData, whdr_i1.dwBufferLength);
        //            CopyMemory(temp_buff_l,whdr_i2.lpData, whdr_i2.dwBufferLength);
        char* buff = ucToChar(file);
        //            BYTE* sound = Tool_C::charToUc(buff);
        qDebug()<<QString("length:%1").arg(strlen(buff));
        qDebug()<<QString("length:%1").arg(hasRecorded);
        //释放录音缓冲区
        waveInUnprepareHeader(hWaveIn,&whdr_i1,sizeof(WAVEHDR));
        waveInUnprepareHeader(hWaveIn,&whdr_i2,sizeof(WAVEHDR));
        //            temp = file;
        //            t = hasRecorded;
        delete t_sound;
    }
}

void ToolSound::Sound_Inital()
{
    int count = waveInGetNumDevs();
    qDebug()<<"音频输入数量："<<count;
    // 设备名称
    WAVEINCAPS waveIncaps;
    MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2
    qDebug() << "设备名称：" << waveIncaps.szPname;
    // 设置音频流格式
    waveform.nSamplesPerSec = 44100;												// 采样率
    waveform.wBitsPerSample = 16;												// 采样精度
    waveform.nChannels = 2;                                                     // 声道个数
    waveform.cbSize = 0;														// 额外空间
    waveform.wFormatTag = WAVE_FORMAT_PCM;										// 音频格式
    waveform.nBlockAlign = (waveform.wBitsPerSample * waveform.nChannels) / 8;  // 块对齐
    waveform.nAvgBytesPerSec = waveform.nBlockAlign * waveform.nSamplesPerSec;  // 传输速率
    //分配内存
    pBuffer1 = new BYTE[buffSize];
    pBuffer2 = new BYTE[buffSize];
    memset(pBuffer1, 0, buffSize);   // 内存置0
    memset(pBuffer2, 0, buffSize);   // 内存置0
    // 设置音频头
    whdr_i1.lpData = (LPSTR)pBuffer1; // 指向buffer
    whdr_i1.dwBufferLength = buffSize;     // buffer大小
    whdr_i1.dwBytesRecorded = 0;      // buffer存放大小
    whdr_i1.dwUser = 0;
    whdr_i1.dwFlags = 0;
    whdr_i1.dwLoops = 1;
    whdr_i2.lpData = (LPSTR)pBuffer2; // 指向buffer
    whdr_i2.dwBufferLength = buffSize;     // buffer大小
    whdr_i2.dwBytesRecorded = 0;      // buffer存放大小
    whdr_i2.dwUser = 0;
    whdr_i2.dwFlags = 0;
    whdr_i2.dwLoops = 1;
}

void ToolSound::callback(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)						   // 参数2
{
    // 获取对象
    USER* user2 = (USER*)dwInstance;
    // 获取音频头
    PWAVEHDR  pwhdr = (PWAVEHDR)dwParam1;

    // 处理消息
    switch (uMsg)
    {
    case WIM_OPEN:                                 // 打开录音设备

        qDebug()<<"open success..\n";
        break;

    case WIM_DATA:                                 // 缓冲区已满
    {
        qDebug()<<"buffer is full..";
        qDebug()<<QString("user:  a:%1 , b:%2").arg( user2->a).arg( user2->b);
        // 缓冲池信息
        DWORD buflen = pwhdr->dwBufferLength;
        DWORD bytrecd = pwhdr->dwBytesRecorded;
        hasRecorded += bytrecd;
        // 缓冲扩增
        file = (BYTE*)realloc(file, hasRecorded * sizeof(BYTE));
        // 存储新内容
        if (file)
        {
            memcpy(&file[hasRecorded - bytrecd], pwhdr->lpData, bytrecd);
            qDebug()<<QString("buffer size:%1 byte\n").arg(hasRecorded);
        }
        // 循环
        if (recurr)
        {
            // 加入缓存
            waveInAddBuffer(hwi, pwhdr, sizeof(WAVEHDR));
        }
    }
        break;

    case WIM_CLOSE:                               // 关闭录音设备
    {
        qDebug()<<"stop recording..";

    }
        break;

    default:
        break;
    }
}

ToolSound::ToolSound()
{
    Sound_Inital();
}

ToolSound::~ToolSound()
{

}
