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
#pragma once
#include <mutex>
#include <vector>
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

void PrintErr(int err);

////////////////////////////////////////////
//// ����ͽ���Ļ���
class XCodec
{
public:
    //////////////////////////////////////////
    /// ���������������
    /// @para codec_id ������ID�ţ���Ӧffmpeg
    /// @return ���������� ,ʧ�ܷ���nullptr
    static AVCodecContext* Create(int codec_id,bool is_encode);

    //////////////////////////////////////////
    /// ���ö���ı����������� �����Ĵ��ݵ������У���Դ��XEncodeά��
    /// ���� �̰߳�ȫ
    /// @para c ������������ ���c_��Ϊnullptr������������Դ
    void set_c(AVCodecContext* c);

    /////////////////////////////////////////////
    /// ���ñ���������̰߳�ȫ
    bool SetOpt(const char* key, const char* val);
    bool SetOpt(const char* key, int val);

    //////////////////////////////////////////////////////////////
    /// �򿪱����� �̰߳�ȫ
    bool Open();

    ///////////////////////////////////////////////////////////////
    //����AVCodecContext ����һ��AVFrame����Ҫ�������ͷ�av_frame_free
    AVFrame* CreateFrame();


protected:
    AVCodecContext* c_ = nullptr;  //������������
    std::mutex mux_;               //��������������
};

