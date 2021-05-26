/*/*******************************************************************************
**                                                                            **
**                     Jiedi(China nanjing)Ltd.                               **
**	               ������������ �Ĳܿ����˴��������Ϊѧϰ�ο�                **
*******************************************************************************/

/*****************************FILE INFOMATION***********************************
**
** Project       :FFmpeg 4.2 �ӻ���ʵս-��·H265���¼�ſ��� ʵѵ��

** Contact       : xiacaojun@qq.com
**  ����   : http://blog.csdn.net/jiedichina
**	��Ƶ�γ� : �����ƿ���	http://study.163.com/u/xiacaojun		
			   ��Ѷ����		https://jiedi.ke.qq.com/				
			   csdnѧԺ               http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961	
**             51ctoѧԺ              http://edu.51cto.com/lecturer/index/user_id-12016059.html	
** 			   ���Ŀ���		http://www.laoxiaketang.com 
**                              �����������ڴ���ҳ����            http://ffmpeg.club
**  FFmpeg 4.2 �ӻ���ʵս-��·H265���¼�ſ��� ʵѵ��  �γ�Ⱥ ��639014264����Ⱥ���ش����ѧԱ����
**                           ΢�Ź��ں�  : jiedi2007
**		ͷ����	 : �Ĳܿ�
**
*****************************************************************************
//������������������FFmpeg 4.2 �ӻ���ʵս-��·H265���¼�ſ��� ʵѵ�� �γ�  QQȺ��639014264���ش����ѧԱ����*/
#ifndef XVIDEO_VIEW_H
#define XVIDEO_VIEW_H
#include <mutex>
#include <fstream>
struct AVFrame;

void MSleep(unsigned int ms);

//��ȡ��ǰʱ��� ����
long long NowMs();

////////////////////////////////////
/// ��Ƶ��Ⱦ�ӿ���
/// ����SDLʵ��
/// ��Ⱦ���������
// �̰߳�ȫ
class XVideoView
{
public:
    enum Format  //ö�ٵ�ֵ��ffmpeg��һ��
    {
        YUV420P = 0,
        NV12 = 23,
        ARGB = 25,
        RGBA = 26,
        BGRA = 28
    };
    enum RenderType
    {
        SDL = 0
    };
    static XVideoView* Create(RenderType type=SDL);

    ////////////////////////////////////////////////
    /// ��ʼ����Ⱦ���� �̰߳�ȫ �ɶ�ε���
    /// @para w ���ڿ��
    /// @para h ���ڸ߶�
    /// @para fmt ���Ƶ����ظ�ʽ
    /// @para win_id ���ھ�������Ϊ�գ������´���
    /// @return �Ƿ񴴽��ɹ�
    virtual bool Init(int w, int h,
        Format fmt = RGBA) = 0;

    //���������������Դ�������رմ���
    virtual void Close() = 0;

    //�������˳��¼�
    virtual bool IsExit() = 0;

    //////////////////////////////////////////////////
    /// ��Ⱦͼ�� �̰߳�ȫ
    ///@para data ��Ⱦ�Ķ���������
    ///@para linesize һ�����ݵ��ֽ���������YUV420P����Yһ���ֽ���
    /// linesize<=0 �͸��ݿ�Ⱥ����ظ�ʽ�Զ������С
    /// @return ��Ⱦ�Ƿ�ɹ�
    virtual bool Draw(const unsigned  char* data, int linesize = 0) = 0;
    virtual bool Draw(
        const unsigned  char* y, int y_pitch,
        const unsigned  char* u, int u_pitch,
        const unsigned  char* v, int v_pitch
        ) = 0;

    
    //��ʾ����
    void Scale(int w, int h)
    {
        scale_w_ = w;
        scale_h_ = h;
    }

    bool DrawFrame(AVFrame* frame);

    int render_fps() { return render_fps_; }

    //���ļ�
    bool Open(std::string filepath);


    //////////////////////////////////////
    /// ��ȡһ֡���ݣ���ά��AVFrame�ռ�
    /// ÿ�ε��ûḲ����һ������
    AVFrame* Read();
    void set_win_id(void* win) { win_id_ = win; }
    virtual ~XVideoView();
protected:
    void* win_id_ = nullptr; //���ھ��
    int render_fps_ = 0;       //��ʾ֡��
    int width_ = 0;             //���ʿ��
    int height_ = 0;
    Format fmt_ = RGBA;         //���ظ�ʽ
    std::mutex mtx_;            //ȷ���̰߳�ȫ
    int scale_w_ = 0;           //��ʾ��С
    int scale_h_ = 0;
    long long beg_ms_ = 0;       //��ʱ��ʼʱ��
    int count_ = 0;              //ͳ����ʾ����
    
private:
    std::ifstream ifs_;
    AVFrame* frame_ = nullptr;
    unsigned char* cache_ = nullptr;//����NV12����
};

#endif
