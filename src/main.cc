#include <iostream>
#include <emscripten.h>

#include "mfcc.h"
#include "spectrogram.h"

using namespace std;

vector<vector<double> > get_audio_spectrogram(vector<double> input,
                                              int window_length,
                                              int step_length)
{
    tensorflow::Spectrogram sgram;
    vector<vector<double> > output;

    sgram.Initialize(window_length, step_length);
    sgram.ComputeSquaredMagnitudeSpectrogram(input, &output);

    return output;
}

vector<vector<double> > get_audio_mfcc(vector<vector<double> > spectrogram_output,
                                      int sample_rate = 16000,
                                      int upper_frequency_limit = 4000,
                                      int lower_frequency_limit = 20,
                                      int filterbank_channel_count = 40,
                                      int dct_coefficient_count = 13)
{
    tensorflow::Mfcc mfcc;
    vector<vector<double> > mfcc_output(spectrogram_output.size());

    mfcc.set_upper_frequency_limit(upper_frequency_limit);
    mfcc.set_lower_frequency_limit(lower_frequency_limit);
    mfcc.set_filterbank_channel_count(filterbank_channel_count);
    mfcc.set_dct_coefficient_count(dct_coefficient_count);
    mfcc.Initialize(spectrogram_output[0].size(), sample_rate);

    for(size_t i = 0; i < spectrogram_output.size(); ++i)
    {
        vector<double> output;
        mfcc.Compute(spectrogram_output[i], &output);
        mfcc_output[i] = output;
    }

    return mfcc_output;
}

extern "C" {
    /** 
     * @brief Calculate Mel Frequency Cepstral Coefficents (MFCCs) from audio PCM.
     * @param *pcm                      Audio PCM value pointer
     * @param *len                      Buffer length pointer
     * @param sample_rate               Aduio Sample Rate
     * @param window_size               sample_rate * window_size_ms / 1000
     * @param window_stride             sample_rate * window_stride_ms / 1000
     * @param upper_frequency_limit     Default 4000
     * @param lower_frequency_limit     Default 20
     * @param filterbank_channel_count  Default 40
     * @param dct_coefficient_count     MFCC feature extractor should return
     *
     * @return
     *  MFCCs pointer
     */
    double* EMSCRIPTEN_KEEPALIVE tf_mfccs(double *pcm,
                                          int *len,
                                          int sample_rate,
                                          int window_size,
                                          int window_stride,
                                          int upper_frequency_limit,
                                          int lower_frequency_limit,
                                          int filterbank_channel_count,
                                          int dct_coefficient_count)
    {
        vector<double> v_pcm(pcm, pcm + *len);

        vector<vector<double> > spectrogram_output = get_audio_spectrogram(v_pcm, window_size, window_stride);

        vector<vector<double> > mfcc_output = get_audio_mfcc(spectrogram_output, sample_rate,
                                                            upper_frequency_limit, lower_frequency_limit,
                                                            filterbank_channel_count, dct_coefficient_count);

        *len = mfcc_output.size() * mfcc_output[0].size();
        double mfccs[*len];
        int mfccs_index = 0;

        for(int i = 0; i < mfcc_output.size(); ++i)
        {
            for(int j = 0; j < mfcc_output[i].size(); ++j)
            {
                mfccs[mfccs_index++] = mfcc_output[i][j];
            }
        }

        return mfccs;
    }
}
