#pragma once

//libraries lib
#pragma comment(lib, "../../../Libraries/lib/glui32.lib")
#pragma comment(lib, "../../../Libraries/lib/freeglut.lib")
#pragma comment(lib, "../../../Libraries/lib/glew32.lib")



//----------------------------------------------------------------------------------------------
//OpenGL, latest version 4.5
#include "../../../Libraries/include/GL/glew.h"
#include "../../../Libraries/include/GL/wglew.h"

/*
//Initialize glew after creating an opengl context (i.e., creating a window with glut or others)
//  (example: 'glutCreateWindow("my window");' would actually create the opengl context)
// Then you can proceed to initialize glew this way (don't forget the first line):

glewExperimental = GL_TRUE;
GLenum err = glewInit();
if (GLEW_OK != err)
{
fprintf(stderr, "Error initializing GLEW: %s\n",
glewGetErrorString(err));
}

*/
//------------------------------------------------------------------------------------------------


//OpenGL, GLES 3.2
//#include "../../Libraries/include/GLES/GLES3/gl32.h"
//#include "../../Libraries/include/GLES/GLES3/gl2ext.h"
//#include "../../Libraries/include/GLES/GLES3/gl3platform.h"

//window manager freeglut
#include "../../../Libraries/include/GL/freeglut.h"
#include "../../../Libraries/include/GL/freeglut_ext.h"
#define GLUI_FREEGLUT
#include "../../../Libraries/include/GL/glui.h"

//GLM  math for OpenGL
#define GLM_SWIZZLE
#include "../../../Libraries/include/glm/glm.hpp"
#include "../../../Libraries/include/glm/gtc/matrix_transform.hpp"
#include "../../../Libraries/include/glm/gtx/transform2.hpp"

//a math library i found
//#include "../../Libraries/include/mathlib/vector.h"


// Octet
//#include "../../Libraries/include/octet/src/octet.h"
//#include "../../Libraries/include/octet/src/math/math.h"


/*//Math Library Eigen
#include "../../Libraries/include/Eigen/Core"
#include "../../Libraries/include/Eigen/Dense"
using namespace Eigen;
using namespace std;
*/

// Math libraries from ~Angel
//#include "../../Libraries/include/vec.h"
//#include "../../Libraries/include/mat.h"


//CML math library
//#include "../../Libraries/include/cml/cml.h"
//#include "../../Libraries/include/cml/cml.h"
