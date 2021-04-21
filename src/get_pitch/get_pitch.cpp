/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <cstdlib>
#include <math.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN 0.030   /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Detector 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help                   Show this screen
    --version                    Show the version of the project
    -0 FLOAT, --k0=FLOAT         Power threshold [default: 5] 
    -1 FLOAT, --k1=FLOAT         r[1]/r[0] threshold [default: 0.7] 
    -2 FLOAT, --k2=FLOAT         r[lag]/r[0] threshold [default: 0.12]
    -c FLOAT, --clipping=FLOAT   Zero clipping threshold [default: 0.015]
    -o, --offset                 Add zero-clipping offset   
    -m, --method=STRING          Choose between the autocorrelation ("aut") and AMDF ("amdf") methods [default: "amdf"]  
    -w, --window=STRING          Choose between the Hamming window ("hamming") and rectangle window ("rectangle") [default: "hamming"]   
Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the detection:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[])
{
  /// \TODO
  ///  Modify the program syntax and the call to **docopt()** in order to
  ///  add options and arguments to the program.
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
                                                             {argv + 1, argv + argc}, // array of arguments, without the program name
                                                             true,                    // show help if requested
                                                             "2.0");                  // version string

  std::string input_wav = args["<input-wav>"].asString();
  std::string output_txt = args["<output-txt>"].asString();
  float k0 = stof(args["--k0"].asString()); //Power threshold
  float k1 = stof(args["--k1"].asString()); //r[1]/r[0] threshold
  float k2 = stof(args["--k2"].asString()); //r[lag]/r[0] threshold

  float clipping_thr = stof(args["--clipping"].asString()); //Zero clipping threshold
  bool offset = args["--offset"].asBool();                  //Add offset (zero clipping)
  std::string method = args["--method"].asString();         //Computing method
  std::string window = args["--window"].asString();       //Window type
  fprintf(stderr, "%c",method[1]);

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0)
  {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  //Set window and method type
  PitchAnalyzer::Method methodnum= PitchAnalyzer::AMDF;
  PitchAnalyzer::Window windownum = PitchAnalyzer::HAMMING;
  if(strcmp("aut", method.c_str())==0)  
    methodnum=PitchAnalyzer::AUT;

  if(strcmp("rectangle", window.c_str())==0) 
    windownum=PitchAnalyzer::RECT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, k0, k1, k2, methodnum, windownum, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  /// \DONE Central clipping implemented (offset is optional -o)
  for (unsigned int i = 0; i < x.size(); i++)
  {
    if (abs(x[i]) < clipping_thr)
      x[i] = 0;
    else if (offset)
    {
      if (x[i] > 0) //add offset
        x[i] = x[i] - clipping_thr;
      else
        x[i] = x[i] + clipping_thr;
    }
  }
  /*vector<float> num =	{ 0,                                        
-0.00016078372206606997632527544794811547, 
-0.000802890356005967441299642217700238689,
-0.002038933873048945355499483866879018024,
-0.003396830005450876717454367081927557592,
 0.995999994467316351887120617902837693691,
-0.003396830005450876717454367081927557592,
-0.002038933873048945355499483866879018024,
-0.000802890356005967441299642217700238689,
-0.00016078372206606997632527544794811547 ,
 0     }       ; */

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0, f0temp; //f0 temporal
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift)
  {

    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  /// \DONE median filter (size == 3) implemented
  int med_window = (int)3;

  int half_window = round(med_window / 2);
  for (unsigned int i = half_window; i < (f0.size() - half_window); i++)
  {

    for (int j = 0; j < med_window * 2; j++)
    {
      f0temp.push_back(f0[j + i - half_window]);
    }
    sort(f0temp.begin(), f0temp.begin() + med_window);
    f0[i] = f0temp[round(med_window / 2)];
    f0temp.clear();
  }

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good())
  {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX)
    os << *iX << '\n';
  os << 0 << '\n'; //pitch at t=Dur

  return 0;
}
