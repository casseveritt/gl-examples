/*
 * Cass Everitt - Aug 10, 2013
*/

#include <GL/Regal.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/RegalGLUT.h>
#endif

#include <GL/RegalGLU.h>

#include <cstdlib>
#include <cstdio>
#include <map>
using namespace std;

int width, height;

void reshape( int w, int h ) {
  width = w;
  height = h;
  glViewport( 0, 0, width, height );
}

int fbotex_sz = 1024;
GLuint fbotex = 1;
GLuint fbo = 1;

static void render_fbo() {

  glBindTexture( GL_TEXTURE_2D, 0 );

  glViewport( 0, 0, fbotex_sz, fbotex_sz );
  glBindFramebuffer( GL_FRAMEBUFFER, fbo );
  glClearColor( 0, 1, 0, 0 );
  glClear( GL_COLOR_BUFFER_BIT );
  glColor3f( 0, 0, 0 );
  glPushMatrix();
  glTranslatef( -.4, -.4, 0 );
  glBegin( GL_TRIANGLES );
  glVertex2f( 0, 0 );
  glVertex2f( 1, 0 );
  glVertex2f( 0, 1 );
  glEnd();
  glPopMatrix();

  
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glViewport( 0, 0, width, height );


  //glGenerateTextureMipmapEXT( fbotex, GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, fbotex );
  glGenerateMipmap( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, 0 );

}

static void display()
{
  render_fbo();

  glClearColor( 0, 0, 1, 0 );
  glClear( GL_COLOR_BUFFER_BIT );

  glEnable(GL_TEXTURE_2D);
  glBindTexture( GL_TEXTURE_2D, fbotex );
  glColor3f( 1, 1, .5 );
  glBegin(GL_TRIANGLES);
  glTexCoord2f( 0, 0 );
  glVertex2f( 0, 0 );
  glTexCoord2f( 1, 0 );
  glVertex2f( 1, 0 );
  glTexCoord2f( 0, 1 );
  glVertex2f( 0, 1 );
  glEnd();
  glBindTexture( GL_TEXTURE_2D, 0 );
  glDisable( GL_TEXTURE_2D );

  glutSwapBuffers();
}

static void init_opengl() {
  glBindTexture( GL_TEXTURE_2D, fbotex );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, fbotex_sz, fbotex_sz, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

  glGenFramebuffers( 1, &fbo );
  glBindFramebuffer( GL_FRAMEBUFFER, fbo );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex, 0 );
  GLenum fbostatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
  printf( "framebuffer %s\n", fbostatus == GL_FRAMEBUFFER_COMPLETE ? "complete" : "incomplete" );
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

static void tick(int dummy)
{
  glutPostRedisplay();
  glutTimerFunc( 16, tick, 0 );
}

static void keyboard(unsigned char c, int x, int y)
{
  switch (c)
  {
    case 'q':
    case 27:  /* Esc key */
      exit(0);
      break;
  }
}

int main(int argc, const char *argv[])
{
  glutInitDisplayString("rgba>=8 depth double");
  glutInitWindowSize(500, 500);
  glutInit( &argc, (char **) argv);
  glutCreateWindow("readback");

  // Regal workaround for OSX GLUT

  #ifdef __APPLE__
  RegalMakeCurrent(CGLGetCurrentContext());
  #endif

  init_opengl();

  glutTimerFunc(16, tick, 0);
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  return 0;
}
