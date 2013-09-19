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
#include <set>
using namespace std;

namespace subdiv {

  typedef r3::Vec3f Vec3f;

  struct Vertex {
    vector<int> edgeIndex;
    vector<int> faceIndex;
  };
  
  struct Edge {
    Edge() : v0(-1), v1(-1), f0(-1), f1(-1) {}
    Edge( int vi0, int vi1, int face ) {
      if( vi0 < vi1 ) {
        v0 = vi0;
        v1 = vi1;
        f0 = face;
        f1 = -1;
      } else {
        v1 = vi0;
        v0 = vi1;
        f0 = -1;
        f1 = face;
      }
    }
    void AddFace( int vi0, int vi1, int face ) {
      if( vi0 < vi1 ) {
        assert( f0 == -1 );
        f0 = face;
      } else {
        assert( f1 == -1 );
        f1 = face;
      }
    }
    int v0, v1;
    int f0, f1;
  };
  bool operator<( const Edge & a, const Edge & b ) {
    return a.v0 < b.v0 || ( ( a.v0 == b.v0 ) && ( a.v1 < b.v1 ) );
  }
  
  struct Face {
    vector<int> vertIndex;
    vector<int> edgeIndex;
  };
  
  struct Topo {
    vector<Vertex> vert;
    vector<Face> face;
    vector<Edge> edge;
  };
  
  struct Model {
    Model() : prev(NULL), next(NULL) {}
    vector<Vec3f> vpos;
    Topo topo;
    Model *prev;
    Model *next;
  };
  
}

subdiv::Model model;

void derive_subdiv_from_face_verts( subdiv::Topo & t ) {
  map<subdiv::Edge, int> em;
  int maxvert = 0;
  for( int i = 0; i < t.face.size(); i++ ) {
    subdiv::Face &f = t.face[i];
    assert( f.vertIndex.size() > 2 );
    for( int j = 0; j < f.vertIndex.size(); j++ ) {
      int j0 = f.vertIndex[ j ];
      int j1 = f.vertIndex[ ( j + 1 ) % f.vertIndex.size() ];
      subdiv::Edge e( j0, j1, i );
      int eidx = -1;
      if( em.count( e ) != 0 ) {
        eidx = em[e];
        subdiv::Edge & ee = t.edge[ eidx ];
        ee.AddFace( j0, j1, i );
      } else {
        eidx = em[e] = (int)t.edge.size();
        t.edge.push_back( e );
      }
      f.edgeIndex.push_back( eidx );
      maxvert = max( maxvert, j0 );
    }
  }
  t.vert.resize( maxvert + 1 );
  for( int i = 0; i < t.face.size(); i++ ) {
    subdiv::Face &f = t.face[i];
    for( int j = 0; j < f.vertIndex.size(); j++ ) {
      t.vert[ f.vertIndex[ j ] ].faceIndex.push_back( i );
    }
  }
  for( int i = 0; i < t.edge.size(); i++ ) {
    subdiv::Edge &e = t.edge[i];
    assert( e.v0 >= 0 );
    assert( e.v1 >= 0 );
    t.vert[ e.v0 ].edgeIndex.push_back( i );
    t.vert[ e.v1 ].edgeIndex.push_back( i );
  }
}


void subdivide_model( subdiv::Model & m ) {
  delete m.next;
  m.next = new subdiv::Model();
  subdiv::Model &r = *m.next;
  size_t rverts = m.topo.vert.size() + m.topo.face.size() + m.topo.edge.size();
  r.vpos.resize( rverts );
  // finish it!
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
  m.topo.face.push_back( f );
  // -z
  f = subdiv::Face();
  f.vertIndex.push_back( 5 );
  f.vertIndex.push_back( 4 );
  f.vertIndex.push_back( 7 );
  f.vertIndex.push_back( 6 );
  m.topo.face.push_back( f );
  // +x
  f = subdiv::Face();
  f.vertIndex.push_back( 1 );
  f.vertIndex.push_back( 5 );
  f.vertIndex.push_back( 6 );
  f.vertIndex.push_back( 2 );
  m.topo.face.push_back( f );
  // -x
  f = subdiv::Face();
  f.vertIndex.push_back( 4 );
  f.vertIndex.push_back( 0 );
  f.vertIndex.push_back( 3 );
  f.vertIndex.push_back( 7 );
  m.topo.face.push_back( f );
  // +y
  f = subdiv::Face();
  f.vertIndex.push_back( 3 );
  f.vertIndex.push_back( 2 );
  f.vertIndex.push_back( 6 );
  f.vertIndex.push_back( 7 );
  m.topo.face.push_back( f );
  // -y
  f = subdiv::Face();
  f.vertIndex.push_back( 4 );
  f.vertIndex.push_back( 5 );
  f.vertIndex.push_back( 1 );
  f.vertIndex.push_back( 0 );
  m.topo.face.push_back( f );
  
  derive_subdiv_from_face_verts( m.topo );
}


void draw_model( subdiv::Model & m ) {
  glPolygonOffset( 1, 1 );
  glEnable( GL_POLYGON_OFFSET_FILL );
  glColor3f( 1, 1, 1 );
  for( int i = 0; i < m.topo.face.size(); i++ ) {
    subdiv::Face &f = m.topo.face[i];
    glBegin( GL_TRIANGLE_FAN );
    for( int j = 0; j < f.vertIndex.size(); j++) {
      glVertex3fv( m.vpos[ f.vertIndex[ j ] ].Ptr() );
    }
    glEnd();
  }
  glDisable( GL_POLYGON_OFFSET_FILL );
  glColor3f( 0.4, 0.4, 0.4 );
  glBegin( GL_LINES );
  for( int i = 0; i < m.topo.edge.size(); i++ ) {
    subdiv::Edge &e = m.topo.edge[i];
    glVertex3fv( m.vpos[ e.v0 ].Ptr() );
    glVertex3fv( m.vpos[ e.v1 ].Ptr() );
  }
  glEnd();
}


int width, height;
bool b[256];

int mousebtn = -1;
r3::Vec2f mousepos;
r3::Vec3f trans( 0, 0, -3 );
r3::Rotationf rot;

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
  
  glClearColor( 0.5, 0.25, .25, 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  glMatrixPushEXT( GL_MODELVIEW );
  glMatrixTranslatefEXT( GL_MODELVIEW, trans.x, trans.y, trans.z );
  r3::Vec3f axis;
  float angle;
  rot.GetValue( axis, angle );
  glMatrixRotatefEXT( GL_MODELVIEW, r3::ToDegrees( angle ), axis.x, axis.y, axis.z );
  draw_model( model );
  glMatrixPopEXT( GL_MODELVIEW );
  
  glutSwapBuffers();
}

static void init_opengl() {
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LESS );
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
  glutCreateWindow( "subdiv" );

#if REGAL_SYS_OSX
  // Regal workaround for OSX GLUT
  RegalMakeCurrent(CGLGetCurrentContext());
#endif
  
  init_opengl();
  
  build_subdiv_cube( model );
  
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  
  return 0;
}

