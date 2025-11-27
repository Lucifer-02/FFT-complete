#include <math.h>
#include <stdio.h>

#define PIx2 6.2832

struct cpl { // Create complex datatype

  float img, real;
};

typedef struct cpl complex;

void Gen_sig(float *x, int N) // Generate signals
{
  int n;
  float t, freq1 = 100, freq2 = 50;

  for (n = 0; n < N; n++) {
    t = (float)n * (1 / (float)N);

    x[n] = 3 * sin(freq1 * PIx2 * t) + 5 * sin(freq2 * PIx2 * t);
  }
}

complex c_add(complex a, complex b) // sum of two complex numbers
{
  complex c;
  c.real = a.real + b.real;
  c.img = a.img + b.img;

  return c;
}
complex c_sub(complex a, complex b) // subtract two complex numbers
{
  complex c;
  c.real = a.real - b.real;
  c.img = a.img - b.img;

  return c;
}

complex c_multiple(complex a, complex b) // multiple two complex numbers
{
  complex c;
  c.real = a.real * b.real - a.img * b.img;
  c.img = a.img * b.real + a.real * b.img;

  return c;
}
complex c_divide(complex a,
                 float b) // divide a complex number with a float number
{
  complex c;
  c.real = a.real / b;
  c.img = a.img / b;

  return c;
}

complex c_exp(float a) // compute e^(ia) with a in R
{
  complex c;

  c.real = cos(a);
  c.img = sin(a);

  return c;
}

void FFT(float *x, complex *X,
         int N) // Fast Fourier Transform, return FFT of x signal
{
  if (N == 1)
    return;

  complex X_even[N / 2], X_odd[N / 2], W;
  float x_even[N / 2], x_odd[N / 2];
  int i, j, k;

  // Separate even and odd indexs
  for (i = 0, j = 0; i < N; i += 2) {
    x_even[j] = x[i];
    x_odd[j] = x[i + 1];
    ++j;
  }

  // Minimum case
  if (N == 2) {

    X_even[0].real = x_even[0];
    X_odd[0].real = x_odd[0];
  }

  // Calculate FFT using formula
  FFT(x_even, X_even, N / 2);
  FFT(x_odd, X_odd, N / 2);

  // Pairing together
  for (k = 0; k < N / 2; k++) {
    W = c_exp(-PIx2 * k / N);

    X[k] = c_add(X_even[k], c_multiple(W, X_odd[k]));

    X[k + N / 2] = c_sub(X_even[k], c_multiple(W, X_odd[k]));
  }
}

int main() {
  int N = 256; // ATTENTION!!! number of points, Nyquist Law

  // time and frequency domain data arrays
  float x[N], P[N]; // discrete-time signal, x
  complex X[N];
  int i, k;

  // Perform
  Gen_sig(x, N);
  FFT(x, X, N);

  // Nomarlize the amplitude
  int N_oneside = N / 2;
  complex X_oneside[N / 2];

  for (i = 0; i < N_oneside; ++i) {
    X_oneside[i] = c_divide(X[i], N_oneside);
  }

  // Amplitude of X
  for (k = 0; k < N; k++) {
    P[k] = sqrt(X_oneside[k].img * X_oneside[k].img +
                X_oneside[k].real * X_oneside[k].real);
  }

  // write to .txt file
  FILE *f = fopen("FFT.txt", "w");

  for (i = 0; i < N; ++i) {
    fprintf(f, "%f,", x[i]);
  }
  fprintf(f, "\n");
  for (k = 0; k < N_oneside; k++)
    fprintf(f, "%f,", P[k]);

  fclose(f);

  return 0;
}
