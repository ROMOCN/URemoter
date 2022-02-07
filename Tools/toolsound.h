#ifndef TOOL_SOUND_H
#define TOOL_SOUND_H
#include <iostream>
#include <windows.h>
#include <QDebug>
#include <Mmsystem.h>
#include <thread>
#include <iostream>
#include <QObject>
//#pragma comment(lib, "winmm.lib")
class USER
{
public:
    int a = 0;
    char b = 'a';
};
static BYTE* file = (BYTE*)malloc(sizeof(BYTE) * 100);
static DWORD    hasRecorded = 0;
static BOOL     recurr = TRUE;
class ToolSound :public QObject
{
    Q_OBJECT
protected:
    const int buffSize = 1024 * 10000;
    HWAVEIN         hWaveIn;		        //输入设备
    HWAVEOUT        hWaveOut;		        //输出设备
    WAVEFORMATEX    waveform;	            //定义音频流格式
    BYTE* pBuffer1, * pBuffer2;				//输入音频缓冲区（左右声道）
    WAVEHDR         whdr_i1, whdr_i2;       //输入音频头
    WAVEHDR         whdr_o;                //输出音频头
    USER* user = new USER();		//定义用户
    bool started = false;
    std::thread *t_sound;
    BYTE* temp;
    int t = 0;
    MMRESULT res;


public:
    ToolSound();
    ~ToolSound();
    void Sound_Start();
    void Sound_Stop();
    unsigned char* charToUc(char* read_buff)
    {
        return  reinterpret_cast<unsigned char*> (read_buff);
    }
    char* ucToChar(unsigned char* u_read_buff)
    {
        return  reinterpret_cast<char*> (u_read_buff);
    }
    BYTE* Sound_get()
    {
        return file;
    }
    int Sound_len()
    {
        return hasRecorded;
    }

    void Sound_Listen();

    void Sound_Listen(BYTE* data,int len);

private:


    void Run();
    void Sound_Inital();

    static void CALLBACK callback(HWAVEIN   hwi,                              // 设备句柄
        UINT      uMsg,							   // 消息
        DWORD_PTR dwInstance,					   // 对象
        DWORD_PTR dwParam1,						   // 参数1
        DWORD_PTR dwParam2);


    //--------------
    DWORD m_dwAudioInId;
    HANDLE m_hAudioIn;
       static  BOOL m_bThreadStart;
    BOOL m_bDevOpen;
    BOOL m_bAllocBuffer;
    BOOL m_bRecord;
    static DWORD s_dwInstance;
    BOOL StartThread()
    {
        if (m_bThreadStart)
        {
            qDebug()<<("CWaveIn::StartThread: Wave in thread has run.\n");
            return FALSE;
        }
        unsigned int id = 0;
        m_bThreadStart = TRUE;
        m_hAudioIn=CreateThread(0,0,AudioInThreadProc,this,0,&m_dwAudioInId);
        if(!m_hAudioIn)
        {
            qDebug()<<("CWaveIn::StartThread: Strat wave in thread fail.\n");
            m_bThreadStart = FALSE;
            return FALSE;
        }
        return TRUE;
    }

    BOOL StopThread()
    {
        if (!m_bThreadStart)
        {
            qDebug()<<("CWaveIn::StopThread: Wave in thread hasn't run.\n");
            return FALSE;
        }

        if(m_hAudioIn)
        {
            int t=50;
            DWORD ExitCode;
            BOOL bEnd=FALSE;
            PostThreadMessage(m_dwAudioInId,WM_QUIT,0,0);
            while(t)
            {
                GetExitCodeThread(m_hAudioIn,&ExitCode);
                if(ExitCode!= STILL_ACTIVE)
                {
                    bEnd=TRUE;
                    break;
                }
                else
                    Sleep(10);
                t--;
            }
            if(!bEnd)
            {
                TerminateThread(m_hAudioIn,0);
                qDebug()<<("CWaveIn::StopThread: TerminateThread wave in thread.\n");
            }
            m_hAudioIn=0;
        }
        m_bThreadStart = FALSE;
        return TRUE;
    }

    static DWORD WINAPI AudioInThreadProc(LPVOID lpParameter)
    {
        const int SIZE_AUDIO_FRAME = 960;
        qDebug()<<("AudioInThreadProc start.\n");

        char buffer[1024];
        MSG msg;

        while(m_bThreadStart)
        {

            while(GetMessage(&msg,0,0,0))
            {
                switch(msg.message)
                {
                case MM_WIM_OPEN:
                    qDebug()<<("MM_WIM_OPEN \n");
                    break;
                case MM_WIM_CLOSE:
                    qDebug()<<("MM_WIM_CLOSE \n");
                    break;
                case MM_WIM_DATA:
                    qDebug()<<(" MM_WIM_DATA\n");

                    WAVEHDR* pWH=(WAVEHDR*)msg.lParam;
                    waveInUnprepareHeader((HWAVEIN)msg.wParam,pWH,sizeof(WAVEHDR));

                    if(pWH->dwBytesRecorded!=SIZE_AUDIO_FRAME)
                        break;

                    memcpy(buffer,pWH->lpData,pWH->dwBytesRecorded);

                    waveInPrepareHeader((HWAVEIN)msg.wParam,pWH,sizeof(WAVEHDR));
                    waveInAddBuffer((HWAVEIN)msg.wParam,pWH,sizeof(WAVEHDR));

                    break;
                }
            }

        }
        qDebug()<<("AudioInThreadProc exit.\n");
        return msg.wParam;
    }


    //    bool WriteDataToFile(LPSTR pData, DWORD dwSize)
    //    {
    //        CFile file;
    //        CFileException fileException;
    //        file.Open(m_strFileName,CFile::modeCreate|CFile::modeReadWrite, &fileException);
    //        DWORD WaveHeaderSize = 38; // RIFF 12个字节, 格式块26个字节
    //         DWORD WaveFormatSize = 18; // 格式块内容长度
    //        file.SeekToBegin();
    //        file.Write("RIFF",4);
    //        // RIFF + 格式块 +数据块长度[8+dwSize]
    //        long int fileLen=(8 + dwSize + WaveHeaderSize);
    //        file.Write(&fileLen,sizeof(fileLen));

    //        file.Write("WAVE",4);
    //        file.Write("fmt ",4);
    //        file.Write(&WaveFormatSize,sizeof(WaveFormatSize));
    //        file.Write(&m_wfx.wFormatTag,sizeof(m_wfx.wFormatTag));
    //        file.Write(&m_wfx.nChannels,sizeof(m_wfx.nChannels));
    //        file.Write(&m_wfx.nSamplesPerSec,sizeof(m_wfx.nSamplesPerSec));
    //        file.Write(&m_wfx.nAvgBytesPerSec,sizeof(m_wfx.nAvgBytesPerSec));
    //        file.Write(&m_wfx.nBlockAlign,sizeof(m_wfx.nBlockAlign));
    //        file.Write(&m_wfx.wBitsPerSample,sizeof(m_wfx.wBitsPerSample));
    //        file.Write(&m_wfx.cbSize,sizeof(m_wfx.cbSize)); // 附加信息，2个字节
    //        file.Write("data",4);
    //        file.Write(&dwSize,sizeof(dwSize));
    //        file.Write(pData,dwSize);
    //        file.Seek(dwSize,CFile::begin);
    //        file.Close();
    //        return TRUE;
    //    }
    //    bool ReadData(char **pData, DWORD *pdwSize, WAVEFORMATEX *pwfx)
    //    {

    //        CFile file(m_strFileName,CFile::modeRead);
    //        int nSize = file.GetLength();
    //        char *pBuf = new char [nSize+1];
    //        nSize = file.Read(pBuf,nSize+1);

    //        //long int nFileLen = 0;
    //        //memcpy(&nFileLen,pBuf+4,4); // WAVE文件长度

    //        int nFormatLen = 0;
    //        memcpy(&nFormatLen,pBuf+12+4,4); // 读取wave格式的大小

    //        memcpy(pwfx,pBuf+12+8,nFormatLen); // 读取wave格式

    //        char dataflag[5];
    //        memcpy(dataflag,pBuf+12+8+nFormatLen,4); // 读取数据标志
    //        dataflag[4] = '\0';
    //        int nFactLen = 0;
    //        if(strcmp(dataflag,"fact") == 0) // 判断是否包含附加块
    //        {
    //            nFactLen += 12;
    //        }
    //        DWORD nDataLen = 0;
    //        memcpy(&nDataLen,pBuf+12+8+nFormatLen+nFactLen+4,4); // 读取样本大小
    //        *pData = new char[nDataLen];

    //        memcpy(*pData,pBuf+12+8+nFormatLen+nFactLen+4+4,nDataLen); // 复制数据
    //        *pdwSize = nDataLen;

    //        SAFE_DELETE(pBuf);
    //        file.Close();
    //        return TRUE;
    //    }

};

#endif // TOOL_SOUND_H
