#include "xencode.h"
#include <iostream>
using namespace std;
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
}


static void PrintErr(int err)
{
    char buf[1024] = { 0 };
    av_strerror(err, buf, sizeof(buf) - 1);
    cerr << buf << endl;
}

// ��������������
AVCodecContext* XEncode::Create(int codec_id)
{
    //1 �ҵ�������
    auto codec = avcodec_find_encoder((AVCodecID)codec_id);
    if (!codec)
    {
        cerr << "avcodec_find_encoder failed!" << codec_id << endl;
        return nullptr;
    }
    //2 ����������
    auto c = avcodec_alloc_context3(codec);
    if (!c)
    {
        cerr << "avcodec_alloc_context3 failed!" << codec_id << endl;
        return nullptr;
    }
    //���ò���Ĭ��ֵ,֡�ʣ�����
    c->time_base = { 1,100};
    c->pix_fmt = AV_PIX_FMT_YUV420P;
    c->thread_count = 16;
    return c;
}


// ���ö���ı����������� �����Ĵ��ݵ������У���Դ��XEncodeά��
void XEncode::set_c(AVCodecContext* c)
{
    unique_lock<mutex>lock(mux_);
    if (c_)
    {
        avcodec_free_context(&c_);
    }
    this->c_ = c;
}


bool XEncode::SetOpt(const char* key, const char* val)
{
    unique_lock<mutex>lock(mux_);
    if (!c_)return false;
    auto re = av_opt_set(c_->priv_data, key, val, 0);
    if (re != 0)
    {
        cerr << "av_opt_set failed!";
        PrintErr(re);
    }
    return true;
}

bool XEncode::SetOpt(const char* key, int val)
{
    unique_lock<mutex>lock(mux_);
    if (!c_)return false;
    auto re = av_opt_set_int(c_->priv_data, key, val, 0);
    if (re != 0)
    {
        cerr << "av_opt_set failed!";
        PrintErr(re);
    }
    return true;
}


//�򿪱����� �̰߳�ȫ
bool XEncode::Open()
{
    unique_lock<mutex>lock(mux_);
    if (!c_)return false;
    auto re = avcodec_open2(c_, NULL, NULL);
    if (re != 0)
    {
        PrintErr(re);
        return false;
    }
    return true;
}

/// �������� �̰߳�ȫ ÿ���´���AVPacket
AVPacket* XEncode::Encode(const AVFrame* frame)
{
    if (!frame)return nullptr;
    unique_lock<mutex>lock(mux_);
    if (!c_)return nullptr;
    
    //���͵������߳�
    auto re = avcodec_send_frame(c_, frame);
    if (re != 0)return nullptr;
    auto pkt = av_packet_alloc();
    //���ձ����߳�����
    re = avcodec_receive_packet(c_, pkt);
    if (re == 0)
    {
        return pkt;
    }
    av_packet_free(&pkt);
    if (re == AVERROR(EAGAIN) || re == AVERROR_EOF)
    {
        return nullptr;
    }
    if (re < 0)
    {
        PrintErr(re);
    }
    return nullptr;

}


//�������б��뻺����AVPacket
std::vector<AVPacket*> XEncode::End()
{
    std::vector<AVPacket*> res;
    unique_lock<mutex>lock(mux_);
    if (!c_)return res;
    auto re = avcodec_send_frame(c_, NULL); //����NULL ��ȡ����
    if (re != 0)return res;
    while (re >= 0)
    {
        auto pkt = av_packet_alloc();
        re = avcodec_receive_packet(c_, pkt);
        if (re != 0)
        {
            av_packet_free(&pkt);
            break;
        }
        res.push_back(pkt);
    }
    return res;
}


//����AVCodecContext ����һ��AVFrame����Ҫ�������ͷ�av_frame_free
AVFrame* XEncode::CreateFrame()
{
    unique_lock<mutex>lock(mux_);
    if (!c_)return nullptr;
    auto frame = av_frame_alloc();
    frame->width = c_->width;
    frame->height = c_->height;
    frame->format = c_->pix_fmt;
    auto re = av_frame_get_buffer(frame, 0);
    if (re != 0)
    {
        av_frame_free(&frame);
        PrintErr(re);
        return nullptr;
    }
    return frame;
}