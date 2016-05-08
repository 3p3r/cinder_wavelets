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
    void setup() override;
    void update() override;
    void draw() override;
    void resize() override;

private:
    audio::InputDeviceNodeRef	mInputDeviceNode;
    wavy::DftNodeRef	        mDftNode;
    wavy::DwtNodeRef	        mDwtNode;
    SpectrumPlot				mDftPlot;
    SpectrumPlot				mDwtPlot;
};

void CinderScalogramApp::setup()
{
    auto ctx = audio::Context::master();

    mInputDeviceNode = ctx->createInputDeviceNode();

    auto dftFormat = wavy::DftNode::Format()
        .fftSize(2048)
        .windowSize(1024);

    mDftNode = ctx->makeNode(new wavy::DftNode(dftFormat));

    auto dwtFormat = wavy::DwtNode::Format()
        .decompositionLevels(5)
        .motherWavelet(dsp::MotherWavelet::Daubechies5)
        .windowSize(1024);

    mDwtNode = ctx->makeNode(new wavy::DwtNode(dwtFormat));

    mInputDeviceNode >> mDftNode;
    mInputDeviceNode >> mDwtNode;

    mInputDeviceNode->enable();
    ctx->enable();
}

void CinderScalogramApp::resize()
{
    auto margin = 40.0f;
    mDftPlot.setBounds(Rectf(margin, margin, getWindowWidth() - margin, (getWindowHeight() - margin) * 0.5f));
    mDwtPlot.setBounds(Rectf(margin, (getWindowHeight() + margin) * 0.5f, getWindowWidth() - margin, getWindowHeight() - margin));
}

void CinderScalogramApp::update()
{
    
}

void CinderScalogramApp::draw()
{
    gl::clear(Color::black());

    mDwtPlot.draw(mDwtNode->getCoefficients());
    mDftPlot.draw(mDftNode->getMagSpectrum());
}

CINDER_APP(CinderScalogramApp, RendererGl)
