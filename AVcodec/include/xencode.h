#include <mutex>
#include <vector>
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
class XEncode
{
public:
    /* ��������������*/

    static AVCodecContext* Create(int codec_id);

    /*���� �̰߳�ȫ*/
    void set_c(AVCodecContext* c);


    /*���ñ���������̰߳�ȫ*/
    bool SetOpt(const char* key, const char* val);
    bool SetOpt(const char* key, int val);

 	/*�򿪱����� �̰߳�ȫ*/
    bool Open();

    AVPacket *Encode(const AVFrame* frame);


    /*����AVCodecContext ����һ��AVFrame����Ҫ�������ͷ�av_frame_free*/
    AVFrame* CreateFrame();

    /*�������б��뻺����AVPacket*/
    std::vector<AVPacket *> End();

private:
    AVCodecContext* c_ = nullptr;  //������������
    std::mutex mux_;               //��������������
};

