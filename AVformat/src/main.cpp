#include <iostream>
#include <thread>
extern "C" 
{
//引用ffmpeg头文件
#include "libavformat/avformat.h"
}

using namespace std;

#define CERR(err) if(err!=0){ PrintErr(err);getchar();return -1;}


void PrintErr(int err)
{
	char buf[1024] = { 0 };
	av_strerror(err, buf, sizeof(buf) - 1);
	cerr << endl;
}


int main(int argc, char* argv[])
{
	//打开媒体文件
	//const char* url = "zhanghui.dav" ;
	//const char* url = "zhanghui.avi" ;
	const char* url = "zhanghui.mp4" ;
	////////////////////////////////////////////////////////////////////////////////////
	/// 解封装
	//解封装输入上下文
	AVFormatContext* ic = nullptr;
	auto re = avformat_open_input(&ic, url,
	    NULL,       //封装器格式 null 自动探测 根据后缀名或者文件头
	    NULL        //参数设置，rtsp需要设置
	);
	CERR(re);
	//获取媒体信息 无头部格式
	re = avformat_find_stream_info(ic, NULL);
	CERR(re);

	//打印封装信息
	av_dump_format(ic, 0, url,
	    0 //0表示上下文是输入 1 输出
	);
	AVStream* as = nullptr; //音频流
	AVStream* vs = nullptr; //视频流
	/*遍历获取流信息*/
	for (int i = 0; i < ic->nb_streams; i++)
	{
		//音频
		if (ic->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			as = ic->streams[i];
			cout << "=====Audio=====" << endl;
			cout << "sample_rate:" << as->codecpar->sample_rate << endl;
		}
		else if (ic->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			vs = ic->streams[i];
			cout << "=========Video=========" << endl;
			cout << "width:" << vs->codecpar->width << endl;
			cout << "height:" << vs->codecpar->height << endl;
	    }
	}



	////////////////////////////////////////////////////////////////////////////////////
	/// 解封装
	//编码器上下文
	const char* out_url = "test.mp4";
	//封装上下文
	AVFormatContext* ec = nullptr;
	//初始化一个用于输出的AVFormatContext结构体
	re = avformat_alloc_output_context2(&ec, NULL, NULL,
	    out_url         //根据文件名推测封装格式
	);
	CERR(re);
	//添加视频流、音频流两路流
	auto mvs = avformat_new_stream(ec, NULL);  //视频流
	auto mas = avformat_new_stream(ec, NULL);  //音频流

	//打开输出IO
	re = avio_open(&ec->pb, out_url, AVIO_FLAG_WRITE);
	CERR(re);


	//设置编码音视频流参数
	//ec->streams[0];
	//mvs->codecpar;//视频参数
	if (vs)
	{
		//1.时间基数
		mvs->time_base = vs->time_base;// 时间基数与原视频一致
		//2.封装复制参数
		avcodec_parameters_copy(mvs->codecpar, vs->codecpar);
	}

	if (as)
	{
	    mas->time_base = as->time_base;
	    //从解封装复制参数
	    avcodec_parameters_copy(mas->codecpar, as->codecpar);
	}
	//写入文件头
	re = avformat_write_header(ec, NULL);
	CERR(re);
	//打印输出上下文
	av_dump_format(ec, 0, out_url, 1);	



	// 截取10 ~ 20 秒之间的音频视频
	double begin_sec = 10.0;    //截取开始时间
	double end_sec = 20.0;      //截取结束时间
	long long begin_pts = 0;	//视频开始
	long long begin_audio_pts = 0;  //音频的开始pts
	long long end_pts = 0;
    //换算成pts 换算成输入ic的pts，以视频流为准
    if (vs && vs->time_base.num > 0)
    {
        double t = (double)vs->time_base.den / (double)vs->time_base.num;//den分母/num分子
        begin_pts = begin_sec * t;
        end_pts = end_sec * t;
    }
	//音频时间基数
    if (as&&as->time_base.num>0)
        begin_audio_pts = begin_sec * ((double)as->time_base.den / (double)as->time_base.num);	//音频开始的pts

    //seek输入媒体 移动到第十秒的关键帧位置
    if(vs)
        re = av_seek_frame(ic, vs->index, begin_pts, //对Index索引流跳转到对应PTS位置
            AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD); //向后关键帧
    CERR(re);

    AVPacket pkt;
    for (;;)
    {
    	/*读取一帧*/
        re = av_read_frame(ic, &pkt);
        if (re != 0)
        {
            PrintErr(re);
            break;
        }
        AVStream* in_stream = ic->streams[pkt.stream_index];
        AVStream* out_stream = nullptr;
        long long offset_pts = 0; //偏移pts，用于截断的开头pts运算
        
        if (vs && pkt.stream_index == vs->index)		//视频帧
        {
            cout << "视频:";

            //超过第20秒退出，只存10~20秒
            if (pkt.pts > end_pts)
            {
            	//资源一定要清理
                av_packet_unref(&pkt);
                break;
            }
            out_stream = ec->streams[0];		//存入视频流
            offset_pts = begin_pts;
        }
        else if (as && pkt.stream_index == as->index)	//音频帧
        {
            cout << "音频:";
            out_stream = ec->streams[1];		//存入音频流
            offset_pts = begin_audio_pts; 
        }
        cout << pkt.pts << " : " << pkt.dts << " :" << pkt.size << endl;

        //重新计算pts dts duration
        //`a * bq（输入basetime） / cq（输出basetime）`
        if (out_stream)
        {
            pkt.pts = av_rescale_q_rnd(pkt.pts- offset_pts, in_stream->time_base,
                out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
            );
            pkt.dts = av_rescale_q_rnd(pkt.dts- offset_pts, in_stream->time_base,
                out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
            );
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        }
        pkt.pos = -1;


        //写入音视频帧清理pkt
        re = av_interleaved_write_frame(ec,
            &pkt);
        if (re != 0)
        {
            PrintErr(re);
        }
        //av_packet_unref(&pkt);
        //this_thread::sleep_for(100ms);
    }

	//写入结尾 包含文件偏移索引
	re = av_write_trailer(ec);
	if (re != 0)PrintErr(re);

	avformat_close_input(&ic);
	/*io关闭必须在free之前*/
	avio_closep(&ec->pb);
	avformat_free_context(ec);
	ec = nullptr;
	return 0;
}

