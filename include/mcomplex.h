#ifndef COMPLEX_H
#define COMPLEX_H
#include <iostream>

class Complex {
private:

    double real;
    double imag;
public:

    Complex(double r = 0, double i = 0);
    Complex(const Complex& c);
    double abs() const;
    Complex& operator=(const Complex& c);
    Complex& operator=(const double& r);
    // ~Complex();
    Complex operator+(const Complex& c) const ;
    Complex& operator+= (const Complex& c) ;
    Complex operator-(const Complex& c) const ;
    Complex operator*(const Complex& c) const ;
    Complex operator/(const Complex& c) const ;
    friend std::ostream& operator<<(std::ostream& out, const Complex& c);
};





#endif // COMPLEX_H

