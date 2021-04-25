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
      //Quit loop if amdf has been selected and r[0] and r[1] have been computed
      if ((method == PitchAnalyzer::SDF || method == PitchAnalyzer::AMDF) && (l > 1))
        break;
      /// \TODO Compute the autocorrelation r[l]
      /// \DONE Autocorrelation *computed*

      r[l] = 0;
      for (unsigned int n = l; n < x.size(); n++)
      {
        r[l] += x[n] * x[n - l];
      }
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero
      r[0] = 1e-10;
  }
  unsigned int PitchAnalyzer::amdf(const vector<float> &x) const
  {

    vector<float> a(npitch_max);
    float min = -1;
    int lag = npitch_min;
    float m_k, r_k;
    for (unsigned int k = npitch_min; k < npitch_max; k++)
    {
      a[k] = 0;
      for (unsigned int n = 0; n < x.size() - k - 1; n++)
      {
        if (a[k] > min && min != -1) //quit loop if it's bigger than the current minima and minima has been set
          continue;

        if (method == PitchAnalyzer::AMDF)
          a[k] += abs(x[n] - x[n + k]);
        else
        {
          //SDF is the squared AMDF
          //a[k] += (x[n] - x[n + k]) * (x[n] - x[n + k]) =  x[n] * x[n] + x[n + k] * x[n + k] -2*(x[n] * x[n + k]) = m_k -2r_k
          m_k = x[n] * x[n] + x[n + k] * x[n + k];
          r_k = x[n] * x[n + k];
          a[k] += m_k - 2 * r_k;
        }
      }
      if (min == -1)
      {
        min = a[k];
      }
      else if (min > a[k])
      {
        min = a[k];
        lag = k;
      }
    }

    //If pitch hasn't found we'll consider it unvoiced
    if ((unsigned int)lag == npitch_min)
      lag = 1;
    return lag;
  }

  void PitchAnalyzer::set_window(Window win_type, float alpha)
  {
    if (frameLen == 0)
      return;
    window.resize(frameLen);

    switch (win_type)
    {
    case HAMMING:
      /// \TODO Implement the Hamming window
      /// \DONE Hamming window implemented

      for (unsigned int i = 0; i < frameLen; i++)
      {
        window[i] = alpha + (1 - alpha) * cos(((2 * M_PI) / frameLen) * i); //obtain i sample of hamming window
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

    if (((rmaxnorm < k2 || r1norm < k1 || pot < k0))) 
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

    vector<float>::const_iterator iR = r.begin(), iRMax = iR + npitch_min;
    /// \TODO
    /// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
    /// Choices to set the minimum value of the lag are:
    ///    - The first negative value of the autocorrelation.
    ///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
    /// In either case, the lag should not exceed that of the minimum value of the pitch.
    /// \DONE. Autocorrelation + amdf implemented, method can be chosen with -m
    unsigned int lag;
    switch (method)
    {
    case AUT: //Autocorrelation method

      for (iR = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++)
      {
        if (*iR > *iRMax)
        {
          iRMax = iR;
        }
      }
      lag = iRMax - r.begin();
      break;
    case SDF: //amdf method
    default:
      lag = amdf(x);
      //obtain r[lag]
      r[lag] = 0;
      for (unsigned int n = lag; n < x.size(); n++)
        r[lag] += x[n] * x[n - lag];
      break;
    }

    float pot = 10 * log10(r[0]);

    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
#if 0
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1] / r[0] << '\t' << r[lag] / r[0] << endl;

#endif

    if ((unvoiced(pot, r[1] / r[0], r[lag] / r[0])) || lag == 1)
      return 0;
    else
      return (float)samplingFreq / (float)lag;
  }
}
