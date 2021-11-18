
#ifndef MAIN_H
#define MAIN_H

//#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib,"glu32.lib")
//#pragma comment(lib,"glut32.lib")
#pragma warning( disable: 4244 )	// disable warning of data conversion.
#pragma warning( disable: 4996 )	// disable warning of security.

#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::ostream;
using std::istream;
using std::ifstream;
using std::ofstream;
#include <iomanip>
using std::setprecision;
using std::setw;
#include <algorithm>
using std::min;
using std::max;
using std::swap;
using std::sort;
using std::fill;
using std::copy;
#include <vector>
using std::vector;
#include <list>
using std::list;
#include <set>
using std::set;
#include <map>
using std::map;
#include <deque>
using std::deque;
#include <queue>
using std::queue;
#include <cmath>
using std::sqrt;
using std::abs;
using std::exp;
using std::cos;
using std::sin;
using std::atan;
using std::atan2;
using std::log;
using std::exp;
using std::ceil;
using std::floor;
#include <ctime>
using std::clock;
using std::time;
using std::difftime;
#include <cstdlib>
#include <cassert>
#include <fstream>
using std::ostringstream;
using std::ios;
#include <sstream>
#include <iomanip>
#include <cstdio>
#ifdef WIN32
#include <atlimage.h> // To use CImage class
#endif
/*
#if defined(WIN32) || defined(linux)
  //#include <GL/gl.h>
  //#include <GL/glu.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#endif
*/

//------------------------------CONSTANTS---------------------------------------------
// User-specified data type
typedef float Scalar;

#ifndef INFINITY
#define INFINITY HUGE_VAL
#endif

const float HALF_PI = 2.0f * atan(1.0f);
//
//#if defined(WIN32)
//const float M_PI = 4.0f * atan(1.0f);
//#endif

const float TWO_PI = 8.0f * atan(1.0f);
const float INV_PI = 1.0f/(float)M_PI;
const float INV_TWO_PI = 1.0f/TWO_PI;
const float PI_INV180 = (float)M_PI   / 180.0f;
const float INV_PI180 = INV_PI * 180.0f;
const float ONE_THIRD = 1.0f/3.0f;
const float INV_255 = 1.0f / 255.0f;
const float SQRT_TWO = sqrt(2.0f);
const float	INV_SQRT_TWO = 1.0f / sqrt(2.0f);

#define EPS					1e-5
#define IS_ZERO(x)			(x < EPS && x > -EPS)

#define DELETE_OBJECT(p)	{if(NULL!=p){delete p;p = NULL;}}
#define DELETE_ARRAY(p)		{if(NULL!=p){delete[] p;p = NULL;}}

inline float Radians(const float &deg) {return PI_INV180 * deg; }
inline float Degrees(const float &rad) {return INV_PI180 * rad; }

#endif // MAIN_H
