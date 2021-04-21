/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"
#include <ffft/FFTReal.h>

using namespace std;

/// Name space of Universitat Politecnica de Catalunya
namespace upc
{
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const
  {

    for (unsigned int l = 0; l < r.size(); ++l)
    {
      /// \TODO Compute the autocorrelation r[l]
      /// \DONE Autocorrelation *computed*
      r[l] = 0;
      for (unsigned int n = l; n < x.size(); n++)
      {
        r[l] += x[n] * x[n - l];
      }
      //fprintf(stderr,"%f\n", r[l]);
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero
      r[0] = 1e-10;
  }
  unsigned int PitchAnalyzer::amdf(const vector<float> &x) const
  {

    //for (unsigned int l = 0; l < r.size(); ++l)
    // {
    /// \TODO Compute the autocorrelation r[l]
    /// \DONE Autocorrelation *computed*
    vector<float> a(npitch_max);
    for (unsigned int k = npitch_min; k < npitch_max; k++)
    {
      // fprintf(stderr,"Entra 1r bucle\n");
      a[k] = 0;
      for (unsigned int n = 0; n < x.size() - k - 1; n++)
      {
        //fprintf(stderr,"Entra 2n bucle\n");
        a[k] += abs(x[n] - x[n + k]) * abs(x[n] - x[n + k]);
      }
    }
    //fprintf(stderr,"end\n");

    float min = a[npitch_min];
    int lag = npitch_min;

    for (unsigned int k = npitch_min; k < npitch_max; k++)
    {
      //fprintf(stderr," no minim lag %d min %f a[k] %f\n",lag, min, a[k]);
      if (min > a[k])
      {

        lag = k;
        min = a[k];
        //fprintf(stderr,"lag %d min %f a[k] %f\n",lag, min, a[k]);
      }
    }
    //  fprintf(stderr,"end2\n");
    if (lag == 40)
      lag = 1;
    return lag;
    //fprintf(stderr,"%f\n", r[l]);
  }

  void PitchAnalyzer::set_window(Window win_type)
  {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type)
    {
    case HAMMING:
      /// \TODO Implement the Hamming window
      /// \DONE Hamming window implemented
      // float alpha = 0.53836;
      for (unsigned int i = 0; i < frameLen; i++)
      {
        window[i] = 0.6 + (1 - 0.6) * cos(((2 * M_PI) / frameLen) * i); //obtain i sample of hamming window
      }
      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0)
  {

    npitch_min = (unsigned int)samplingFreq / max_F0;

    if (npitch_min < 2)
      npitch_min = 2; // samplingFreq/2

    npitch_max = 1 + (unsigned int)samplingFreq / min_F0;
    //frameLen should include at least 2*T0
    if (npitch_max > frameLen / 2)
      npitch_max = frameLen / 2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const
  {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.
    //fprintf(stderr, "%f\n", r1norm);
    //&&pot<k0

    if ((r1norm < k1 || rmaxnorm < k2)&&pot<k0)
      return true;
    else
      return false;
  }

  float PitchAnalyzer::compute_pitch(vector<float> &x) const
  {
    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i = 0; i < x.size(); i++)
      x[i] *= window[i];

    vector<float> r(npitch_max);
    //Compute correlation
    autocorrelation(x, r);
    /*int N = 1024;
  float X[N], xtf[N];
  float eps = 1e-20;
  ffft::FFTReal <float> fft_object (N);
  //Zero padding
  for (int i = 0; i< N; i++) {
    if(i>=x.size())
    xtf[i] = 0;
    else
    xtf[i]=x[i];
  }
   for (int i =0; i< N; i++)
    //fprintf(stderr,"x[%d] )= %f\n", i,xtf[i]);
 // fprintf(stderr,"----------------------------\n");

  fft_object.do_fft (X, xtf);     // x (real) --FFT---> f (complex)
  for(int i = 0; i<N; i++)
     X[i] = 10*log10(abs(X[i])+eps);

  fft_object.do_ifft (X, xtf);    // f (complex) --IFFT--> x (real)
  fft_object.rescale (xtf);       // Post-scaling should be done after FFT+IFFT

  for (int i = 0; i< x.size(); i++) {
    x[i]=xtf[i];
  }

    vector<float>::const_iterator iR = r.begin(), iRMax = iR + npitch_min;

    /// \TODO
    /// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
    /// Choices to set the minimum value of the lag are:
    ///    - The first negative value of the autocorrelation.
    ///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
    /// In either case, the lag should not exceed that of the minimum value of the pitch.
    /* float max = 0;
    float abs_max = 0;
    int i = 1, pos, posini;*/

    /*  for (iR = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++)
    {
      if (*iR > *iRMax)
      {
        iRMax = iR;
      }
    }
    unsigned int lag = iRMax - r.begin();
    

   // 
 for (iR = x.begin() + npitch_min; iR < x.begin() + npitch_max; iR++)
    {
      if (*iR > *iRMax)
      {
        iRMax = iR;
      }
    }
    unsigned int lag = iRMax - x.begin();*/
    //fprintf(stderr, "lag %d\n", lag);
    unsigned int lag = amdf(x);
    float pot = 10 * log10(r[0]);

    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
#if 1
    if (r[0] > 0.0F)
      //cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << endl;
      cout << r[0] << endl;
    for (int i = 1; i < r.size(); i++)
      cout << r[i] << endl;
#endif

    if ((unvoiced(pot, r[1] / r[0], r[lag] / r[0])) || lag == 1)
      return 0;
    else
      return (float)samplingFreq / (float)lag;
  }
}
