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
    vector<size_t> edgeIndex;
    vector<size_t> faceIndex;
  };
  
  struct Edge {
    Edge() : v0( ~0 ), v1( ~0 ), f0( ~0 ), f1( ~0 ), crease( 0.0f ) {}
    Edge( size_t vi0, size_t vi1, size_t face ) : crease( 0.0f ) {
      if( vi0 < vi1 ) {
        v0 = vi0;
        v1 = vi1;
        f0 = face;
        f1 = ~0;
      } else {
        v1 = vi0;
        v0 = vi1;
        f0 = ~0;
        f1 = face;
      }
    }
    void AddFace( size_t vi0, size_t vi1, size_t face ) {
      if( vi0 < vi1 ) {
        assert( f0 == ~0 );
        f0 = face;
      } else {
        assert( f1 == ~0 );
        f1 = face;
      }
    }
    size_t v0, v1;
    size_t f0, f1;
    float crease;
  };
  bool operator<( const Edge & a, const Edge & b ) {
    return a.v0 < b.v0 || ( ( a.v0 == b.v0 ) && ( a.v1 < b.v1 ) );
  }
  
  struct Face {
    vector<size_t> vertIndex;
    vector<size_t> edgeIndex;
  };
  
  struct Topo {
    vector<Vertex> vert;
    vector<Face> face;
    vector<Edge> edge;
    map<Edge,size_t> edgeMap;
    Edge * FindEdge( size_t v0, size_t v1 ) {
      Edge e( v0, v1, 0 );
      map<Edge,size_t>::iterator i = edgeMap.find( e );
      if( i != edgeMap.end() ) {
        return &edge[ i->second ];
      }
      return NULL;
    }
  };
  
  struct Model {
    Model() : prev(NULL), next(NULL), level(0) {}
    ~Model() {
      if( next != 0 ) {
        delete next;
      }
    }
    vector<Vec3f> vpos;
    vector<Vec3f> vnrm;
    vector<Vec3f> fnrm;
    Topo topo;
    Model *prev;
    Model *next;
    size_t level;
  };
  
  
  //

  void compute_normals( Model & m ) {
    m.fnrm.resize( m.topo.face.size() );
    for( size_t i = 0; i < m.fnrm.size(); i++ ) {
      Face & f = m.topo.face[i];
      Vec3f & v0 = m.vpos[ f.vertIndex[0] ];
      Vec3f n(0,0,0);
      for( size_t j = 1; j < f.vertIndex.size() - 1; j++ ) {
        Vec3f & v1 = m.vpos[ f.vertIndex[j] ];
        Vec3f & v2 = m.vpos[ f.vertIndex[j+1] ];
        Vec3f tn = (v1-v0).Cross( v2-v0 );
        tn.Normalize();
        n+= tn;
      }
      n.Normalize();
      m.fnrm[ i ] = n;
    }
    
    m.vnrm.resize( m.vpos.size() );
    
    for( size_t i = 0; i < m.vnrm.size(); i++ ) {
      Vertex & v = m.topo.vert[ i ];
      size_t faces = v.faceIndex.size();
      Vec3f n(0,0,0);
      for( size_t j = 0; j < faces; j++ ) {
        n += m.fnrm[ v.faceIndex[ j ] ];
      }
      n.Normalize();
      m.vnrm[ i ] = n;
    }
  }
  
  void average( Model & m ) {
    if( m.prev == NULL ) {
      return;
    }
    Model & prev = *m.prev;
    size_t pv = prev.topo.vert.size(); // previous verts
    size_t pf = prev.topo.face.size(); // previous faces
    size_t pe = prev.topo.edge.size(); // previous edges

    m.vpos.resize( m.topo.vert.size() );
    
    // per-face verts
    for( size_t i = 0; i < pf; i++ ) {
      Vec3f p( 0, 0, 0);
      Face & f = prev.topo.face[i];
      size_t fv = f.vertIndex.size();
      for( size_t j = 0; j < fv; j++ ) {
        p += prev.vpos[ f.vertIndex[ j ] ];
      }
      p /= float( fv );
      m.vpos[ pv + i ] = p;
    }

    // per-edge verts
    for( size_t i = 0; i < pe; i++ ) {
      Edge & e = prev.topo.edge[ i ];
      Vec3f p = prev.vpos[ e.v0 ];
      size_t count = 1;
      p += prev.vpos[ e.v1 ];
      count++;
      if( e.crease == 0.0f ) {
        if( e.f0 != ~0 ) {
          p += m.vpos[ pv + e.f0 ];
          count++;
        }
        if( e.f1 != ~0 ) {
          p += m.vpos[ pv + e.f1 ];
          count++;
        }
      }
      p /= count;
      m.vpos[ pv + pf + i ] = p;
    }
    
    // original verts
    for( size_t i = 0; i < pv; i++ ) {
      Vertex & ov = prev.topo.vert[i];
      size_t valence = ov.edgeIndex.size();
      Vec3f fp(0, 0, 0);
      Vec3f rp(0, 0, 0);
      bool creased = false;
      for( size_t j = 0; j < valence; j++ ) {
        Edge & e = prev.topo.edge[ ov.edgeIndex[j] ];
        rp += ( prev.vpos[ e.v0 ] + prev.vpos[ e.v1 ] ) / 2.0f;
        if( e.crease > 0.0f ) {
          creased = true;
        }
      }
      if( ! creased ) {
        rp /= valence;
        for( size_t j = 0; j < ov.faceIndex.size(); j++ ) {
          fp += m.vpos[ pv + ov.faceIndex[j] ];
        }
        fp /= ov.faceIndex.size();
        Vec3f p = fp + rp * 2.0f + prev.vpos[i] * float( valence - 3 );
        p /= valence;
        m.vpos[i] = p;
      } else {
        m.vpos[i] = prev.vpos[i];
      }
    }
    
    
  }

  void derive_topo_from_face_verts( Topo & t ) {
    map<Edge, size_t> & em = t.edgeMap;
    size_t maxvert = 0;
    for( int i = 0; i < t.face.size(); i++ ) {
      Face &f = t.face[i];
      assert( f.vertIndex.size() > 2 );
      for( int j = 0; j < f.vertIndex.size(); j++ ) {
        size_t j0 = f.vertIndex[ j ];
        size_t j1 = f.vertIndex[ ( j + 1 ) % f.vertIndex.size() ];
        Edge e( j0, j1, i );
        size_t eidx = ~0;
        if( em.count( e ) != 0 ) {
          eidx = em[e];
          Edge & ee = t.edge[ eidx ];
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
      Face &f = t.face[i];
      for( int j = 0; j < f.vertIndex.size(); j++ ) {
        t.vert[ f.vertIndex[ j ] ].faceIndex.push_back( i );
      }
    }
    for( int i = 0; i < t.edge.size(); i++ ) {
      Edge &e = t.edge[i];
      assert( e.v0 >= 0 );
      assert( e.v1 >= 0 );
      t.vert[ e.v0 ].edgeIndex.push_back( i );
      t.vert[ e.v1 ].edgeIndex.push_back( i );
    }
  }


  void split_model( Model & m ) {
    delete m.next;
    m.next = new Model();
    m.next->prev = &m;
    m.next->level = m.level + 1;
    Model &r = *m.next;
    size_t pv = m.topo.vert.size(); // previous verts
    size_t pf = m.topo.face.size(); // previous faces
    size_t pe = m.topo.edge.size(); // previous faces
    
    // populate faces
    size_t fb = pv;        // base offset for newly added per-face vertexes
    size_t eb = pv + pf;   // base offset for newly added per-edge vertexes

    // refined mesh faces
    for( size_t i = 0; i < pf; i++ ) {
      Face &f = m.topo.face[i];
      size_t fv = f.vertIndex.size();
      if( fv == 4 ) { // ordinary
        Face rf;
        // quad 00
        rf.vertIndex.push_back( f.vertIndex[0] );
        rf.vertIndex.push_back( eb + f.edgeIndex[0] );
        rf.vertIndex.push_back( fb + i );
        rf.vertIndex.push_back( eb + f.edgeIndex[3] );
        r.topo.face.push_back( rf );
        // quad 01
        rf = Face();
        rf.vertIndex.push_back( eb + f.edgeIndex[0] );
        rf.vertIndex.push_back( f.vertIndex[1] );
        rf.vertIndex.push_back( eb + f.edgeIndex[1] );
        rf.vertIndex.push_back( fb + i );
        r.topo.face.push_back( rf );
        // quad 11
        rf = Face();
        rf.vertIndex.push_back( fb + i );
        rf.vertIndex.push_back( eb + f.edgeIndex[1] );
        rf.vertIndex.push_back( f.vertIndex[2] );
        rf.vertIndex.push_back( eb + f.edgeIndex[2] );
        r.topo.face.push_back( rf );
        // quad 10
        rf = Face();
        rf.vertIndex.push_back( eb + f.edgeIndex[3] );
        rf.vertIndex.push_back( fb + i );
        rf.vertIndex.push_back( eb + f.edgeIndex[2] );
        rf.vertIndex.push_back( f.vertIndex[3] );
        r.topo.face.push_back( rf );
      } else { // extra-ordinary
        for( size_t j = 0; j < fv; j++ ) {
          size_t e0 = f.edgeIndex[j];
          size_t e1 = f.edgeIndex[ (j + 1) % fv ];
          Face rf;
          rf.vertIndex.push_back( f.vertIndex[j] );
          rf.vertIndex.push_back( eb + e0 );
          rf.vertIndex.push_back( fb + i );
          rf.vertIndex.push_back( eb + e1 );
          r.topo.face.push_back( rf );
        }
      }
    }
    derive_topo_from_face_verts( r.topo );    
 
    for( size_t i = 0; i < pe; i++ ) {
      Vertex & v = r.topo.vert[ eb + i ];
      size_t idx[2];
      size_t children[2];
      int curr_idx = 0;
      for( size_t j = 0; j < v.edgeIndex.size() && curr_idx < 2; j++ ) {
        Edge & e = r.topo.edge[ v.edgeIndex[ j ] ];
        if( e.v0 < pv ) {
          idx[ curr_idx ] = e.v0;
          children[ curr_idx ] = v.edgeIndex[j];
          curr_idx++;
        }
      }
      assert( curr_idx == 2 );
      Edge e( idx[0], idx[1], 0 );
      assert( m.topo.edgeMap.count( e ) > 0 );
      size_t parent = m.topo.edgeMap[ e ];
      Edge &pe = m.topo.edge[ parent ];
      float crease = std::max( 0.0f, pe.crease - 1.0f);
      Edge &e0 = r.topo.edge[ children[ 0 ] ];
      Edge &e1 = r.topo.edge[ children[ 1 ] ];
      e0.crease = e1.crease = crease;
    }

  }
  
  void subdivide_model( Model & m ) {
    split_model( m );
    if( m.next != NULL ) {
      average( *m.next );
      compute_normals( *m.next );
    }
  }

}

subdiv::Model *model;


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
  
  derive_topo_from_face_verts( m.topo );
  compute_normals( m );
  subdiv::Edge *ep = m.topo.FindEdge( 0, 1 );
  assert( ep );
  ep->crease = 3.0;
  ep = m.topo.FindEdge( 2, 3 );
  assert( ep );
  ep->crease = 3.0;
}


int width, height;
bool b[256];

int mousebtn = -1;
r3::Vec2f mousepos;
r3::Vec3f trans( 0, 0, -3 );
r3::Rotationf rot;



void draw_model( subdiv::Model & m ) {
  glPolygonOffset( 1, 1 );
  glEnable( GL_POLYGON_OFFSET_FILL );
  glEnable( GL_COLOR_MATERIAL );
  glEnable( GL_LIGHT0 );
  glEnable( GL_LIGHTING );
  glColor3f( 0, 0, 1 );
  for( int i = 0; i < m.topo.face.size(); i++ ) {
    subdiv::Face &f = m.topo.face[i];
    glBegin( GL_TRIANGLE_FAN );
    for( int j = 0; j < f.vertIndex.size(); j++) {
      size_t vi = f.vertIndex[j];
      r3::Vec3f c = m.vnrm[ vi ];
      c *= 0.5;
      c += 0.5;
      //glColor3fv( c.Ptr() );
      glNormal3fv( m.vnrm[ vi ].Ptr() );
      glVertex3fv( m.vpos[ vi ].Ptr() );
    }
    glEnd();
  }
  glDisable( GL_LIGHTING );
  glDisable( GL_POLYGON_OFFSET_FILL );

  if( b['w'] ) {
    glColor3f( 0.4, 0.4, 0.4 );
    glBegin( GL_LINES );
    for( int i = 0; i < m.topo.edge.size(); i++ ) {
      subdiv::Edge &e = m.topo.edge[i];
      glVertex3fv( m.vpos[ e.v0 ].Ptr() );
      glVertex3fv( m.vpos[ e.v1 ].Ptr() );
    }
    glEnd();
  }
  
  if( b['p'] ) {
    glColor3f( .5, .5, 0 );
    glPointSize( 3 );
    glBegin( GL_POINTS );
    for( int i = 0; i < m.vpos.size(); i++ ) {
      glVertex3fv( m.vpos[ i ].Ptr() );
    }
    glEnd();
  }
  
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
  
  glClearColor( 0.5, 0.25, .25, 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  

  glMatrixPushEXT( GL_MODELVIEW );
  glMatrixTranslatefEXT( GL_MODELVIEW, trans.x, trans.y, trans.z );
  r3::Vec3f axis;
  float angle;
  rot.GetValue( axis, angle );
  glMatrixRotatefEXT( GL_MODELVIEW, r3::ToDegrees( angle ), axis.x, axis.y, axis.z );
  draw_model( *model );
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
    case 'c':
      if( model->level > 0 ) {
        model = model->prev;
      }
      printf( "Model level = %d\n", (int)model->level );
      break;
    case 'f':
      if( model->next == NULL && model->level < 6 ) {
        subdiv::subdivide_model( *model );
      }
      if( model->next ) {
        model = model->next;
      }
      printf( "Model level = %d\n", (int)model->level );
      break;
    case 's':
      subdiv::subdivide_model( *model );
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
  
  model = new subdiv::Model();
  build_subdiv_cube( *model );
  
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  
  return 0;
}

