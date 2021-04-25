/// @file

#ifndef PITCH_ANALYZER_H
#define PITCH_ANALYZER_H

#include <vector>
#include <algorithm>

namespace upc
{
  const float MIN_F0 = 20.0F;    ///< Minimum value of pitch in Hertzs
  const float MAX_F0 = 10000.0F; ///< Maximum value of pitch in Hertzs

  ///
  /// PitchAnalyzer: class that computes the pitch (in Hz) from a signal frame.
  /// No pre-processing or post-processing has been included
  ///
  class PitchAnalyzer
  {
  public:
    /// Wndow type
    enum Window
    {
      RECT,   ///< Rectangular window
      HAMMING ///< Hamming window
    };

    void set_window(Window type, float alpha); ///< pre-compute window

    /// Pitch retrieval method
    enum Method
    {
      AUT,   ///< Autocorrelation method
      AMDF,  ///< AMDF method
      SDF    ///< SDF method
    };

  private:
    std::vector<float> window; ///< precomputed window
    unsigned int frameLen,     ///< length of frame (in samples). Has to be set in the constructor call
        samplingFreq,          ///< sampling rate (in samples per second). Has to be set in the constructor call
        npitch_min,            ///< minimum value of pitch period, in samples
        npitch_max;            ///< maximum value of pitch period, in samples
    Method method;             ///< Computation method (==0 autocorrelation, ==1 SDF)          
    float k0;                  ///< Power threshold inputted by the user
    float k1;                  ///< r[1]/r[0] threshold
    float k2;                  ///< r[lag]/r[0] threshold
    ///
    /// Computes correlation from lag=0 to r.size()
    ///
    void autocorrelation(const std::vector<float> &x, std::vector<float> &r) const;

    ///
    /// Obtains lag by using the AMDF or SDF method
    ///
    unsigned int amdf(const std::vector<float> &x) const;
    ///
    /// Returns the pitch (in Hz) of input frame x
    ///
    float compute_pitch(std::vector<float> &x) const;

    ///
    /// Returns true is the frame is unvoiced
    ///
    bool unvoiced(float pot, float r1norm, float rmaxnorm) const;

  public:
    PitchAnalyzer(unsigned int fLen,  ///< Frame length in samples
                  unsigned int sFreq, ///< Sampling rate in Hertzs
                  float k0thresh,     ///< Power threshold
                  float k1thresh,     ///< r[1]/r[0] threshold
                  float k2thresh,     ///< r[lag]/r[0] threshold
                  Method m,           ///< Computation method (autocorrelation, AMDF, SDF)
                  float alpha = 0.54, ///< Hamming window's alpha (==0.54 for the standard Hamming window)
                  Window w = PitchAnalyzer::HAMMING, ///< Window type
                  float min_F0 = MIN_F0,             ///< Pitch range should be restricted to be above this value
                  float max_F0 = MAX_F0              ///< Pitch range should be restricted to be below this value
    )
    {

      frameLen = fLen;
      samplingFreq = sFreq;
      k0 = k0thresh;
      k1 = k1thresh;
      k2 = k2thresh;
      method = m;
      set_f0_range(min_F0, max_F0);

      set_window(w, alpha);
    }

    ///
    /// Operator (): computes the pitch for the given vector x
    ///
    float operator()(const std::vector<float> &_x) const
    {
      if (_x.size() != frameLen)
        return -1.0F;

      std::vector<float> x(_x); //local copy of input frame
      return compute_pitch(x);
    }

    ///
    /// Operator (): computes the pitch for the given "C" vector (float *).
    /// N is the size of the vector pointer by pt.
    ///
    float operator()(const float *pt, unsigned int N) const
    {
      if (N != frameLen)
        return -1.0F;

      std::vector<float> x(N);          //local copy of input frame, size N
      std::copy(pt, pt + N, x.begin()); ///copy input values into local vector x
      return compute_pitch(x);
    }

    ///
    /// Operator (): computes the pitch for the given vector, expressed by the begin and end iterators
    ///
    float operator()(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end) const
    {

      if (end - begin != frameLen)
        return -1.0F;

      std::vector<float> x(end - begin); //local copy of input frame, size N
      std::copy(begin, end, x.begin());  //copy input values into local vector x
      return compute_pitch(x);
    }

    ///
    /// Sets pitch range: takes min_F0 and max_F0 in Hz, sets npitch_min and npitch_max in samples
    ///
    void set_f0_range(float min_F0, float max_F0);
  };
}
#endif
