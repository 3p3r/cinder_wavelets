#include "DwtNode.h"
#include "wavelib.h"

namespace {

class ScopedWaveObject
{
public:
    ScopedWaveObject(dsp::MotherWavelet w)
        : mWaveObj(::wave_init(const_cast<char*>(dsp::WavelettoString(w).c_str()))) {}

    ~ScopedWaveObject() { ::wave_free(mWaveObj); }
    wave_object         getWaveObject() const { return mWaveObj; }

private:
    wave_object mWaveObj;
};

class ScopedWtObject
{
public:
    ScopedWtObject()                    = delete;
    explicit ScopedWtObject(dsp::MotherWavelet wavelet, int num_samples, int decomp_levels)
        : mWaveObject(wavelet), mNumSamples(num_samples), mDecompLevels(decomp_levels)
    {
        mWtObject = ::wt_init(mWaveObject.getWaveObject(), "dwt", mNumSamples, mDecompLevels);
        ::setDWTExtension(getWtObject(), "sym");
        ::setWTConv(getWtObject(), "direct");
    }

    ~ScopedWtObject()   { wt_free(mWtObject); }
    wt_object           getWtObject() const { return mWtObject; }
    wave_object         getWaveObject() const { return mWaveObject.getWaveObject(); }

private:
    ScopedWaveObject    mWaveObject;
    wt_object           mWtObject;
    int                 mNumSamples;
    int                 mDecompLevels;
};

}

namespace wavy
{

struct DwtNode::Data
{
    Data()              = delete;
    Data(const Data&)   = delete;
    explicit Data(const DwtNode::Format& fmt)
        : mWtObject(
            fmt.getMotherWavelet(),
            fmt.getWindowSize(),
            fmt.getDecompositionLevels())
    { /* no-op */ }

    ScopedWtObject      mWtObject;
};

DwtNode::DwtNode(const Format &format /*= Format()*/)
    : mCurrentFormat(format)
    , mWavelibData(std::make_unique<Data>(format))
{}

void DwtNode::initialize()
{
    MonitorNode::initialize();
    mSamplesBuffer = ci::audio::BufferT<double>(mCurrentFormat.getWindowSize());
}

const std::vector<float>& DwtNode::getCoefficients()
{
    fillCopiedBuffer();
    mSamplesBuffer.zero();

    double scale = 1.0f / getNumChannels();
    for (std::size_t ch = 0; ch < getNumChannels(); ch++) {
        for (std::size_t i = 0; i < mWindowSize; i++)
            mSamplesBuffer[i] += mCopiedBuffer.getChannel(ch)[i] * scale;
    }

    ::dwt(mWavelibData->mWtObject.getWtObject(), mSamplesBuffer.getData());

    if (mCoefficients.size() != mWavelibData->mWtObject.getWtObject()->outlength)
        mCoefficients.resize(mWavelibData->mWtObject.getWtObject()->outlength);

    for (std::size_t i = 0; i < mCoefficients.size(); i++)
        mCoefficients[i] = static_cast<float>(mWavelibData->mWtObject.getWtObject()->output[i]);

    return mCoefficients;
}

}
