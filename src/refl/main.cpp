/*
 Copyright (c) 2013 NVIDIA Corporation
 Copyright (c) 2013 Cass Everitt
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 
 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Simple gl example template
// Cass Everitt - Sept 15, 2013

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include <GL/Regal.h>

#if REGAL_SYS_OSX
#  include <GLUT/GLUT.h>
#  include <GL/RegalCGL.h>
#else
#  include <GL/glut.h>
#endif

#include <math.h>
#include <r3/linear.h>
#include <vector>

using namespace std;
using namespace r3;

int width, height;
float aspect = 1.0f;
bool b[256];

void reshape( int w, int h ) {
  width = w;
  height = h;
  aspect = float(w)/float(h);
  glViewport( 0, 0, width, height );
  glMatrixLoadIdentityEXT( GL_PROJECTION );
  glMatrixOrthoEXT( GL_PROJECTION, -1.0 * aspect, 1.0f * aspect, -1.0f, 1.0f, -1, 1 );
  glutPostRedisplay();
}

Vec2f translate;
float scale = 1.0f;

vector<Vec2f> reflPoint;
vector<Vec2f> targetPoint;

Vec2f computeNormal( Vec2f eye, Vec2f p, Vec2f target ) {
  Vec2f n;
  Vec2f ve = eye - p;
  Vec2f vt = target - p;
  n = ve + vt;
  n.Normalize();
  return n;
}

float computeIntersection( Vec2f p1, Vec2f p2, Vec2f p3, Vec2f p4 ) {
  
  float denom = ( p1.x - p2.x ) * ( p3.y - p4.y ) - ( p1.y - p2.y ) * ( p3.x - p4.x );
  if( denom == 0 ) {
    return -1.0f;
  }
  
  Vec2f m;
  m.x =  ( p1.x * p2.y - p1.y * p2.x ) * ( p3.x - p4.x ) - ( p1.x - p2.x ) * ( p3.x * p4.y - p3.y * p4.x );
  m.y =  ( p1.x * p2.y - p1.y * p2.x ) * ( p3.y - p3.y ) - ( p1.y - p2.y ) * ( p3.x * p4.y - p3.y * p4.x );
  
  m /= denom;

  m -= p1;
  p2 -= p1;
  
  return m.Length() / p2.Length();
}


void initPoints() {

  int rays = 600;
  
  Vec2f targetStart( 3, 4 );
  Vec2f targetEnd( 9, 10 );
  targetStart /= 20.f;
  targetEnd /= 20.f;
  targetPoint.clear();
  for( int i = 0; i < rays; i++ ) {
    float f = float(i) / (rays - 1);
    Vec2f p = f * targetEnd + (1.0f - f) * targetStart;
    targetPoint.push_back( p );
  }

  
  
  Vec2f reflStart( 3, -3 );
  Vec2f reflEnd( 6, 3 );
  reflStart /= 20.f;
  reflEnd /= 20.f;
  reflPoint.clear();

  float angle = ToRadians( -45 );
  float angleIncr = ToRadians( (45.0f - -45.0f ) / ( rays - 1 ) );

  for( int i = 0; i < rays; i++ ) {
    Vec2f p( cosf( angle ), sinf( angle ) );
    angle += angleIncr;
    reflPoint.push_back( p );
  }
  
  reflPoint[0] *= 0.4f;
  
  for( int i = 0; i < rays - 1; i++ ) {
    Vec2f n = computeNormal( Vec2f(0,0), reflPoint[i], targetPoint[i] );
    Vec2f t( n.y, -n.x );
    float u = computeIntersection( Vec2f(0,0), reflPoint[i+1], reflPoint[i], reflPoint[i] + t );
    if( u > 0 ) {
      reflPoint[i+1] *= u;
    }
  }
  
  int thin = 20;
  vector<Vec2f> sparse;
  for( size_t i = 0; i < reflPoint.size(); i += thin ) {
    sparse.push_back( reflPoint[i] );
  }

  reflPoint = sparse;
  sparse.clear();
  for( size_t i = 0; i < targetPoint.size(); i += thin ) {
    sparse.push_back( targetPoint[i] );
  }
  targetPoint = sparse;
  
}

static void display()
{
  
  glClearColor( 0, 0, 0, 0 );
  glClear( GL_COLOR_BUFFER_BIT );

  glMatrixPushEXT( GL_MODELVIEW );
  glMatrixLoadIdentityEXT( GL_MODELVIEW );
  glMatrixScalefEXT( GL_MODELVIEW, scale, scale, scale );
  glMatrixTranslatefEXT( GL_MODELVIEW, translate.x, translate.y, 0 );
  
  
  GLfloat colors[][3] = {
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 0, 1, 0 },
    { 0, 1, 1 },
    { 0, 0, 1 },
    { 1, 0, 1 },
    { 1, .5, .5 }
  };
  
  glColor3f( 1, 1, 1 );
  glBegin( GL_LINES );
  for( size_t i = 0; i < targetPoint.size(); i++ ) {
    glColor3fv( colors[ i % 7 ] );
    glVertex2f( 0, 0 );
    glVertex2fv( reflPoint[i].Ptr() );
    glVertex2fv( reflPoint[i].Ptr() );
    glVertex2fv( targetPoint[i].Ptr() );
  }
  glEnd();
  
  glMatrixPopEXT( GL_MODELVIEW );
  
  glutSwapBuffers();
}

static void init_opengl() {
}

static void keyboard(unsigned char c, int x, int y)
{
  b[c] = ! b[c];
  switch (c)
  {
    case 'q':
    case 27:  /* Esc key */
      exit(0);
      break;
    case 's':
      scale *= 1.05f;
      break;
    case 'S':
      scale /= 1.05f;
      break;
    case 'x':
      translate.x += .05f;
      break;
    case 'X':
      translate.x -= .05f;
      break;
    case 'y':
      translate.y += .05f;
      break;
    case 'Y':
      translate.y -= .05f;
      break;
    default:
      break;
  }
  glutPostRedisplay();
}


int main(int argc, const char * argv[])
{
  glutInitDisplayString("rgba>=8 depth double");
  glutInitWindowSize(768, 768);
  glutInit( &argc, (char **) argv);
  glutCreateWindow( "refl" );

#if REGAL_SYS_OSX
  // Regal workaround for OSX GLUT
  RegalMakeCurrent(CGLGetCurrentContext());
#endif

  init_opengl();
  initPoints();
  
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  
  return 0;
}

