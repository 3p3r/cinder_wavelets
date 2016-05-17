#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/audio/InputNode.h"
#include "cinder/audio/MonitorNode.h"

#include "DftNode.h"
#include "DwtNode.h"
#include "AudioDrawUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderScalogramApp : public App {
public:
    void                        setup() override;
    void                        draw() override;
    void                        resize() override;

private:
    audio::InputDeviceNodeRef	mInputDeviceNode;
    audio::MonitorNodeRef       mMonitorNode;
    wavy::DftNodeRef	        mDftNode;
    wavy::DwtNodeRef	        mDwtNode;
    SpectrumPlot				mDftPlot;
    WaveletDecompositionPlot    mDwtPlot;
    float                       mDrawMargin = 40.0f;
    int                         mSampleSize = 512;
};

void CinderScalogramApp::setup()
{
    auto ctx = audio::Context::master();

    mInputDeviceNode = ctx->createInputDeviceNode();

    auto dftFormat = wavy::DftNode::Format()
        .fftSize(mSampleSize * 2)
        .windowSize(mSampleSize);

    mDftNode = ctx->makeNode(new wavy::DftNode(dftFormat));

    auto dwtFormat = wavy::DwtNode::Format()
        .decompositionLevels(5)
        .motherWavelet(dsp::MotherWavelet::Daubechies5)
        .windowSize(mSampleSize);

    mDwtNode = ctx->makeNode(new wavy::DwtNode(dwtFormat));

    auto monitorFormat = audio::MonitorNode::Format()
        .windowSize(mSampleSize);

    mMonitorNode = ctx->makeNode(new audio::MonitorNode(monitorFormat));

    mInputDeviceNode >> mDftNode;
    mInputDeviceNode >> mDwtNode;
    mInputDeviceNode >> mMonitorNode;

    mInputDeviceNode->enable();
    ctx->enable();

    mDwtPlot = WaveletDecompositionPlot(mDwtNode);
}

void CinderScalogramApp::resize()
{
    mDftPlot.setBounds(Rectf(mDrawMargin, mDrawMargin, getWindowWidth() - mDrawMargin, (getWindowHeight() - mDrawMargin) * 0.5f));
    mDwtPlot.setBounds(Rectf(mDrawMargin, (getWindowHeight() + mDrawMargin) * 0.5f, getWindowWidth() - mDrawMargin, getWindowHeight() - mDrawMargin));
}

void CinderScalogramApp::draw()
{
    gl::clear(Color::black());

    mDftPlot.draw(mDftNode->getMagSpectrum());
    mDwtPlot.draw();
}

CINDER_APP(CinderScalogramApp, RendererGl)
