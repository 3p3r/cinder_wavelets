#pragma once

#include "cinder/audio/MonitorNode.h"
#include "DwtDsp.h"

namespace wavy
{

using DwtNodeRef = std::shared_ptr<class DwtNode>;

class DwtNode : public ci::audio::MonitorNode {
public:
    struct Format : public MonitorNode::Format {
        Format() : mDecompositionLevels(3), mMotherWavelet(dsp::MotherWavelet::Haar) {}

        Format&                 decompositionLevels(std::size_t size)   { mDecompositionLevels = size; return *this; }
        Format&                 motherWavelet(dsp::MotherWavelet wav)   { mMotherWavelet = wav; return *this; }
        Format&		            windowSize(std::size_t size)			{ MonitorNode::Format::windowSize(size); return *this; }

        size_t			        getDecompositionLevels() const			{ return mDecompositionLevels; }
        dsp::MotherWavelet      getMotherWavelet() const			    { return mMotherWavelet; }

    protected:
        std::size_t	            mDecompositionLevels;
        dsp::MotherWavelet      mMotherWavelet;
    };

    DwtNode(const Format &format = Format());
    virtual ~DwtNode() { /* no-op */ }

protected:
    void                        initialize() override;

public:
    const std::vector<float>&   getCoefficients();

private:
    struct Data;
    std::shared_ptr<Data>       mWavelibData;
    ci::audio::BufferT<double>  mSamplesBuffer;
    std::vector<float>		    mCoefficients;
    Format                      mCurrentFormat;
};

}
