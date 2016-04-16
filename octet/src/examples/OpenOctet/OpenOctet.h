////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "Libraries.h"


//namespace octet {
  /// Scene containing a box with octet.
  class OpenOctet {
    // scene for drawing box
	  int NumVertices = 36;
	  int Index = 0;
	  typedef vec4 point4;
	  typedef vec4 color4;

	  point4 positions[8], vPositions[36];
	  color4 colors[8], vColors[36];

	  public:

		  OpenOctet(int argc, char **argv) { //: app(argc, argv) {
			  glutInit(&argc, argv);
			  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
			  glutInitWindowSize(512, 512);
			  glutCreateWindow("Color Cube");

			  //glewInit();
			  init();

			  glutDisplayFunc(display);  // the rendering callback
			  glutKeyboardFunc(keyboard); // a keyboard callback
			  glutMainLoop();

		  }

		  ~OpenOctet() {
		  }

	  void quad(int a, int b, int c, int d)
	  {
		  vColors[Index] = colors[a]; vPositions[Index] = positions[a]; Index++;
		  vColors[Index] = colors[b]; vPositions[Index] = positions[b]; Index++;
		  vColors[Index] = colors[c]; vPositions[Index] = positions[c]; Index++;
		  vColors[Index] = colors[a]; vPositions[Index] = positions[a]; Index++;
		  vColors[Index] = colors[c]; vPositions[Index] = positions[c]; Index++;
		  vColors[Index] = colors[d]; vPositions[Index] = positions[d]; Index++;
	  }

	  void colorcube()
	  {
		  quad(1, 0, 3, 2);
		  quad(2, 3, 7, 6);
		  quad(3, 0, 4, 7);
		  quad(6, 5, 1, 2);
		  quad(4, 5, 6, 7);
		  quad(5, 4, 0, 1);
	  }




	  void init()
	  {
		  positions[8] = { vec4(-0.5, -0.5,  0.5, 1.0),
			  vec4(-0.5,  0.5,  0.5, 1.0),
			  vec4(0.5,  0.5,  0.5, 1.0),
			  vec4(0.5, -0.5,  0.5, 1.0),
			  vec4(-0.5, -0.5, -0.5, 1.0),
			  vec4(-0.5,  0.5, -0.5, 1.0),
			  vec4(0.5,  0.5, -0.5, 1.0),
			  vec4(0.5, -0.5, -0.5, 1.0),
		  };

		  colors[8] = {
			  color4(0.0, 0.0, 0.0, 1.0),
			  color4(1.0, 0.0, 0.0, 1.0),
			  color4(1.0, 1.0, 0.0, 1.0),
			  color4(0.0, 1.0, 0.0, 1.0),
			  color4(0.0, 0.0, 1.0, 1.0),
			  color4(1.0, 0.0, 1.0, 1.0),
			  color4(1.0, 1.0, 1.0, 1.0),
			  color4(0.0, 1.0, 1.0, 1.0),
		  };

		  colorcube();



	  }


	  void display(void)
	  {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		  glutSwapBuffers();
	  }

	  void keyboard(unsigned char key, int x, int y)
	  {
		  switch (key) {
		  case 033: case 'q': case 'Q':
			  exit(EXIT_SUCCESS);
			  break;
		  }
	  }

	  int toRender(int argc, char **argv)
	  {
		  glutInit(&argc, argv);
		  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		  glutInitWindowSize(512, 512);
		  glutCreateWindow("Color Cube");

		  //glewInit();
		  init();

		  glutDisplayFunc(display);  // the rendering callback
		  glutKeyboardFunc(keyboard); // a keyboard callback
		  glutMainLoop();

		  return 0;
	  }
  };
//}
