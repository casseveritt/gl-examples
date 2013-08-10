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
#include <map>
using namespace std;

void reshape( int w, int h ) {
  glViewport( 0, 0, w, h );
}

static void display()
{
  glClearColor( 0, 0, 1, 0 );
  glClear( GL_COLOR_BUFFER_BIT );
  glutSwapBuffers();
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

  glutTimerFunc(16, tick, 0);
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard );
  glutMainLoop();
  return 0;
}
