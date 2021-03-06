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

// How to read depth in ES2
// Cass Everitt - December 16, 2013

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include <GL/Regal.h>

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/RegalGLUT.h>
#endif

int width, height;
bool b[256];

void reshape( int w, int h ) {
  width = w;
  height = h;
  glViewport( 0, 0, width, height );
  glMatrixLoadIdentityEXT( GL_PROJECTION );
  float a = float(w)/float(h);
  float n = 0.4f;
  float f = 3.0f;
  if( a >= 1.0f ) {
    glMatrixFrustumEXT( GL_PROJECTION, -n*a, n*a, -n, n, n, f );
  } else {
    glMatrixFrustumEXT( GL_PROJECTION, -n, n, -n/a, n/a, n, f );
  }
  glMatrixLoadIdentityEXT( GL_MODELVIEW );
  glMatrixTranslatefEXT( GL_MODELVIEW, 0, 0, -1 );
  glutPostRedisplay();
}

struct Pos { GLfloat x, y, z; };


void show_depth() {
  
  float interval = 0.5;
  GLuint *pix = new GLuint[ width * height ];
  Pos *points = new Pos[ width * height ];
  for( int j = 0; j < height; j++ ) {
    for( int i = 0; i < width; i++ ) {
      Pos & p = points[ j * width + i ];
      p.x = i + 0.5;
      p.y = j + 0.5;
      p.z = interval;
    }
  }
  

  glClearColor( 0, 0, 0, 0 );
  glClear( GL_COLOR_BUFFER_BIT );
  glDepthMask( GL_FALSE );
  
  glMatrixPushEXT( GL_PROJECTION );
  glMatrixLoadIdentityEXT( GL_PROJECTION );
  glMatrixOrthoEXT( GL_PROJECTION, 0, width, 0, height, 0, -1 );
  glMatrixPushEXT( GL_MODELVIEW );
  glMatrixLoadIdentityEXT( GL_MODELVIEW );
  
  for( int iterations = 0; iterations < 24; iterations++ ) {

    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    
    glBegin( GL_POINTS );
    for( int j = 0; j < height; j++ ) {
      for( int i = 0; i < width; i++ ) {
        Pos & p = points[ j * width + i ];
        glColor3f( p.z, p.z, p.z );
        glVertex3fv( &p.x );
      }
    }
    glEnd();
    
    glReadPixels( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pix );
    
    interval /= 2.0;
    
    for( int i = 0; i < width * height; i++ ) {
      points[ i ].z += pix[i] ? interval : -interval;
    }
    
  }

  // draw every point - no clear needed
  glDisable( GL_DEPTH_TEST );
  glBegin( GL_POINTS );
  for( int j = 0; j < height; j++ ) {
    for( int i = 0; i < width; i++ ) {
      Pos & p = points[ j * width + i ];
      double z = p.z * ( (1<<24) - 1 );
      int iz = z;
      
      glColor4ubv( (GLubyte *)&iz );
      glVertex3fv( &p.x );
    }
  }
  glEnd();
  glEnable( GL_DEPTH_TEST );
  
  glMatrixPopEXT( GL_MODELVIEW );
  glMatrixPopEXT( GL_PROJECTION );

  glDepthMask( GL_TRUE );

  delete [] points;
  delete [] pix;
}


static void display()
{
  glClearColor( 0, 0, 1, 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  glColor3f( 1, 0.5, 0 );
  glPushMatrix();
  glScalef( 0.9, 0.9, 0.9 );
  glBegin( GL_QUADS );
  glVertex3f  ( -1, -0.1, 1 );
  glVertex3f  (  1, -0.1, 1 );
  glVertex3f  (  1,  1, -4 );
  glVertex3f  ( -1,  1, -4 );
  glEnd();
  glScalef( 1, -1, 1 );
  glColor3f( 0.5, 1, 0 );
  glBegin( GL_TRIANGLES );
  glVertex3f  ( -1, -1, -1.2 );
  glVertex3f  (  1, -1, -1 );
  glVertex3f  (  1,  1, -1.1 );
  glEnd();
  glPopMatrix();
  
  if( b['d'] ) {
    show_depth();
  }
  
  glutSwapBuffers();
}

static void init_opengl() {
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
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
  glutCreateWindow( "gl example template" );

#if __APPLE__
  RegalMakeCurrent( CGLGetCurrentContext() );
#endif
  
  init_opengl();
  
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  
  return 0;
}

