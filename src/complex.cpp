#include "mcomplex.h"
#include <cmath>
Complex::Complex(double r, double i) : real(r), imag(i) {}
Complex::Complex(const Complex& c) : real(c.real), imag(c.imag) {}
Complex& Complex::operator=(const Complex& c) {
    if (this == &c) return *this;
    real = c.real;
    imag = c.imag;
    return *this;
}

Complex& Complex::operator=(const double& r)
{
    *this = Complex(r, 0);
    return *this;
}

double Complex::abs() const {
    return sqrt(real * real + imag * imag);
}

Complex Complex::operator+(const Complex& c) const {
    return Complex(this->real + c.real, this->imag + c.imag);   
}

Complex& Complex::operator+=(const Complex& c) {
    this->real += c.real;
    this->imag += c.imag;
    return *this;
}

Complex Complex::operator-(const Complex& c) const {
    return Complex(this->real - c.real, this->imag - c.imag);   
}
Complex Complex::operator*(const Complex& c) const {
    return Complex(this->real * c.real - this->imag * c.imag, this->real * c.imag + this->imag * c.real);   
}
Complex Complex::operator/(const Complex& c) const {
    double denominator = c.real * c.real + c.imag * c.imag;
    return Complex((this->real * c.real + this->imag * c.imag) / denominator, (this->imag * c.real - this->real * c.imag) / denominator);   
}

std::ostream& operator<<(std::ostream& out, const Complex& c){
    out << c.real << " + " << c.imag << "i";
    return out;
}
