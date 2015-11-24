/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
* Copyright (c) 2013 Google, Inc.
* Copyright (c) 2015 The Cinder Project
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#if ! defined( CINDER_GL_ES )
	#include "cinder/params/Params.h"
#endif
using namespace ci;
using namespace ci::app;

#include "Render.h"
#include "Test.h"
#include "Arrow.h"
#include "FullscreenUI.h"
#include "ParticleParameter.h"

#include <stdio.h>
#include <algorithm>
#include <string>
#include <sstream>

namespace TestMain
{

namespace {
	int32 testIndex = 0;
	int32 testSelection = 0;
	int32 testCount = 0;
	TestEntry* entry;
	Test* test;
	Settings settings;
	int32 width = 640;
	int32 height = 540;
	float settingsHz = 60.0;

	float32 viewZoom = 1.0f;
	int tx, ty, tw, th;
	bool rMouseDown = false;

	// State of the mouse on the previous call of Mouse().
	int32 previousMouseState = -1;
	b2Vec2 lastp;
	b2Vec2 extents;

	// Fullscreen UI object.
	FullscreenUI fullscreenUI;
	// Used to control the behavior of particle tests.
	ParticleParameter particleParameter;
}

// Set whether to restart the test on particle parameter changes.
// This parameter is re-enabled when the test changes.
void SetRestartOnParticleParameterChange(bool enable)
{
	particleParameter.SetRestartOnChange(enable);
}

// Set the currently selected particle parameter value.  This value must
// match one of the values in TestMain::k_particleTypes or one of the values
// referenced by particleParameterDef passed to SetParticleParameters().
uint32 SetParticleParameterValue(uint32 value)
{
	const int32 index = particleParameter.FindIndexByValue(value);
	// If the particle type isn't found, so fallback to the first entry in the
	// parameter.
	particleParameter.Set(index >= 0 ? index : 0);
	return particleParameter.GetValue();
}

// Get the currently selected particle parameter value and enable particle
// parameter selection arrows on Android.
uint32 GetParticleParameterValue()
{
	// Enable display of particle type selection arrows.
	fullscreenUI.SetParticleParameterSelectionEnabled(true);
	return particleParameter.GetValue();
}

// Override the default particle parameters for the test.
void SetParticleParameters(
	const ParticleParameter::Definition * const particleParameterDef,
	const uint32 particleParameterDefCount)
{
	particleParameter.SetDefinition(particleParameterDef,
									particleParameterDefCount);
}

} // namespace TestMain

using namespace TestMain;

static b2Vec2 ConvertScreenToWorld(int32 x, int32 y)
{
	float32 u = x / float32(tw);
	float32 v = (th - y) / float32(th);

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
}

//! \class TestbedApp
//!
//!
class TestbedApp : public App {
public:

	virtual void setup() override;
	virtual void mouseDown( MouseEvent event ) override;
	virtual void mouseUp( MouseEvent event ) override;
	virtual void mouseDrag( MouseEvent event ) override;
	virtual void keyDown( KeyEvent event ) override;
	virtual void keyUp( KeyEvent event ) override;
	virtual void resize() override;
	virtual void update() override;
	virtual void draw() override;

private:
	void resizeView( int width, int height );

#if ! defined( CINDER_GL_ES )
	params::InterfaceGl		mParams;
#endif
};

void TestbedApp::setup()
{
	std::vector<std::string> testNames;

	testCount = 0;
	while( nullptr != g_testEntries[testCount].createFcn ) {
		testNames.push_back( g_testEntries[testCount].name );
		++testCount;
	}

	testIndex = b2Clamp(testIndex, 0, testCount-1);
	testSelection = testIndex;

	entry = g_testEntries + testIndex;
	if (entry && entry->createFcn) {
		test = entry->createFcn();
		testSelection = testIndex;
		testIndex = -1;
	}

#if ! defined( CINDER_GL_ES )
	mParams = params::InterfaceGl( "Params", ivec2( 250, 400 ) );
	mParams.addParam( "Tests", testNames, &testSelection );
	mParams.addSeparator();
	mParams.addParam( "Vel Iters", &settings.velocityIterations, "min=1 max=500 step=1" );
	mParams.addParam( "Pos Iters", &settings.positionIterations, "min=0 max=100 step=1" );
	mParams.addParam( "Pcl Iters", &settings.particleIterations, "min=1 max=100 step=1" );
	mParams.addParam( "Hertz", &settingsHz, "min=1 max=100 step=1" );
	mParams.addSeparator();
	mParams.addParam( "Sleep", &settings.enableSleep );
	mParams.addParam( "Warm Starting", &settings.enableWarmStarting );
	mParams.addParam( "Time of Impact", &settings.enableContinuous );
	mParams.addParam( "Sub-Stepping", &settings.enableSubStepping );
	mParams.addParam( "Strict Particle/Body Contacts", &settings.strictContacts );
	mParams.addSeparator( "Draw" );
	mParams.addParam( "Shapes", &settings.drawShapes );
	mParams.addParam( "Particles", &settings.drawParticles );
	mParams.addParam( "Joints", &settings.drawJoints );
	mParams.addParam( "AABBs", &settings.drawAABBs) ;
	mParams.addParam( "Contact Points", &settings.drawContactPoints );
	mParams.addParam( "Contact Normals", &settings.drawContactNormals );
	mParams.addParam( "Contact Impulses", &settings.drawContactImpulse );
	mParams.addParam( "Friction Impulses", &settings.drawFrictionImpulse );
	mParams.addParam( "Center of Masses", &settings.drawCOMs );
	mParams.addParam( "Statistics", &settings.drawStats );
	mParams.addParam( "Profile", &settings.drawProfile );
#endif
}

void TestbedApp::mouseDown( MouseEvent event )
{
	int x = event.getPos().x;
	int y = event.getPos().y;
	int button = GLUT_NONE;
	int state = GLUT_NONE;
	int mod = GLUT_NONE;
	if( event.isLeft() ) {
		button = GLUT_LEFT_BUTTON;
		if( event.isLeftDown() ) {
			state = GLUT_DOWN;
		}
	}
	else if( event.isRight() ) {
		button = GLUT_RIGHT_BUTTON;
		if( event.isRightDown() ) {
			state = GLUT_DOWN;
		}
	}

	if (button == GLUT_LEFT_BUTTON)
	{
		b2Vec2 p = ConvertScreenToWorld(x, y);

		switch (fullscreenUI.Mouse(button, state, previousMouseState, p))
		{
		case FullscreenUI::e_SelectionTestPrevious:
			testSelection = std::max(0, testSelection - 1);
			break;
		case FullscreenUI::e_SelectionTestNext:
			if (g_testEntries[testSelection + 1].name) testSelection++;
			break;
		case FullscreenUI::e_SelectionParameterPrevious:
			particleParameter.Decrement();
			break;
		case FullscreenUI::e_SelectionParameterNext:
			particleParameter.Increment();
			break;
		default:
			break;
		}

		if (state == GLUT_DOWN)
		{
			b2Vec2 p = ConvertScreenToWorld(x, y);
			if (mod == GLUT_ACTIVE_SHIFT)
			{
				test->ShiftMouseDown(p);
			}
			else
			{
				test->MouseDown(p);
			}
		}

		/*
		if (state == GLUT_UP)
		{
			test->MouseUp(p);
		}
		*/
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			lastp = ConvertScreenToWorld(x, y);
			rMouseDown = true;
		}

		/*
		if (state == GLUT_UP)
		{
			rMouseDown = false;
		}
		*/
	}
	previousMouseState = state;
}

void TestbedApp::mouseUp( MouseEvent event )
{
	int x = event.getPos().x;
	int y = event.getPos().y;

	int state = GLUT_NONE;
	if( event.isLeft() ) {
		state = GLUT_UP;
		b2Vec2 p = ConvertScreenToWorld(x, y);
		test->MouseUp(p);
	}
	else if( event.isRight() ) {
		state = GLUT_UP;
		rMouseDown = false;
	}
	previousMouseState = state;
}

void TestbedApp::mouseDrag( MouseEvent event )
{
	int x = event.getPos().x;
	int y = event.getPos().y;
	b2Vec2 p = ConvertScreenToWorld(x, y);

	if (fullscreenUI.GetSelection() == FullscreenUI::e_SelectionNone)
	{
		test->MouseMove(p);
	}

	if (rMouseDown)
	{
		b2Vec2 diff = p - lastp;
		settings.viewCenter.x -= diff.x;
		settings.viewCenter.y -= diff.y;
		resizeView( width, height );
		lastp = ConvertScreenToWorld(x, y);
	}
}

void TestbedApp::keyDown(KeyEvent event)
{
	char key = event.getChar();

console() << "key: " << key << " : " << (int)key << std::endl;

	switch (key)
	{
	case 27:
		quit();
		break;

		// Press 'z' to zoom out.
	case 'z':
	case 'Z':
		viewZoom = b2Min(1.1f * viewZoom, 20.0f);
		resizeView( width, height );
		break;

		// Press 'x' to zoom in.
	case 'x':
	case 'X':
		viewZoom = b2Max(0.9f * viewZoom, 0.02f);
		resizeView( width, height );
		break;

		// Press 'r' to reset.
	case 'r':
	case 'R':
		delete test;
		test = entry->createFcn();
		break;

		// Press space to launch a bomb.
	case ' ':
		if (test)
		{
			test->LaunchBomb();
		}
		break;

	case 'p':
	case 'P':
		settings.pause = !settings.pause;
		break;

		// Press [ to prev test.
	case '[':
		--testSelection;
		if (testSelection < 0)
		{
			testSelection = testCount - 1;
		}
		break;

		// Press ] to next test.
	case ']':
		++testSelection;
		if (testSelection == testCount)
		{
			testSelection = 0;
		}
		break;

		// Press < to select the previous particle parameter setting.
	case '<':
		particleParameter.Decrement();
		break;

		// Press > to select the next particle parameter setting.
	case '>':
		particleParameter.Increment();
		break;

	default:
		if( test ) {
			test->Keyboard(key);
		}
	}
}

void TestbedApp::keyUp( KeyEvent event )
{
	char key = event.getChar();
	if (test) {
		test->KeyboardUp( key );
	}
}

void TestbedApp::resizeView( int viewWidth, int viewHeight )
{
	width  = viewWidth;
	height = viewHeight;

	tx = 0;
	ty = 0;
	tw = width;
	th = height;

	gl::viewport( tx, ty, tw, th );

	float32 ratio = th ? float32(tw) / float32(th) : 1;

	extents = ratio >= 1 ? b2Vec2(ratio * 25.0f, 25.0f) : b2Vec2(25.0f, 25.0f / ratio);
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	// L/R/B/T
	LoadOrtho2DMatrix( getWindowWidth(), getWindowHeight(), lower.x, upper.x, lower.y, upper.y);

	if (fullscreenUI.GetEnabled())
	{
		fullscreenUI.SetViewParameters(&settings.viewCenter, &extents);
	}
}

void TestbedApp::resize()
{
	resizeView( getWindowWidth(), getWindowHeight() );
}

void TestbedApp::update()
{

}

void TestbedApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

	settings.hz = settingsHz;

	test->Step(&settings);

	// Update the state of the particle parameter.
	bool restartTest;
	const bool changed = particleParameter.Changed(&restartTest);
	B2_NOT_USED(changed);

	test->DrawTitle(entry->name);

#if ! defined( CINDER_GL_ES )
	mParams.draw();
#endif

	if (testSelection != testIndex || restartTest)
	{
		fullscreenUI.Reset();
		if (!restartTest) particleParameter.Reset();

		testIndex = testSelection;
		delete test;
		entry = g_testEntries + testIndex;
		test = entry->createFcn();
		viewZoom = test->GetDefaultViewZoom();
		settings.viewCenter.Set(0.0f, 20.0f * viewZoom);
		resizeView( width, height );
	}

	// print world step time stats every 600 frames
	static int s_printCount = 0;
	static b2Stat st;
	st.Record(settings.stepTimeOut);

	const int STAT_PRINT_INTERVAL = 600;
	if( settings.printStepTimeStats && st.GetCount() == STAT_PRINT_INTERVAL ) {

		console() << "World Step Time samples "
		          << s_printCount*STAT_PRINT_INTERVAL << "-" << (s_printCount+1)*STAT_PRINT_INTERVAL-1 << ": "
		          << st.GetMin() << "min " <<  st.GetMax() << "max " <<  st.GetMean() << "avg (ms)" << std::endl;
		s_printCount++;
	}
}

void prepareSettings( TestbedApp::Settings *settings )
{
	std::stringstream title;
	title << "Cinder LiquidFun" << " | " << "Box2D Version " << b2_version.major << "." << b2_version.minor << "." << b2_version.revision;
	settings->setTitle( title.str() );
	settings->setWindowSize( 1280, 800 );
	settings->setMultiTouchEnabled( false );
}

CINDER_APP( TestbedApp, RendererGl, prepareSettings )
