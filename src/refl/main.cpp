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

int width, height;
float aspect = 1.0f;
bool b[256];

void reshape( int w, int h ) {
  width = w;
  height = h;
  aspect = float(w)/float(h);
  glViewport( 0, 0, width, height );
  glMatrixLoadIdentityEXT( GL_PROJECTION );
  glMatrixOrthoEXT( GL_PROJECTION, -0.1f, 1 * aspect, -0.1f, 1, -1, 1 );
  glutPostRedisplay();
}

static void display()
{
  
  glClearColor( 0, 0, 1, 0 );
  glClear( GL_COLOR_BUFFER_BIT );

  glPushMatrix();
  
  glColor3f( 1, 1, 1 );
  float angle = r3::ToRadians( -45 );
  float angleIncr = r3::ToRadians( 10 );
  glBegin( GL_LINES );
  for( int i = 0; i < 9; i++ ) {
    glVertex2f( 0, 0 );
    glVertex2f( cosf(angle), sinf(angle) );
    angle += angleIncr;
  }
  glEnd();
  
  glVertex2f  ( 0, 0 );
  glVertex2f  (  1, 0 );
  glVertex2f  (  1,  1 );
  glVertex2f  ( 0,  1 );

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
  glutCreateWindow( "gl example template" );

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

