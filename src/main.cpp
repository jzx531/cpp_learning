#include "mcomplex.h"
#include "fft.h"
#include <chrono>
#include <unistd.h>
int main()
{
    Complex a(1, 2);
    a+=Complex(2, 2);
    std::cout << a.abs()<<std::endl;
    uint8_t Nfft = 128;
    FFT fft(Nfft);
    std::vector<Complex> x;
    //以下为数据生成入口
    for (int i = 0; i < Nfft; i++)
    {
        x.push_back(Complex(sin(2 * M_PI * i*2 / Nfft), 0)+Complex(cos(2 * M_PI * i*6 / Nfft), 0));
    }
    
    
     // Start measuring time
    auto start = std::chrono::high_resolution_clock::now();
    fft.calc_dft_mthreads(x);
    // Stop measuring time
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout <<"\n"<< "Time taken by DFT multi-threaded: " << duration.count() << " microseconds" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    fft.calc_dft(x);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout <<"\n"<< "Time taken by DFT: " << duration.count() << " microseconds" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    fft.calc_fft(x);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout <<"\n"<< "Time taken by FFT: " << duration.count() << " microseconds" << std::endl;

    fft.print_abs();
    fft.plot_freq();
    if(execv("./test.png", NULL) == -1){
        perror("execv");
    }
    return 0;
}


