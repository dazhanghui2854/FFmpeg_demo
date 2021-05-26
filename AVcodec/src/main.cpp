#include <iostream>
#include <fstream>
#include "xencode.h"
using namespace std;

/*ffmpeg头文件引用*/
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
    //输出文件
    ofstream ofs;
    //ofstream ofs1;
    /*二进制打开*/
    ofs.open(filename,ios::binary);

    XEncode en;
    /*找到编码器、创建上下文并返回，设置默认参数：时间基数，像素格式，线程数*/
    auto c = en.Create(codec_id);
    /*编码宽高*/
    c->width = 1920;
    c->height = 1080;
    /*设置恒定QP值*/
    av_opt_set_int(c->priv_data, "qp", 1,0);
    c->max_b_frames = 0; // B帧设为0 降低延时，增大空间
    en.set_c(c);
#if(0)
    /*恒定速率因子（CRF）高级QP参数设置*/
    en.SetOpt("crf", 50); 

	//约束编码（VBV） Constrained Encoding (VBV)
    //av_opt_set_int(c->priv_data, "crf", 23, 0);
    //c->rc_max_rate = br;
    //c->rc_buffer_size = br * 2;
#endif
    /*打开编码器*/
    en.Open();
    auto frame = en.CreateFrame();		// linesize = 1920, 960, 960  ？？？默认值？

    int count = 0;//写入文件的帧数 SPS PPS IDR放在一帧中
    for (int i = 0; i < 200; i++)
    {
        //生成AVFrame 数据 每帧数据不同
        //Y
#if(1)
        for (int y = 0; y < c->height; y++)
        {
            for (int x = 0; x < c->width; x++)
            {
            	/*等价在data[0]空间中存放 x*y个 不同单字节（一个Y占用1字节）*/
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

            	/*等价在data[0]空间中存放 k个 不同单字节（一个Y占用1字节） linesize[0]表示一行多少个Y*/
                frame->data[0][k] = k * 3+i;
        }
		for (int k = 0; k < c->height * c->width / 4; k++)
		{
			frame->data[1][k] = 128 + k + i * 2;
			frame->data[2][k] = 64 + k + i * 5;
        }
#endif
        frame->pts = i;//显示的时间
        auto pkt = en.Encode(frame);
        if (pkt)
        {
            count++;
            ofs.write((char*)pkt->data, pkt->size);
            av_packet_free(&pkt);
        }
    }

	/*将缓存中的几帧取出*/
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