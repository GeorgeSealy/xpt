#ifndef _Noise_h_
#define _Noise_h_

extern double noise(double xin, double yin);
extern double noise(double xin, double yin, double zin);
extern void dNoise(double result[3], double xin, double yin, double zin);

#endif // _Noise_h_
