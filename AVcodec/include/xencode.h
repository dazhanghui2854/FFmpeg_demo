#include <mutex>
#include <vector>
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
class XEncode
{
public:
    /* 创建编码上下文*/

    static AVCodecContext* Create(int codec_id);

    /*加锁 线程安全*/
    void set_c(AVCodecContext* c);


    /*设置编码参数，线程安全*/
    bool SetOpt(const char* key, const char* val);
    bool SetOpt(const char* key, int val);

 	/*打开编码器 线程安全*/
    bool Open();

    AVPacket *Encode(const AVFrame* frame);


    /*根据AVCodecContext 创建一个AVFrame，需要调用者释放av_frame_free*/
    AVFrame* CreateFrame();

    /*返回所有编码缓存中AVPacket*/
    std::vector<AVPacket *> End();

private:
    AVCodecContext* c_ = nullptr;  //编码器上下文
    std::mutex mux_;               //编码器上下文锁
};

