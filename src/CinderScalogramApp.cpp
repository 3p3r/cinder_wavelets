#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderScalogramApp : public App {
public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void update() override;
    void draw() override;
};

void CinderScalogramApp::setup()
{
}

void CinderScalogramApp::mouseDown(MouseEvent event)
{
}

void CinderScalogramApp::update()
{
}

void CinderScalogramApp::draw()
{
    gl::clear(Color(0, 0, 0));
}

CINDER_APP(CinderScalogramApp, RendererGl)
