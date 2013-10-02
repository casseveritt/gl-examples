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

// Simple subdivision surface self-tutorial
// Cass Everitt - Sept 15, 2013

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include <GL/Regal.h>
#include <GL/RegalGLUT.h>


// subdiv data structures
#include "r3/linear.h"
#include <vector>
using namespace std;



int width, height;
bool b[256];

int mousebtn = -1;
r3::Vec2f mousepos;
r3::Vec3f trans( 0, 0, -3 );
r3::Rotationf rot;



void draw_model() {
  
  glColor3f( 1, 1, 1 );
  glBegin( GL_TRIANGLES );
  glVertex2f( -1, -1 );
  glVertex2f(  1, -1 );
  glVertex2f(  0, 0.7f );
  glEnd();
  
}


void mouse( int button, int state, int x, int y ) {
  //printf( "Mouse func %d %d %d %d\n", button, state, x, y );
  y = height - 1 - y;
  if( state == GLUT_DOWN ) {
    mousebtn = button;
    mousepos = r3::Vec2f( x, y );
  } else {
    mousebtn = -1;
  }
  glutPostRedisplay();
}

void project_to_hump( r3::Vec3f & v ) {
  v.z = pow( 2.0, -(v.x * v.x + v.y * v.y) );
  v.Normalize();
}

void motion( int x, int y ) {
  //printf( "Motion func %d %d\n", x, y );
  if( mousebtn == -1 ) {
    return;
  }
  y = height - 1 - y;
  r3::Vec2f mp = r3::Vec2f( x, y );
  switch( mousebtn ) {
    case GLUT_RIGHT_BUTTON:
      trans.z -= 0.02f * ( mp.y - mousepos.y );
      break;
    case GLUT_LEFT_BUTTON:
    {
      float mindim = width < height ? width : height;
      r3::Vec3f p0( mousepos.x - width / 2, mousepos.y - height / 2, 0 );
      r3::Vec3f p1( mp.x - width / 2, mp.y - height / 2, 0 );
      p0 /= mindim / 2;
      p1 /= mindim / 2;
      project_to_hump( p0 );
      project_to_hump( p1 );
      r3::Rotationf r;
      r.SetValue( p0, p1 );
      rot = r * rot;
    }
    default:
      break;
  }
  mousepos = mp;
  glutPostRedisplay();
}

void reshape( int w, int h ) {
  width = w;
  height = h;
  glViewport( 0, 0, width, height );
  float aspect = float(width)/float(height);
  float fov = 0.08;
  glMatrixLoadIdentityEXT( GL_PROJECTION );
  if( aspect >= 1.0 ) {
    glMatrixFrustumEXT( GL_PROJECTION, -fov * aspect, fov * aspect, -fov, fov, 0.1, 10);
  } else {
    glMatrixFrustumEXT( GL_PROJECTION, -fov, fov, -fov/aspect, fov/aspect, 0.1, 10);
  }
  glutPostRedisplay();
}

static void display()
{
  void orig_idle();
  orig_idle();
  
  glClearColor( 0.5, 0.25, .25, 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  

  glMatrixPushEXT( GL_MODELVIEW );
  glMatrixTranslatefEXT( GL_MODELVIEW, trans.x, trans.y, trans.z );
  r3::Vec3f axis;
  float angle;
  rot.GetValue( axis, angle );
  glMatrixRotatefEXT( GL_MODELVIEW, r3::ToDegrees( angle ), axis.x, axis.y, axis.z );
  
  draw_model();

  glMatrixPopEXT( GL_MODELVIEW );
  
  glutSwapBuffers();
}

static void init_opengl() {
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LESS );
  GLfloat shininess = 64.0f;
  glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, &shininess );
  GLfloat spec[] = { 1, 1, 1, 1 };
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, spec );
  GLfloat lightpos[] = { 0.5, 1, 1, 0 };
  glLightfv( GL_LIGHT0, GL_POSITION, lightpos );
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
    default:
      break;
  }
  glutPostRedisplay();
}


int main(int argc, const char * argv[])
{
  glutInitDisplayString("rgba>=8 depth double samples=4");
  glutInitWindowSize(768, 768);
  glutInit( &argc, (char **) argv);
  glutCreateWindow( "osdsimple" );

#if REGAL_SYS_OSX
  // Regal workaround for OSX GLUT
  RegalMakeCurrent(CGLGetCurrentContext());
#endif
  
  init_opengl();
  
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  
  return 0;
}

