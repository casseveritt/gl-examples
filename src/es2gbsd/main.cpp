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

// Code illustrating how to get around the lack of glGetBufferSubData() in OpenGL ES 2.0
// Cass Everitt - Aug 26, 2013

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#if __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/GLUT.h>
#else
#  include <GL/Regal.h>
#  include <GL/RegalGLUT.h>
#endif

int width, height;
int fbotex_sz = 256;
int fbotex_px_sz = 4;
GLuint fbotex = 1;
GLuint fbo = 1;
GLuint posVbo = 2;
GLuint colVbo = 3;
GLuint prog;
GLubyte pix[1<<24];
GLubyte bufdata[1<<24];
bool b[256];

int gbsdOffset = 12386821;
int gbsdSize = 4390395;

typedef unsigned char uchar;

void reshape( int w, int h ) {
  width = w;
  height = h;
  glViewport( 0, 0, width, height );
  glutPostRedisplay();
}

void drawBufferObject( int offset, int size ) {
  glClearColor( 1, 1, 1, 0 );
  glClear( GL_COLOR_BUFFER_BIT );
  glPointSize( 1.0 );
  
  glDisable( GL_BLEND );
  
  // pos
  glBindBuffer( GL_ARRAY_BUFFER, posVbo );
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 2, GL_UNSIGNED_BYTE, GL_TRUE, 0, NULL );
  // col
  glBindBuffer( GL_ARRAY_BUFFER, colVbo );
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, ((char *)NULL)+offset );
  
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  
  glUseProgram( prog );
  glDrawArrays( GL_POINTS, 0, ( size + 3 ) / 4  );
  glUseProgram( 0 );
  
  glDisableVertexAttribArray( 0 );
  glDisableVertexAttribArray( 1 );
}

void GetBufferSubData( int offset, int size ) {
  int O = offset & ~3;
  int S = ( size + (offset - O) + 3 ) & ~3;
  glBindTexture( GL_TEXTURE_2D, 0 );
  glBindFramebuffer( GL_FRAMEBUFFER, fbo );
  glViewport( 0, 0, fbotex_sz, fbotex_sz );
  int o = 0;
  while( o < S ) {
    int sz = std::min( int(1<<18), S - o );
    drawBufferObject( O + o, sz );
    int height = ( ( ( sz + 3 ) / 4 ) + fbotex_sz - 1 ) / fbotex_sz;
    glReadPixels( 0, 0, fbotex_sz, height, GL_RGBA, GL_UNSIGNED_BYTE, pix + o );
    o += sz;
  }
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glViewport( 0, 0, width, height );

  glBindBuffer( GL_ARRAY_BUFFER, colVbo );
  glGetBufferSubData( GL_ARRAY_BUFFER, O, S, bufdata );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  int sum = 0;
  for( int i = offset - O; i < size; i++ ) {
    int p = pix[i];
    int b = bufdata[i];
    int diff = p - b;
    sum += diff > 0 ? diff : -diff;
  }
  printf( "sum of diff between emu and gbsd for offset=%d and size=%d: %d\n", offset, size, sum );
}

static void display()
{
  if( b['f'] ) {
    drawBufferObject( 0, 256 * 256 * 4);
    glutSwapBuffers();
    return;
  }

  GetBufferSubData( gbsdOffset, gbsdSize );
  
  glClearColor( 0, 0, 1, 0 );
  glClear( GL_COLOR_BUFFER_BIT );
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture( GL_TEXTURE_2D, fbotex );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glColor3f( 1, 1, 1 );
  glBegin( GL_QUADS );
  glTexCoord2f(  0,  0 );
  glVertex2f  ( -1, -1 );
  glTexCoord2f(  1,  0 );
  glVertex2f  (  1, -1 );
  glTexCoord2f(  1,  1 );
  glVertex2f  (  1,  1 );
  glTexCoord2f(  0,  1 );
  glVertex2f  ( -1,  1 );
  glEnd();
  glBindTexture( GL_TEXTURE_2D, 0 );
  glDisable( GL_TEXTURE_2D );
  
  glutSwapBuffers();
}

static void init_opengl() {
  {
    glGenBuffers( 1, &posVbo );
    glBindBuffer( GL_ARRAY_BUFFER, posVbo );
    glBufferData( GL_ARRAY_BUFFER, 256 * 256 * 2, NULL, GL_STATIC_DRAW );
    GLubyte * row = new GLubyte[256 * 2];
    for( int j = 0 ; j < 256; j++ ) {
      GLubyte y = j;
      for( int i = 0; i < 256; i++ ) {
        GLubyte x = i;
        row[ i * 2 + 0 ] = x;
        row[ i * 2 + 1 ] = y;
      }
      glBufferSubData( GL_ARRAY_BUFFER, j * 256 * 2, 256 * 2, row );
    }
    delete [] row;
  }

  
  { // init color vbo with random data
    glGenBuffers( 1, &colVbo );
    glBindBuffer( GL_ARRAY_BUFFER, colVbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(bufdata), NULL, GL_DYNAMIC_DRAW );
    GLubyte col[1024];
    for( int j = 0; j < sizeof( bufdata ) / sizeof( col ); j++ ) {
      for( int i = 0; i < 1024; i++ ) {
        col[ i ] = GLubyte( rand() );
      }
      glBufferSubData( GL_ARRAY_BUFFER, j * sizeof( col ), sizeof( col ), col );
    }
  }
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  
  GLuint vs = glCreateShader( GL_VERTEX_SHADER );
  {
    char vshader[] =
    "attribute vec4 pos;\n"
    "attribute vec4 col;\n"
    "varying vec4 ocol;\n"
    "\n"
    "void main() {\n"
    "  // step 1: scale normalized to integer [0,255]\n"
    "  // step 2: scale / bias s.t. lower left corner of lower left pixel is [0,0]\n"
    "  //                 and upper right corner of upper right pixel is [512,512]\n"
    "  //                 and odd integers are at pixel centers\n"
    "  // step 3: bias so screen corners are at [-256,-256] and [256,256]\n"
    "  // step 4: scale so screen corners are [-1,-1] and [1,1]\n"
    "  // vec2 p = ( pos.xy * 255.0 * 2.0 + 1.0 - 256.0 ) / 256.0;\n"
    "  //\n"
    "  // Or reducing steps 1-4 into a single MAD\n"
    "  vec2 p = pos.xy * (255.0/128.0) - (255.0/256.0);\n"
    "  gl_Position = vec4( p.x, p.y, 0.0, 1.0 );\n"
    "  ocol = col;\n"
    "}\n";
    char *vshader_list[] = { vshader, NULL };
    int vshader_list_sizes[] = { sizeof( vshader ), 0 };
    glShaderSource( vs, 1, vshader_list, vshader_list_sizes );
    glCompileShader( vs );
    char dbgLog[1<<15];
    int dbgLogLen = 0;
    glGetShaderInfoLog( vs, (1<<15) - 2, &dbgLogLen, dbgLog );
    dbgLog[ dbgLogLen ] = 0;
    printf( "%s\n", dbgLog );
  }
  
  GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
  {
    char fshader[] =
    "varying vec4 ocol;\n"
    "\n"
    "void main() {\n"
    "  gl_FragColor = ocol;\n"
    "}\n";
    char *fshader_list[] = { fshader, NULL };
    int fshader_list_sizes[] = { sizeof( fshader ), 0 };
    glShaderSource( fs, 1, fshader_list, fshader_list_sizes );
    glCompileShader( fs );
    char dbgLog[1<<15];
    int dbgLogLen = 0;
    glGetShaderInfoLog( fs, (1<<15) - 2, &dbgLogLen, dbgLog );
    dbgLog[ dbgLogLen ] = 0;
    printf( "%s\n", dbgLog );
  }
  
  prog = glCreateProgram();
  glAttachShader( prog, vs );
  glAttachShader( prog, fs );

  glLinkProgram( prog );
  {
    char dbgLog[1<<15];
    int dbgLogLen = 0;
    glGetProgramInfoLog( prog, (1<<15) - 2, &dbgLogLen, dbgLog );
    dbgLog[ dbgLogLen ] = 0;
    printf( "%s\n", dbgLog );
  }
  
  glBindAttribLocation( prog, 0, "pos" );
  glBindAttribLocation( prog, 1, "col" );
  
  glLinkProgram( prog );
  {
    char dbgLog[1<<15];
    int dbgLogLen = 0;
    glGetProgramInfoLog( prog, (1<<15) - 2, &dbgLogLen, dbgLog );
    dbgLog[ dbgLogLen ] = 0;
    printf( "%s\n", dbgLog );
  }
  
  glBindTexture( GL_TEXTURE_2D, fbotex );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, fbotex_sz, fbotex_sz, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  
  glGenFramebuffers( 1, &fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, fbo );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex, 0 );
  GLenum fbostatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
  printf( "framebuffer %s\n", fbostatus == GL_FRAMEBUFFER_COMPLETE ? "complete" : "incomplete" );
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
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
    case 'o':
      gbsdOffset = rand() & ((1<<24)-1);
      gbsdSize = std::min( gbsdSize, int(1<<24)-gbsdOffset );
      break;
    case 's':
      gbsdSize = rand() & ((1<<25)-1);
      gbsdSize = std::min( gbsdSize, int(1<<24)-gbsdOffset );
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
  glutCreateWindow( "gbsd" );

  init_opengl();
  gbsdSize = std::min( gbsdSize, int(1<<24)-gbsdOffset );

  
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  
  return 0;
}

