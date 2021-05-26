#include <iostream>
#include <fstream>
#include <string>
using namespace std;

extern "C"
{ 
//����ffmpegͷ�ļ�
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
}


int main(int argc, char* argv[])
{
    //1 �ָ�h264 ����AVPacket
    // ffmpeg -i v1080.mp4 -s 400x300 test.h264
    string filename = "test.h264";
    ifstream ifs(filename, ios::binary);
    if (!ifs)return -1;
    /*�����������*/
    unsigned char inbuf[4096] = { 0 };
    AVCodecID codec_id = AV_CODEC_ID_H264;
    int count = 0;
    //auto begintime = NowMs();


    //1 �ҽ�����
    auto codec = avcodec_find_decoder(codec_id);

    //2 ����������������
    auto c = avcodec_alloc_context3(codec);

    //3 ��������
    avcodec_open2(c, NULL, NULL);


#if(0)
//windows��Ӳ�����ٸ�ʽ DXVA2
auto hw_type = AV_HWDEVICE_TYPE_DXVA2;

//��ӡ����֧�ֵ�Ӳ�����ٷ�ʽ
for (int i = 0;; i++)
{
	auto config = avcodec_get_hw_config(codec, i);
	if (!config)break;
	if (config->device_type)
		cout << av_hwdevice_get_type_name(config->device_type) << endl;
}
//��ʼ��Ӳ������������
AVBufferRef *hw_ctx = nullptr;
av_hwdevice_ctx_create(&hw_ctx, hw_type, NULL,NULL, 0);

//�趨Ӳ��GPU����
c->hw_device_ctx = av_buffer_ref(hw_ctx);
c->thread_count = 16;
#endif

	//�ָ�������
	auto parser = av_parser_init(codec_id);
	auto pkt = av_packet_alloc();		//δ�����packet
	auto frame = av_frame_alloc();		//������frame
    while (!ifs.eof())
    {
    	/*ÿ�ζ�ȡ4096�ֽ�*/
        ifs.read((char*)inbuf, sizeof(inbuf));
        int data_size = ifs.gcount();// ��ȡ���ֽ���
        if (data_size <= 0)
            break;
        auto data = inbuf;
        while (data_size > 0) //һ���ж�֡����
        {
            //ͨ��0001 �ض������AVPacket ����֡��С
            int ret = av_parser_parse2(parser,		//parse������
            	c,						//������������
                &pkt->data, &pkt->size, //���pkt��Ϣ
                data, data_size,        //ÿ�ζ�ȡ��������
                AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0
            );
            data += ret;		//�ӵ�ǰλ�ü������·���
            data_size -= ret; 	//ʣ������

            /*pkt->size ==0 �˴�δ��ȡ������*/
            if (pkt->size)
            {
            
				/*�ѻ�ȡ��ǰpkt����*/
                //����packet�������߳�
                ret = avcodec_send_packet(c, pkt);
                if (ret < 0)		//����ʧ��
                    break;
                //��ȡ��֡��������
                while (ret >= 0)
                {
                    //ÿ�λ����av_frame_unref 
                    ret = avcodec_receive_frame(c, frame);
                    if (ret < 0)	//δ���յ�
                        break;
                    //auto curtime = NowMs();
					count++;
                }
            }
        }
    }
    ///ȡ����������
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
	/*��Դ�ͷ�*/
	av_parser_close(parser);
	avcodec_free_context(&c);
	av_frame_free(&frame);
	av_packet_free(&pkt);

    return 0;
}
