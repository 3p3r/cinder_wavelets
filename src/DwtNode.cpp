#include "DwtNode.h"
#include "wavelib.h"

namespace {

class ScopedWaveObject
{
public:
    ScopedWaveObject(dsp::MotherWavelet w)
        : mWaveObj(wave_init(const_cast<char*>(dsp::WavelettoString(w).c_str()))) {}

    ~ScopedWaveObject() { wave_free(mWaveObj); }
    wave_object getWaveObject() const { return mWaveObj; }

private:
    wave_object mWaveObj;
};

class ScopedWtObject
{
public:
    ScopedWtObject()                            = delete;
    explicit ScopedWtObject(ScopedWaveObject&& obj, int num_samples, int decomp_levels)
        : mWaveObject(obj), mNumSamples(num_samples), mDecompLevels(decomp_levels)
    {
        mWtObject = wt_init(mWaveObject.getWaveObject(), "dwt", mNumSamples, mDecompLevels);
        setDWTExtension(getWtObject(), "sym");
        setWTConv(getWtObject(), "direct");
    }

    ~ScopedWtObject()   { wt_free(mWtObject); }
    wt_object           getWtObject() const { return mWtObject; }

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
            ScopedWaveObject(fmt.getMotherWavelet()),
            fmt.getWindowSize(),
            fmt.getDecompositionLevels())
    { /* no-op */ }

    ScopedWtObject  mWtObject;
};

DwtNode::DwtNode(const Format &format /*= Format()*/)
    : mCurrentFormat(format)
    , mWavelibData(std::make_unique<Data>(format))
{

}

void DwtNode::initialize()
{
    MonitorNode::initialize();
    mCoefficients.reserve(mCurrentFormat.getWindowSize());
}

}
