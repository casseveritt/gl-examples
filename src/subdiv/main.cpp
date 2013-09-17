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
#include <map>
using namespace std;

namespace subdiv {

  typedef r3::Vec3f Vec3f;

  struct Vertex {
    vector<int> edgeIndex;
    vector<int> faceIndex;
  };
  
  struct Edge {
    Edge() : v0(-1), v1(-1), f0(-1), f1(-1) {}
    int v0, v1;
    int f0, f1;
  };
  
  struct Face {
    vector<int> vertIndex;
    vector<int> edgeIndex;
  };
  
  struct Model {
    vector<Vec3f> vpos;
    vector<Vertex> vert;
    vector<Face> face;
    vector<Edge> edge;
  };

}

subdiv::Model model;

void derive_subdiv_from_face_verts( subdiv::Model & m ) {
  
}

void build_subdiv_cube( subdiv::Model & m ) {
  m = subdiv::Model();
  m.vpos.push_back( r3::Vec3f( -1, -1,  1 ) );
  m.vpos.push_back( r3::Vec3f(  1, -1,  1 ) );
  m.vpos.push_back( r3::Vec3f(  1,  1,  1 ) );
  m.vpos.push_back( r3::Vec3f( -1,  1,  1 ) );
  m.vpos.push_back( r3::Vec3f( -1, -1, -1 ) );
  m.vpos.push_back( r3::Vec3f(  1, -1, -1 ) );
  m.vpos.push_back( r3::Vec3f(  1,  1, -1 ) );
  m.vpos.push_back( r3::Vec3f( -1,  1, -1 ) );
  // +z
  subdiv::Face f;
  f.vertIndex.push_back( 0 );
  f.vertIndex.push_back( 1 );
  f.vertIndex.push_back( 2 );
  f.vertIndex.push_back( 3 );
  m.face.push_back( f );
  // -z
  f = subdiv::Face();
  f.vertIndex.push_back( 5 );
  f.vertIndex.push_back( 4 );
  f.vertIndex.push_back( 7 );
  f.vertIndex.push_back( 6 );
  m.face.push_back( f );
  // +x
  f = subdiv::Face();
  f.vertIndex.push_back( 1 );
  f.vertIndex.push_back( 5 );
  f.vertIndex.push_back( 6 );
  f.vertIndex.push_back( 2 );
  m.face.push_back( f );
  // -x
  f = subdiv::Face();
  f.vertIndex.push_back( 4 );
  f.vertIndex.push_back( 0 );
  f.vertIndex.push_back( 3 );
  f.vertIndex.push_back( 7 );
  m.face.push_back( f );
  // +y
  f = subdiv::Face();
  f.vertIndex.push_back( 3 );
  f.vertIndex.push_back( 2 );
  f.vertIndex.push_back( 6 );
  f.vertIndex.push_back( 7 );
  m.face.push_back( f );
  // -y
  f = subdiv::Face();
  f.vertIndex.push_back( 4 );
  f.vertIndex.push_back( 5 );
  f.vertIndex.push_back( 1 );
  f.vertIndex.push_back( 0 );
  m.face.push_back( f );
  
}




int width, height;
bool b[256];

void reshape( int w, int h ) {
  width = w;
  height = h;
  glViewport( 0, 0, width, height );
  glutPostRedisplay();
}

static void display()
{
  
  glClearColor( 0, 0, 1, 0 );
  glClear( GL_COLOR_BUFFER_BIT );
  
  glColor3f( 1, 1, 1 );
  glPushMatrix();
  glScalef( 0.9, 0.9, 0.9 );
  glBegin( GL_QUADS );
  glVertex2f  ( -1, -1 );
  glVertex2f  (  1, -1 );
  glVertex2f  (  1,  1 );
  glVertex2f  ( -1,  1 );
  glEnd();
  glPopMatrix();
  
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
  glutCreateWindow( "subdiv" );

#if REGAL_SYS_OSX
  // Regal workaround for OSX GLUT
  RegalMakeCurrent(CGLGetCurrentContext());
#endif
  
  init_opengl();
  
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  
  return 0;
}

