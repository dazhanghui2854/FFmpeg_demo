#include <iostream>
#include <fstream>
#include <string>
using namespace std;

extern "C"
{ 
//引用ffmpeg头文件
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
}


int main(int argc, char* argv[])
{
    //1 分割h264 存入AVPacket
    // ffmpeg -i v1080.mp4 -s 400x300 test.h264
    string filename = "test.h264";
    ifstream ifs(filename, ios::binary);
    if (!ifs)return -1;
    /*存放裸流数据*/
    unsigned char inbuf[4096] = { 0 };
    AVCodecID codec_id = AV_CODEC_ID_H264;
    int count = 0;
    //auto begintime = NowMs();


    //1 找解码器
    auto codec = avcodec_find_decoder(codec_id);

    //2 创建解码器上下文
    auto c = avcodec_alloc_context3(codec);

    //3 打开上下文
    avcodec_open2(c, NULL, NULL);


#if(0)
//windows下硬件加速格式 DXVA2
auto hw_type = AV_HWDEVICE_TYPE_DXVA2;

//打印所有支持的硬件加速方式
for (int i = 0;; i++)
{
	auto config = avcodec_get_hw_config(codec, i);
	if (!config)break;
	if (config->device_type)
		cout << av_hwdevice_get_type_name(config->device_type) << endl;
}
//初始化硬件加速上下文
AVBufferRef *hw_ctx = nullptr;
av_hwdevice_ctx_create(&hw_ctx, hw_type, NULL,NULL, 0);

//设定硬件GPU加速
c->hw_device_ctx = av_buffer_ref(hw_ctx);
c->thread_count = 16;
#endif

	//分割上下文
	auto parser = av_parser_init(codec_id);
	auto pkt = av_packet_alloc();		//未解码的packet
	auto frame = av_frame_alloc();		//解码后的frame
    while (!ifs.eof())
    {
    	/*每次读取4096字节*/
        ifs.read((char*)inbuf, sizeof(inbuf));
        int data_size = ifs.gcount();// 读取的字节数
        if (data_size <= 0)
            break;
        auto data = inbuf;
        while (data_size > 0) //一次有多帧数据
        {
            //通过0001 截断输出到AVPacket 返回帧大小
            int ret = av_parser_parse2(parser,		//parse上下文
            	c,						//解码器上下文
                &pkt->data, &pkt->size, //输出pkt信息
                data, data_size,        //每次读取输入数据
                AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0
            );
            data += ret;		//从当前位置继续往下分析
            data_size -= ret; 	//剩余数据

            /*pkt->size ==0 此次未截取到数据*/
            if (pkt->size)
            {
            
				/*已获取当前pkt数据*/
                //发送packet到解码线程
                ret = avcodec_send_packet(c, pkt);
                if (ret < 0)		//发送失败
                    break;
                //获取多帧解码数据
                while (ret >= 0)
                {
                    //每次会调用av_frame_unref 
                    ret = avcodec_receive_frame(c, frame);
                    if (ret < 0)	//未接收到
                        break;
                    //auto curtime = NowMs();
					count++;
                }
            }
        }
    }
    ///取出缓存数据
	int ret = avcodec_send_packet(c, NULL);
	while (ret >= 0)
	{
		ret = avcodec_receive_frame(c, frame);
		if (ret < 0)
			break;
			count++;
		cout << frame->format << "-" << flush;
	}

	cout << "count:" << count << endl;
	/*资源释放*/
	av_parser_close(parser);
	avcodec_free_context(&c);
	av_frame_free(&frame);
	av_packet_free(&pkt);

    return 0;
}
