#include "fft.h"
#include <iterator>
#include <algorithm>
// static const double M_PI = acos(-1);
#include <pthread.h>
#include <unistd.h>

static  bool examine_mod2(uint8_t Nfft){
    return !(Nfft & (Nfft - 1));
}

FFT::FFT(uint8_t nfft) {
    if(examine_mod2(nfft))Nfft = nfft;
    else throw std::runtime_error("FFT: Nfft must be power of 2");
}

static uint8_t bitreorder(uint8_t x, uint8_t l) {
    uint8_t y=x;
    for (int i = 0; i < l/2; i++)
    {
        uint8_t t = x & (1 << i);
        t = t << (l-2*i-1);
        uint8_t s = x & (1 << (l-i-1));
        s = s >> (l-2*i-1);
        // std::cout<<"t="<<(int)t<<" s="<<(int)s<<std::endl;
        uint8_t z = ~((1<<i)| (1<<(l-i-1)));
        // std::cout<<"z="<<(int)z<<std::endl;
        y = y & z | t |s;
    }
    return y;
}

void FFT::calc_fft(std::vector<Complex>& data_in,bool pad) {
    if(data_in.size() < Nfft) {
        if(pad){
            data_in.resize(Nfft,Complex(0,0));
        }
        else {
            throw std::runtime_error("FFT input size must be  equal to Nfft");
        }
    }
    /*开始蝶形计算*/
    uint8_t l = (uint8_t)log2f(Nfft);
    std::cout<<"l="<<(int)l<<std::endl;
    out.resize(data_in.size());  // Resize out to match data_in size

    /*调整顺序*/
    for(uint8_t i = 0; i < Nfft; i++){
        out.at(i) = data_in.at(bitreorder(i,l));
        // std::cout<<(int)i<<" "<<(int)bitreorder(i,l)<<std::endl;
    }
    // uint8_t k = 2;
    // std::cout<<"bitreorder = "<<(int)(bitreorder(k,l))<<std::endl;
    // copy(out.begin(), out.end(), std::ostream_iterator<Complex>(std::cout, "\n"));
    for(uint8_t i = 1; i <= l; i++)
    {
        for(uint8_t k = 1; k <= Nfft; k += pow(2, i))
        {
            for (size_t j = 0; j < pow(2, i-1); j++)
            {
                // std::cout<<"i="<<(int)i<<" k="<<(int)k<<" j="<<(int)j<<std::endl;
                int idx1 = k + j - 1;
                int idx2 = k + j - 1 + pow(2, i-1);
                Complex w(cos(-2 * M_PI * j / pow(2, i)), -sin(2 * M_PI * j / pow(2, i)));
                Complex temp = w * out[idx2];
                out[idx2] = out[idx1] - temp;
                out[idx1] = out[idx1] + temp;
            }
        }
    }
}

void FFT::calc_dft(std::vector<Complex>& data_in,bool pad) {
    if(data_in.size() < Nfft) {
        if(pad){
            data_in.resize(Nfft,Complex(0,0));
        }
        else {
            throw std::runtime_error("FFT input size must be  equal to Nfft");
        }
    }
    /*开始正弦余弦计算*/
    for (uint8_t k = 0; k < Nfft; k++)
    {
        Complex sum(0, 0);
        for(uint8_t n = 0; n < Nfft; n++)
        {
            sum += data_in[n] * Complex(-cos(2 * M_PI * k * n / Nfft), -sin(2 * M_PI * k * n / Nfft));
        }
        out.push_back(sum);
    }
}

struct thread_arg{
    std::vector<Complex>* data_in;
    uint8_t k;
    Complex* outk;
    uint8_t Nfft;

     // Constructor to initialize the references
     thread_arg(std::vector<Complex>* data_in, Complex* outk, uint8_t k, uint8_t Nfft)
     : data_in(data_in), outk(outk), k(k), Nfft(Nfft) {}
};

static void* dft_once(void* arg)
{
    thread_arg* t_arg = (thread_arg*)arg;
    // std::cout<<"thread "<<int(t_arg->k)<<" start"<<std::endl;
    for(uint8_t n = 0; n < t_arg->Nfft; n++)
    {
       *(t_arg->outk) += (*(t_arg->data_in))[n] * Complex(-cos(2 * M_PI * t_arg->k * n / t_arg->Nfft), -sin(2 * M_PI * t_arg->k * n / t_arg->Nfft));
    }
    return NULL;
}

void FFT::calc_dft_mthreads(std::vector<Complex>& data_in, bool pad) {
    if (data_in.size() < Nfft) {
        if (pad) {
            data_in.resize(Nfft, Complex(0, 0));
        } else {
            throw std::runtime_error("FFT input size must be equal to Nfft");
        }
    }

    // Multi-threaded computation
    out.resize(Nfft, Complex(0, 0));

    std::vector<pthread_t> threads(Nfft);  // To store thread IDs
    const int num_threads = Nfft;  // Number of threads to create
    std::vector<thread_arg*> t_arg(num_threads);
    for (uint8_t i = 0; i < Nfft; i++) {
        // Create a new thread_arg object for each thread
        
        t_arg[i] = new thread_arg(&data_in, &out[i], i, this->Nfft);
        // Create the thread
        if (pthread_create(&threads[i], NULL, dft_once, t_arg[i]) != 0) {
            perror("pthread_create:");
            exit(-1);
        }
    }

    // Join all threads to ensure they complete
    for (uint8_t i = 0; i < Nfft; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join:");
            exit(-1);
        }
    }
    std::cout << "All threads completed." << std::endl;
    std::for_each(t_arg.begin(), t_arg.end(), [](thread_arg* t) { delete t; });
}


void FFT::print() {
    copy(out.begin(), out.end(), std::ostream_iterator<Complex>(std::cout, "\n"));
}

void FFT::print_abs() {
    std::for_each(out.begin(), out.end(), [](Complex& c) { std::cout << c.abs() << " "; });
}

void FFT::plot_freq(){
    //创建gr对象，指定图像大小为800x500,kind=0说明不使用OpenGL
    mglGraph gr(0,800,500);
    
    //数据准备
    std::vector<double> datavec1;
    std::vector<double> datavec2;

    for(uint8_t i = 0; i < Nfft; i++){
        datavec1.push_back(i);
        datavec2.push_back(out[i].abs());
    }

    mglData x(datavec1);
    mglData y(datavec2);

    //通过给定数据计算合适的显示范围
    auto minx = *std::min_element(datavec1.begin(), datavec1.end());
    auto miny = *std::min_element(datavec2.begin(), datavec2.end());
    auto maxx = *std::max_element(datavec1.begin(), datavec1.end());
    auto maxy = *std::max_element(datavec2.begin(), datavec2.end());
    auto x_margin = (maxx - minx) / datavec1.size();
    auto y_margin = (maxy - miny) / datavec2.size();
    
    gr.SetRanges(minx - x_margin, maxx + x_margin, miny - y_margin, maxy + y_margin);
    //显示坐标轴
    gr.Axis();
    //显示图片框
    gr.Box();
    //Plot、Barh、Mark等等可选
    gr.Plot(x, y, "y+");


    //保存图片
    std::cout << "write image as \"test.png\"." << std::endl;
    gr.WritePNG("test.png");  // Don't forget to save the result!
}