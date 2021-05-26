#include <iostream>
#include <fstream>
#include "xencode.h"
using namespace std;

/*ffmpegͷ�ļ�����*/
extern "C"
{	
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
}


int main(int argc, char* argv[])
{
    string filename = "1920_1080_25_preset";
    //string filename = "166_192_288.yuv"
    AVCodecID codec_id = AV_CODEC_ID_H264;
    if (argc > 1)
    {
        string codec = argv[1];
        if (codec == "h265" || codec == "hevc")
        {
            codec_id = AV_CODEC_ID_HEVC;
        }
    }
    if (codec_id == AV_CODEC_ID_H264)
    {
        filename += ".h264";
    }
    else if (codec_id == AV_CODEC_ID_HEVC)
    {
        filename += ".h265";
    }
    //����ļ�
    ofstream ofs;
    //ofstream ofs1;
    /*�����ƴ�*/
    ofs.open(filename,ios::binary);

    XEncode en;
    /*�ҵ������������������Ĳ����أ�����Ĭ�ϲ�����ʱ����������ظ�ʽ���߳���*/
    auto c = en.Create(codec_id);
    /*������*/
    c->width = 1920;
    c->height = 1080;
    /*���ú㶨QPֵ*/
    av_opt_set_int(c->priv_data, "qp", 1,0);
    c->max_b_frames = 0; // B֡��Ϊ0 ������ʱ������ռ�
    en.set_c(c);
#if(0)
    /*�㶨�������ӣ�CRF���߼�QP��������*/
    en.SetOpt("crf", 50); 

	//Լ�����루VBV�� Constrained Encoding (VBV)
    //av_opt_set_int(c->priv_data, "crf", 23, 0);
    //c->rc_max_rate = br;
    //c->rc_buffer_size = br * 2;
#endif
    /*�򿪱�����*/
    en.Open();
    auto frame = en.CreateFrame();		// linesize = 1920, 960, 960  ������Ĭ��ֵ��

    int count = 0;//д���ļ���֡�� SPS PPS IDR����һ֡��
    for (int i = 0; i < 200; i++)
    {
        //����AVFrame ���� ÿ֡���ݲ�ͬ
        //Y
#if(1)
        for (int y = 0; y < c->height; y++)
        {
            for (int x = 0; x < c->width; x++)
            {
            	/*�ȼ���data[0]�ռ��д�� x*y�� ��ͬ���ֽڣ�һ��Yռ��1�ֽڣ�*/
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }
        //UV

        for (int y = 0; y < c->height / 2; y++)
        {
            for (int x = 0; x < c->width / 2; x++)
            {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }
#endif

#if(0)
		for (int k = 0; k < c->height * c->width; k++)
        {

            	/*�ȼ���data[0]�ռ��д�� k�� ��ͬ���ֽڣ�һ��Yռ��1�ֽڣ� linesize[0]��ʾһ�ж��ٸ�Y*/
                frame->data[0][k] = k * 3+i;
        }
		for (int k = 0; k < c->height * c->width / 4; k++)
		{
			frame->data[1][k] = 128 + k + i * 2;
			frame->data[2][k] = 64 + k + i * 5;
        }
#endif
        frame->pts = i;//��ʾ��ʱ��
        auto pkt = en.Encode(frame);
        if (pkt)
        {
            count++;
            ofs.write((char*)pkt->data, pkt->size);
            av_packet_free(&pkt);
        }
    }

	/*�������еļ�֡ȡ��*/
    auto pkts = en.End();
    for (auto pkt : pkts)
    {
        count++;
        ofs.write((char*)pkt->data, pkt->size);
        av_packet_free(&pkt);
    }


    ofs.close();
    en.set_c(nullptr);
    cout << "encode " << count << endl;

    return 0;
}