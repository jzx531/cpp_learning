#ifndef FFT_H
#define FFT_H

#include "mcomplex.h"
#include <cmath>
#include <vector>
#include <stdint.h>
#include <mgl2/mgl.h>

#ifndef M_PI
#define M_PI 3.1415926
#endif

class FFT {
private:
    uint8_t Nfft;
    std::vector<Complex> out;
    // std::vector<double> abs_out;
public:
    FFT(uint8_t nfft);
    void calc_fft(std::vector<Complex>& data_in,bool pad=false);
    void calc_dft(std::vector<Complex>& data_in,bool pad=false);
    void calc_dft_mthreads(std::vector<Complex>& data_in,bool pad=false);
    void print();
    void print_abs();
    void plot_freq();
};


#endif

