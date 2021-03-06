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
vector<Vec2f> reflNormal;
vector<Vec2f> targetPoint;

Vec2f computeNormal( Vec2f eye, Vec2f p, Vec2f target ) {
  Vec2f n;
  Vec2f ve = eye - p;
  ve.Normalize();
  Vec2f vt = target - p;
  vt.Normalize();
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

void thinVector( vector<Vec2f> & v ) {
  int thin = 200;
  vector<Vec2f> sparse;
  for( size_t i = 0; i < v.size(); i += thin ) {
    sparse.push_back( v[i] );
  }
  sparse.push_back( v.back() );
  v = sparse;
}

Vec2f eye(0,0);

void initPoints() {

  reflPoint.clear();
  reflNormal.clear();
  targetPoint.clear();

  int rays = 6000;
  
  Vec2f targetStart( 3, 7 );
  Vec2f targetEnd( 7, 12 );
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
  

  float angle = ToRadians( -45 );
  float angleDelta = ToRadians( 45.0f - -45.0f ) ;

  vector<float> delta;
  float sum = 0;
  for( int i = 0; i < rays; i++ ) {
    float f;
    f = ( i / ( rays / 2.0f ) ) - 1.0f;
    f *= 2.0f;
    f *= f;
    f += 1;
    f = 1;  // hijack distribution to make it uniform
    delta.push_back( f );
    sum += f;
  }
  
  for( int i = 0; i < rays; i++ ) {
    delta[i] *= ( angleDelta / sum );
  }
  
  for( int i = 0; i < rays; i++ ) {
    Vec2f p( cosf( angle ), sinf( angle ) );
    angle += delta[i];
    reflPoint.push_back( p );
  }
  
  reflPoint[0] *= 0.3f;
  
  for( int i = 0; i < rays - 1; i++ ) {
    Vec2f n = computeNormal( eye, reflPoint[i], targetPoint[i] );
    reflNormal.push_back( n );
    
    Vec2f t( n.y, -n.x );
    float u = computeIntersection( eye, reflPoint[i+1], reflPoint[i], reflPoint[i] + t );
    if( u > 0 ) {
      reflPoint[i+1] *= u;
    }
  }
  reflNormal.push_back( computeNormal( eye, reflPoint.back(), targetPoint.back() ) );
  
  thinVector( reflPoint );
  thinVector( reflNormal );
  thinVector( targetPoint );
  
  float minLen = 1000;
  float maxLen = 0;
  float avgLen = 0;
  
  for( size_t i = 0; i < reflPoint.size(); i++ ) {
    float len = (eye - reflPoint[i]).Length() + (targetPoint[i] - reflPoint[i]).Length();
    minLen = min( len, minLen );
    maxLen = max( len, maxLen );
    avgLen += len;
    printf( "Ray len %d: %0.3f\n", (int)i, len );
  }
  
  avgLen /= reflPoint.size();
  
  printf( "Ray length stats: min: %.3f, max: %.3f, avg: %.3f\n", minLen, maxLen, avgLen );
  
}

static void drawRefl( Vec2f t, Vec2f p, Vec2f n ) {
  glVertex2fv( t.Ptr() );
  glVertex2fv( p.Ptr() );
  n.Normalize();
  Vec2f i = t - p;
  Vec2f r = 2 * i.Dot(n) * n - i;
  r.Normalize();
  r *= 4.0f;
  r += p;
  glVertex2fv( p.Ptr() );
  glVertex2fv( r.Ptr() );
}

int currTarget = 0;

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

  // eye
  glBegin(GL_POINTS);
  glVertex2fv( eye.Ptr() );
  glEnd();

  // reflector
  glBegin(GL_LINE_STRIP);
  for( size_t i = 0; i < reflPoint.size(); i++ ) {
    glVertex2fv( reflPoint[i].Ptr() );
  }
  glEnd();

  // display
  glBegin(GL_LINE_STRIP);
  for( size_t i = 0; i < targetPoint.size(); i++ ) {
    glVertex2fv( targetPoint[i].Ptr() );
  }
  glEnd();
  
  if( b['i'] ) {

    glColor3f( 1, 1, 0 );
    glBegin(GL_LINES);
    drawRefl( targetPoint[currTarget], reflPoint[currTarget], reflNormal[currTarget] );
    for( int i = -2; i <= 2; i++ ) {
      int adj = currTarget + i;
      if( adj < 0 || adj >= reflPoint.size() ) {
        continue;
      }
      drawRefl( targetPoint[currTarget], reflPoint[adj], reflNormal[adj] );
    }
    glEnd();
    
  } else {
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

  }

  if( b['n'] ) {
    glColor3f(0, .5, .5);
    glBegin(GL_LINES);
    for( size_t i = 0; i < reflNormal.size(); i++ ) {
      glVertex2fv( (0.3f * reflNormal[i] + reflPoint[i]).Ptr() );
      glVertex2fv( reflPoint[i].Ptr() );
    }
    glEnd();
  }
  

  
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
    case 't':
      currTarget++;
      if( currTarget >= targetPoint.size() ) {
        currTarget = 0;
      }
      break;
    case 'T':
      currTarget--;
      if( currTarget < 0 ) {
        currTarget = targetPoint.size() - 1;
      }
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

