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

#include "Render.h"
//
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Surface.h"
#include "cinder/TriMesh.h"
using namespace ci;

#include <stdio.h>
#include <stdarg.h>

// Global vars for drawing in Cinder
static int gWindowWidth = 0;
static int gWindowHeight = 0;
static CameraOrtho gOrthoCam;
static gl::TextureFontRef gFont18;
static gl::TextureRef gParticleTex;

const float kPointSize = 0.05f/40.0f;
static float gPointScale = kPointSize;

static gl::BatchRef 			gBatch;
static std::vector<uint32_t>	gIndicesBuffer( 10000 );
static std::vector<vec2>		gPositionsBuffer( 10000 );
static std::vector<vec2>		gTexCoordsBuffer( 10000 );
static std::vector<ColorA>		gColorsBuffer( 10000 );

void LoadOrtho2DMatrix( int windowWidth, int windowHeight, double left, double right, double bottom, double top)
{
	gWindowWidth = windowWidth;
	gWindowHeight = windowHeight;

	gOrthoCam.setOrtho( left, right, bottom, top, -1.0f, 1.0f );

	gPointScale = kPointSize*(top - bottom);
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	gl::setMatrices( gOrthoCam );

	gl::color( color.r, color.g, color.b );
	gl::begin( GL_LINE_LOOP );
	for( int32 i = 0; i < vertexCount; ++i ) {
		gl::vertex( vec2( vertices[i].x, vertices[i].y ) );
	}
	gl::end();
}

void DebugDraw::DrawFlatPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	gl::setMatrices( gOrthoCam );

	gl::color( color.r, color.g, color.b, 1.0f );
	gl::begin( GL_TRIANGLE_FAN );
	for( int32 i = 0; i < vertexCount; ++i ) {
		gl::vertex( vec2( vertices[i].x, vertices[i].y ) );
	}
	gl::end();
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	gl::setMatrices( gOrthoCam );

	{
		gl::ScopedBlendAlpha scopedBlend;

		gl::color( 0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f );
		gl::begin( GL_TRIANGLE_FAN );
		for( int32 i = 0; i < vertexCount; ++i ) {
			gl::vertex( vec2( vertices[i].x, vertices[i].y ) );
		}
		gl::end();
	}

	gl::color( color.r, color.g, color.b, 1.0f );
	gl::begin( GL_LINE_LOOP );
	for( int32 i = 0; i < vertexCount; ++i ) {
		gl::vertex( vec2( vertices[i].x, vertices[i].y ) );
	}
	gl::end();
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	gl::setMatrices( gOrthoCam );

	gl::color( color.r, color.g, color.b );
	gl::drawStrokedCircle( vec2( center.x, center.y ), radius, 16 );
}

float smoothstep(float x) { return x * x * (3 - 2 * x); }

void DebugDraw::DrawParticles(const b2Vec2 *centers, float32 radius, const b2ParticleColor *colors, int32 count)
{
	// Textures so the particles look pretty
	if( ! gParticleTex ) {
		const int TSIZE = 64;
		unsigned char tex[TSIZE][TSIZE][4];
		for (int y = 0; y < TSIZE; y++)
		{
			for (int x = 0; x < TSIZE; x++)
			{
				float fx = (x + 0.5f) / TSIZE * 2 - 1;
				float fy = (y + 0.5f) / TSIZE * 2 - 1;
				float dist = sqrtf(fx * fx + fy * fy);
				unsigned char intensity = (unsigned char)(dist <= 1 ? smoothstep(1 - dist) * 255 : 0);
				tex[y][x][0] = tex[y][x][1] = tex[y][x][2] = 128;
				tex[y][x][3] = intensity;
			}
		}

		SurfaceRef surf = Surface::create( (uint8_t*)tex, TSIZE, TSIZE, TSIZE*4, SurfaceChannelOrder::RGBA );
		gParticleTex = gl::Texture::create( *surf );
	}

	// Batch and client buffer
	if( ! gBatch ) {
		gl::GlslProgRef shader = gl::context()->getStockShader( gl::ShaderDef().texture( gParticleTex ).color() );

		// Create separate VBOs for each of the attributes so we can buffer data as needed.
		// A singlve VBO using interleaved or planar will crash if we buffer larger amounts
		// of data than the inital allocation.
		auto positionLayout = geom::BufferLayout( std::vector<geom::AttribInfo>( 1, geom::AttribInfo( geom::POSITION,    2,  8, 0 ) ) );
		auto texCoordLayout = geom::BufferLayout( std::vector<geom::AttribInfo>( 1, geom::AttribInfo( geom::TEX_COORD_0, 2,  8, 0 ) ) );
		auto colorLayout    = geom::BufferLayout( std::vector<geom::AttribInfo>( 1, geom::AttribInfo( geom::COLOR,       4, 16, 0 ) ) );

		auto indicesVbo   = gl::Vbo::create( GL_ELEMENT_ARRAY_BUFFER );
		auto positionsVbo = gl::Vbo::create( GL_ARRAY_BUFFER );
		auto texCoordsVbo = gl::Vbo::create( GL_ARRAY_BUFFER );
		auto colorsVbo    = gl::Vbo::create( GL_ARRAY_BUFFER );

		std::vector<std::pair<geom::BufferLayout, gl::VboRef>> vertexArrayBuffers = {
			std::make_pair( positionLayout, positionsVbo ),
			std::make_pair( texCoordLayout, texCoordsVbo ),
			std::make_pair( colorLayout,    colorsVbo  ),
		};

		// Passing 1 into numIndices forces gl::VboMesh to indices mode
		gl::VboMeshRef vboMesh = gl::VboMesh::create( 0, GL_TRIANGLES, vertexArrayBuffers, 1, GL_UNSIGNED_INT,  indicesVbo );
		gBatch = gl::Batch::create( vboMesh, shader );
	}

	const size_t kIndicesPerPrim   = 6;
	const size_t kPositionsPerPrim = 4;
	const size_t kTexCoordsPerPrim = 4;
	const size_t kColorsPerPrim    = 4;		
	int numParticles = gIndicesBuffer.size() / kIndicesPerPrim;
	// Resize client buffers if we need more
	if( count > numParticles ) {
		gIndicesBuffer.resize( count*kIndicesPerPrim );
		gPositionsBuffer.resize( count*kPositionsPerPrim );
		gTexCoordsBuffer.resize( count*kTexCoordsPerPrim );
		gColorsBuffer.resize( count*kColorsPerPrim );
	}

	const float particle_size_multiplier = 2.0f;  // because of falloff
	const float pointSize = radius  * particle_size_multiplier;

	for( int32 i = 0; i < count; ++i ) {
		// Counter-clockwise
		vec2 P0 = vec2( centers[i].x, centers[i].y ) + vec2( -pointSize, -pointSize );
		vec2 P1 = vec2( centers[i].x, centers[i].y ) + vec2(  pointSize, -pointSize );
		vec2 P2 = vec2( centers[i].x, centers[i].y ) + vec2(  pointSize,  pointSize );
		vec2 P3 = vec2( centers[i].x, centers[i].y ) + vec2( -pointSize,  pointSize );
		vec2 uv0 = vec2( 0, 0 );
		vec2 uv1 = vec2( 1, 0 );
		vec2 uv2 = vec2( 1, 1 );
		vec2 uv3 = vec2( 0, 1 );
		ColorA c = ColorA( 1.0f, 1.0f, 1.0f, 1.0f );
		if( nullptr != colors ) {
			c = ColorA( colors[i].r/255.0f, colors[i].g/255.0f, colors[i].b/255.0f, colors[i].a/255.0f );
		}
		gPositionsBuffer[4*i + 0] = P0;
		gPositionsBuffer[4*i + 1] = P1;
		gPositionsBuffer[4*i + 2] = P2;
		gPositionsBuffer[4*i + 3] = P3;
		gTexCoordsBuffer[4*i + 0] = uv0;
		gTexCoordsBuffer[4*i + 1] = uv1;
		gTexCoordsBuffer[4*i + 2] = uv2;
		gTexCoordsBuffer[4*i + 3] = uv3;
		gColorsBuffer[4*i + 0] = c;
		gColorsBuffer[4*i + 1] = c;
		gColorsBuffer[4*i + 2] = c;
		gColorsBuffer[4*i + 3] = c;
		uint32_t n = static_cast<uint32_t>( 4*(i + 1) );
		uint32_t v0 = n - 4;
		uint32_t v1 = n - 3;
		uint32_t v2 = n - 2;
		uint32_t v3 = n - 1;
		// First triangle
		gIndicesBuffer[6*i + 0] = v0;
		gIndicesBuffer[6*i + 1] = v1;
		gIndicesBuffer[6*i + 2] = v2;
		// Second triangle
		gIndicesBuffer[6*i + 3] = v0;
		gIndicesBuffer[6*i + 4] = v2;
		gIndicesBuffer[6*i + 5] = v3;
	}


	auto vboMesh = gBatch->getVboMesh();

	// Buffer the data to GPU
	auto indicesVbo = vboMesh->getIndexVbo();
	auto positionsVbo = vboMesh->findAttrib( geom::Attrib::POSITION )->second;
	auto texCoordsVbo = vboMesh->findAttrib( geom::Attrib::TEX_COORD_0 )->second;
	auto colorsVbo = vboMesh->findAttrib( geom::Attrib::COLOR )->second;
	indicesVbo->bufferData( count*kIndicesPerPrim*sizeof(uint32_t), gIndicesBuffer.data(), GL_DYNAMIC_DRAW );
	positionsVbo->bufferData( count*kPositionsPerPrim*sizeof(vec2), gPositionsBuffer.data(), GL_DYNAMIC_DRAW );
	texCoordsVbo->bufferData( count*kTexCoordsPerPrim*sizeof(vec2), gTexCoordsBuffer.data(), GL_DYNAMIC_DRAW );
	colorsVbo->bufferData( count*kColorsPerPrim*sizeof(ColorA), gColorsBuffer.data(), GL_DYNAMIC_DRAW );

	gl::setMatrices( gOrthoCam );
	gl::ScopedBlendAlpha scopedBlend;

	gl::ScopedTextureBind scopedTex( gParticleTex, 0 );

	gBatch->draw( 0, count*kIndicesPerPrim );
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	gl::setMatrices( gOrthoCam );

	gl::ScopedBlendAlpha scopedBlend;

	gl::color( 0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f );
	gl::drawSolidCircle( vec2( center.x, center.y ), radius, 16 );

	gl::color( color.r, color.g, color.b, 1.0f );
	gl::drawStrokedCircle( vec2( center.x, center.y ), radius, 16 );

	b2Vec2 p = center + radius * axis;
	gl::drawLine( vec2( center.x, center.y ), vec2( p.x, p.y ) );
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	gl::setMatrices( gOrthoCam );

	gl::color( color.r, color.g, color.b );
	gl::drawLine( vec2( p1.x, p1.y ), vec2( p2.x, p2.y ) );
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	gl::setMatrices( gOrthoCam );

	b2Vec2 p1 = xf.p, p2;
	const float32 k_axisScale = 0.4f;

	gl::color( 1.0f, 0.0f, 0.0f );
	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	gl::drawLine( vec2( p1.x, p1.y ), vec2( p2.x, p2.y ) );

	gl::color( 0.0f, 1.0f, 0.0f );
	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	gl::drawLine( vec2( p1.x, p1.y ), vec2( p2.x, p2.y ) );
}

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	gl::setMatrices( gOrthoCam );

	gl::color( color.r, color.g, color.b );

	size *= gPointScale;
	Rectf r = Rectf( p.x - size, p.y - size, p.x + size, p.y + size );
	gl::drawSolidRect( r );
}

void DebugDraw::DrawString(int x, int y, const char *string, ...)
{
	if( ! gFont18 ) {
		gFont18 = gl::TextureFont::create( ci::Font( "Arial", 18.0f ) );
	}

	char buffer[128];

	va_list arg;
	va_start(arg, string);
	vsprintf(buffer, string, arg);
	va_end(arg);

	std::string s = std::string( buffer, strlen( buffer ) );

	// Use a pixel aligned camera with origin at top left
	gl::setMatricesWindow( gWindowWidth, gWindowHeight, true );

	gl::color( 0.9f, 0.6f, 0.6f );
	// Cinder draws using baseline so offset on y-axis
	gFont18->drawString( s, vec2( x, y + 18.0f ) );
}

void DebugDraw::DrawString(const b2Vec2& p, const char *string, ...)
{
/*
#if !defined(__ANDROID__) && !defined(__IOS__)
	char buffer[128];

	va_list arg;
	va_start(arg, string);
	vsprintf(buffer, string, arg);
	va_end(arg);

	glColor3f(0.5f, 0.9f, 0.5f);
	glRasterPos2f(p.x, p.y);

	int32 length = (int32)strlen(buffer);
	for (int32 i = 0; i < length; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
	}

	glPopMatrix();
#endif // !defined(__ANDROID__) && !defined(__IOS__)
*/
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	gl::color( c.r, c.g, c.b );

	Rectf r = Rectf( aabb->lowerBound.x, aabb->lowerBound.y, aabb->upperBound.x, aabb->upperBound.y );
	gl::drawStrokedRect( r );
}

float ComputeFPS()
{
/*
	static bool debugPrintFrameTime = false;
	static int lastms = 0;
	int curms = glutGet(GLUT_ELAPSED_TIME);
	int delta = curms - lastms;
	lastms = curms;

	static float dsmooth = 16;
	dsmooth = (dsmooth * 30 + delta) / 31;

	if ( debugPrintFrameTime )
	{
#ifdef ANDROID
		__android_log_print(ANDROID_LOG_VERBOSE, "Testbed", "msec = %f", dsmooth);
#endif
	}

	return dsmooth;
*/
	return 0.0f;
}
