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

class CinderWaveletsApp : public App {
public:
    void                        setup() override;
    void                        draw() override;
    void                        resize() override;
    void                        keyDown( KeyEvent event ) override;

private:
    audio::InputDeviceNodeRef	mInputDeviceNode;
    wavy::DftNodeRef	        mDftNode;
    wavy::DwtNodeRef	        mDwtNode;
    SpectrumPlot				mDftPlot;
    WaveletDecompositionPlot    mDwtPlot;
    gl::FboRef                  mPausedScreen;
    int                         mSampleSize = 512;
    bool                        mPaused     = false;
};

void CinderWaveletsApp::setup()
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

    mInputDeviceNode >> mDftNode;
    mInputDeviceNode >> mDwtNode;

    mInputDeviceNode->enable();
    ctx->enable();

    mDwtPlot = WaveletDecompositionPlot(mDwtNode);

    std::string title = dsp::WavelettoString(dwtFormat.getMotherWavelet()) + " wavelet decompositions vs. FFT bins";
    getWindow()->setTitle(title);
}

void CinderWaveletsApp::resize()
{
    float margin_percent = 0.07f;
    float draw_margin_w = margin_percent * getWindowWidth();
    float draw_margin_h = margin_percent * getWindowHeight();

    mDftPlot.setBounds(Rectf(draw_margin_w, draw_margin_h, getWindowWidth() - draw_margin_w, (getWindowHeight() - draw_margin_h) * 0.5f));
    mDwtPlot.setBounds(Rectf(draw_margin_w, (getWindowHeight() + draw_margin_h) * 0.5f, getWindowWidth() - draw_margin_w, getWindowHeight() - draw_margin_h));
}

void CinderWaveletsApp::keyDown(KeyEvent event)
{
    if (event.getCode() == event.KEY_ESCAPE)
        quit();
    else if (event.getCode() == event.KEY_p)
        mPaused = !mPaused;

    if (mPaused)
    {
        mPausedScreen = gl::Fbo::create(getWindowWidth(), getWindowHeight());
        mPausedScreen->blitFromScreen(getWindowBounds(), mPausedScreen->getBounds());
    }
}

void CinderWaveletsApp::draw()
{
    gl::clear(Color::black());

    if (mPaused && mPausedScreen)
    {
        mPausedScreen->blitToScreen(mPausedScreen->getBounds(), getWindowBounds());
    }
    else
    {
        mDftPlot.draw(mDftNode->getMagSpectrum());
        mDwtPlot.draw();
    }
}

CINDER_APP(CinderWaveletsApp, RendererGl)
