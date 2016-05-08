#pragma once

#include "cinder/audio/MonitorNode.h"
#include "DwtDsp.h"

namespace wavy
{

class DwtNode : public ci::audio::MonitorNode {
public:
    struct Format : public MonitorNode::Format {
        Format() : mDecompositionLevels(3), mMotherWavelet(dsp::MotherWavelet::Haar) {}

        Format&             decompositionLevels(std::size_t size)   { mDecompositionLevels = size; return *this; }
        Format&             motherWavelet(dsp::MotherWavelet wav)   { mMotherWavelet = wav; return *this; }

        size_t			    getDecompositionLevels() const			{ return mDecompositionLevels; }
        dsp::MotherWavelet  getMotherWavelet() const			    { return mMotherWavelet; }

    protected:
        std::size_t	        mDecompositionLevels;
        dsp::MotherWavelet  mMotherWavelet;
    };

    DwtNode(const Format &format = Format());
    virtual ~DwtNode() { /* no-op */ }

protected:
    void initialize() override;

private:
    struct Data;
    std::unique_ptr<Data>       mWavelibData;
    ci::audio::Buffer		    mInBuffer;  // samples before transform
    ci::audio::Buffer			mOutBuffer; // transformed samples
    std::vector<float>			mCoefficients;
    Format                      mCurrentFormat;
};

}
