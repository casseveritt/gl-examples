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

#if 0 && __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/GLUT.h>
#else
#  include <GL/Regal.h>
#  include <GL/RegalGLUT.h>
#endif

#include <string>
#include <vector>
#include <map>

using namespace std;


struct GlslType {
  GLenum numericType;
  enum BaseType {
    Variable, Sampler, Image
  };
  BaseType baseType;
  
};

const char * TypeToName( GLenum type ) {
  const char * base = NULL;
  
  
  switch( type ) {
    case GL_FLOAT: return "GL_FLOAT";
    case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
    case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
    case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
    case GL_DOUBLE: return "GL_DOUBLE";
    case GL_DOUBLE_VEC2: return "GL_DOUBLE_VEC2";
    case GL_DOUBLE_VEC3: return "GL_DOUBLE_VEC3";
    case GL_DOUBLE_VEC4: return "GL_DOUBLE_VEC4";
    case GL_INT: return "GL_INT";
    case GL_INT_VEC2: return "GL_INT_VEC2";
    case GL_INT_VEC3: return "GL_INT_VEC3";
    case GL_INT_VEC4: return "GL_INT_VEC4";
    case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
    case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
    case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
    case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
    case GL_BOOL: return "GL_BOOL";
    case GL_BOOL_VEC2: return "GL_BOOL_VEC2";
    case GL_BOOL_VEC3: return "GL_BOOL_VEC3";
    case GL_BOOL_VEC4: return "GL_BOOL_VEC4";
    case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
    case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
    case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
    case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
    case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
    case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
    case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
    case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
    case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";
    case GL_DOUBLE_MAT2: return "GL_DOUBLE_MAT2";
    case GL_DOUBLE_MAT3: return "GL_DOUBLE_MAT3";
    case GL_DOUBLE_MAT4: return "GL_DOUBLE_MAT4";
    case GL_DOUBLE_MAT2x3: return "GL_DOUBLE_MAT2x3";
    case GL_DOUBLE_MAT2x4: return "GL_DOUBLE_MAT2x4";
    case GL_DOUBLE_MAT3x2: return "GL_DOUBLE_MAT3x2";
    case GL_DOUBLE_MAT3x4: return "GL_DOUBLE_MAT3x4";
    case GL_DOUBLE_MAT4x2: return "GL_DOUBLE_MAT4x2";
    case GL_DOUBLE_MAT4x3: return "GL_DOUBLE_MAT4x3";
    case GL_SAMPLER_1D: return "GL_SAMPLER_1D";
    case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
    case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
    case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE";
    case GL_SAMPLER_1D_SHADOW: return "GL_SAMPLER_1D_SHADOW";
    case GL_SAMPLER_2D_SHADOW: return "GL_SAMPLER_2D_SHADOW";
    case GL_SAMPLER_1D_ARRAY: return "GL_SAMPLER_1D_ARRAY";
    case GL_SAMPLER_2D_ARRAY: return "GL_SAMPLER_2D_ARRAY";
    case GL_SAMPLER_1D_ARRAY_SHADOW: return "GL_SAMPLER_1D_ARRAY_SHADOW";
    case GL_SAMPLER_2D_ARRAY_SHADOW: return "GL_SAMPLER_2D_ARRAY_SHADOW";
    case GL_SAMPLER_2D_MULTISAMPLE: return "GL_SAMPLER_2D_MULTISAMPLE";
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
    case GL_SAMPLER_CUBE_SHADOW: return "GL_SAMPLER_CUBE_SHADOW";
    case GL_SAMPLER_BUFFER: return "GL_SAMPLER_BUFFER";
    case GL_SAMPLER_2D_RECT: return "GL_SAMPLER_2D_RECT";
    case GL_SAMPLER_2D_RECT_SHADOW: return "GL_SAMPLER_2D_RECT_SHADOW";
    case GL_INT_SAMPLER_1D: return "GL_INT_SAMPLER_1D";
    case GL_INT_SAMPLER_2D: return "GL_INT_SAMPLER_2D";
    case GL_INT_SAMPLER_3D: return "GL_INT_SAMPLER_3D";
    case GL_INT_SAMPLER_CUBE: return "GL_INT_SAMPLER_CUBE";
    case GL_INT_SAMPLER_1D_ARRAY: return "GL_INT_SAMPLER_1D_ARRAY";
    case GL_INT_SAMPLER_2D_ARRAY: return "GL_INT_SAMPLER_2D_ARRAY";
    case GL_INT_SAMPLER_2D_MULTISAMPLE: return "GL_INT_SAMPLER_2D_MULTISAMPLE";
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
    case GL_INT_SAMPLER_BUFFER: return "GL_INT_SAMPLER_BUFFER";
    case GL_INT_SAMPLER_2D_RECT: return "GL_INT_SAMPLER_2D_RECT";
    case GL_UNSIGNED_INT_SAMPLER_1D: return "GL_UNSIGNED_INT_SAMPLER_1D";
    case GL_UNSIGNED_INT_SAMPLER_2D: return "GL_UNSIGNED_INT_SAMPLER_2D";
    case GL_UNSIGNED_INT_SAMPLER_3D: return "GL_UNSIGNED_INT_SAMPLER_3D";
    case GL_UNSIGNED_INT_SAMPLER_CUBE: return "GL_UNSIGNED_INT_SAMPLER_CUBE";
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
    case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "GL_UNSIGNED_INT_SAMPLER_BUFFER";
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
    case GL_IMAGE_1D: return "GL_IMAGE_1D";
    case GL_IMAGE_2D: return "GL_IMAGE_2D";
    case GL_IMAGE_3D: return "GL_IMAGE_3D";
    case GL_IMAGE_2D_RECT: return "GL_IMAGE_2D_RECT";
    case GL_IMAGE_CUBE: return "GL_IMAGE_CUBE";
    case GL_IMAGE_BUFFER: return "GL_IMAGE_BUFFER";
    case GL_IMAGE_1D_ARRAY: return "GL_IMAGE_1D_ARRAY";
    case GL_IMAGE_2D_ARRAY: return "GL_IMAGE_2D_ARRAY";
    case GL_IMAGE_2D_MULTISAMPLE: return "GL_IMAGE_2D_MULTISAMPLE";
    case GL_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_IMAGE_2D_MULTISAMPLE_ARRAY";
    case GL_INT_IMAGE_1D: return "GL_INT_IMAGE_1D";
    case GL_INT_IMAGE_2D: return "GL_INT_IMAGE_2D";
    case GL_INT_IMAGE_3D: return "GL_INT_IMAGE_3D";
    case GL_INT_IMAGE_2D_RECT: return "GL_INT_IMAGE_2D_RECT";
    case GL_INT_IMAGE_CUBE: return "GL_INT_IMAGE_CUBE";
    case GL_INT_IMAGE_BUFFER: return "GL_INT_IMAGE_BUFFER";
    case GL_INT_IMAGE_1D_ARRAY: return "GL_INT_IMAGE_1D_ARRAY";
    case GL_INT_IMAGE_2D_ARRAY: return "GL_INT_IMAGE_2D_ARRAY";
    case GL_INT_IMAGE_2D_MULTISAMPLE: return "GL_INT_IMAGE_2D_MULTISAMPLE";
    case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY";
    case GL_UNSIGNED_INT_IMAGE_1D: return "GL_UNSIGNED_INT_IMAGE_1D";
    case GL_UNSIGNED_INT_IMAGE_2D: return "GL_UNSIGNED_INT_IMAGE_2D";
    case GL_UNSIGNED_INT_IMAGE_3D: return "GL_UNSIGNED_INT_IMAGE_3D";
    case GL_UNSIGNED_INT_IMAGE_2D_RECT: return "GL_UNSIGNED_INT_IMAGE_2D_RECT";
    case GL_UNSIGNED_INT_IMAGE_CUBE: return "GL_UNSIGNED_INT_IMAGE_CUBE";
    case GL_UNSIGNED_INT_IMAGE_BUFFER: return "GL_UNSIGNED_INT_IMAGE_BUFFER";
    case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: return "GL_UNSIGNED_INT_IMAGE_1D_ARRAY";
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: return "GL_UNSIGNED_INT_IMAGE_2D_ARRAY";
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE";
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY";
    case GL_UNSIGNED_INT_ATOMIC_COUNTER: return "GL_UNSIGNED_INT_ATOMIC_COUNTER";

    default:
      break;
  }

  return "<unknown-type>";
}

const char * ShaderTypeToName( GLenum type ) {
  switch( type ) {
    case GL_VERTEX_SHADER:
      return "GL_VERTEX_SHADER";
      break;
    case GL_FRAGMENT_SHADER:
      return "GL_FRAGMENT_SHADER";
      break;
    default:
      break;
  }
  return "<unknown-shader-type>";
}

enum UniformApiType {
  UAT_Invalid, UAT_Double, UAT_Float, UAT_Int, UAT_UnsignedInt, UAT_DoubleMatrix, UAT_FloatMatrix
};

UniformApiType GetUniformApiType( GLenum type ) {
  switch( type ) {
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
      return UAT_Float;
    case GL_DOUBLE:
    case GL_DOUBLE_VEC2:
    case GL_DOUBLE_VEC3:
    case GL_DOUBLE_VEC4:
      return UAT_Double;
    case GL_INT:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:
      return UAT_Int;
    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4:
      return UAT_UnsignedInt;
    case GL_BOOL:
    case GL_BOOL_VEC2:
    case GL_BOOL_VEC3:
    case GL_BOOL_VEC4:
      return UAT_Int;
    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT4:
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT2x4:
    case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3x4:
    case GL_FLOAT_MAT4x2:
    case GL_FLOAT_MAT4x3:
      return UAT_FloatMatrix;
    case GL_DOUBLE_MAT2:
    case GL_DOUBLE_MAT3:
    case GL_DOUBLE_MAT4:
    case GL_DOUBLE_MAT2x3:
    case GL_DOUBLE_MAT2x4:
    case GL_DOUBLE_MAT3x2:
    case GL_DOUBLE_MAT3x4:
    case GL_DOUBLE_MAT4x2:
    case GL_DOUBLE_MAT4x3:
      return UAT_DoubleMatrix;
    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    case GL_IMAGE_1D:
    case GL_IMAGE_2D:
    case GL_IMAGE_3D:
    case GL_IMAGE_2D_RECT:
    case GL_IMAGE_CUBE:
    case GL_IMAGE_BUFFER:
    case GL_IMAGE_1D_ARRAY:
    case GL_IMAGE_2D_ARRAY:
    case GL_IMAGE_2D_MULTISAMPLE:
    case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_INT_IMAGE_1D:
    case GL_INT_IMAGE_2D:
    case GL_INT_IMAGE_3D:
    case GL_INT_IMAGE_2D_RECT:
    case GL_INT_IMAGE_CUBE:
    case GL_INT_IMAGE_BUFFER:
    case GL_INT_IMAGE_1D_ARRAY:
    case GL_INT_IMAGE_2D_ARRAY:
    case GL_INT_IMAGE_2D_MULTISAMPLE:
    case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_1D:
    case GL_UNSIGNED_INT_IMAGE_2D:
    case GL_UNSIGNED_INT_IMAGE_3D:
    case GL_UNSIGNED_INT_IMAGE_2D_RECT:
    case GL_UNSIGNED_INT_IMAGE_CUBE:
    case GL_UNSIGNED_INT_IMAGE_BUFFER:
    case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_ATOMIC_COUNTER:
      return UAT_Int;

    default: break;
  }
  return UAT_Invalid;
}

struct MatrixDims {
  MatrixDims( int r, int c ) : rows( r ), cols( c ) {}
  MatrixDims( const MatrixDims & rhs ) : rows( rhs.rows ), cols( rhs.cols ) {}
  int rows;
  int cols;
};

MatrixDims GetMatrixDims( GLenum type ) {
  switch( type ) {
    case GL_DOUBLE_MAT2: case GL_FLOAT_MAT2: return MatrixDims( 2, 2 );
    case GL_DOUBLE_MAT3: case GL_FLOAT_MAT3: return MatrixDims( 3, 3 );
    case GL_DOUBLE_MAT4: case GL_FLOAT_MAT4: return MatrixDims( 4, 4 );
    case GL_DOUBLE_MAT2x3: case GL_FLOAT_MAT2x3: return MatrixDims( 2, 3 );
    case GL_DOUBLE_MAT2x4: case GL_FLOAT_MAT2x4: return MatrixDims( 2, 4 );
    case GL_DOUBLE_MAT3x2: case GL_FLOAT_MAT3x2: return MatrixDims( 3, 2 );
    case GL_DOUBLE_MAT3x4: case GL_FLOAT_MAT3x4: return MatrixDims( 3, 4 );
    case GL_DOUBLE_MAT4x2: case GL_FLOAT_MAT4x2: return MatrixDims( 4, 2 );
    case GL_DOUBLE_MAT4x3: case GL_FLOAT_MAT4x3: return MatrixDims( 4, 3 );
    default: break;
  }
  return MatrixDims( 0, 0 );
}

int GetMatrixElementSize( GLenum type ) {
  switch( type ) {
    case GL_FLOAT_MAT2: case GL_FLOAT_MAT3: case GL_FLOAT_MAT4:
    case GL_FLOAT_MAT2x3: case GL_FLOAT_MAT2x4: case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3x4:  case GL_FLOAT_MAT4x2: case GL_FLOAT_MAT4x3:
      return sizeof( GLfloat );
    case GL_DOUBLE_MAT2: case GL_DOUBLE_MAT3: case GL_DOUBLE_MAT4:
    case GL_DOUBLE_MAT2x3: case GL_DOUBLE_MAT2x4: case GL_DOUBLE_MAT3x2:
    case GL_DOUBLE_MAT3x4: case GL_DOUBLE_MAT4x2: case GL_DOUBLE_MAT4x3:
      return sizeof( GLdouble );
    default: break;
  }
  return 0;
}

int GetTypeSize( GLenum type ) {
  switch( type ) {
    case GL_FLOAT:
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_BOOL:
      return 4;
    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_BOOL_VEC2:
      return 4 * 2;
    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
    case GL_BOOL_VEC3:
      return 4 * 3;
    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
    case GL_BOOL_VEC4:
      return 4 * 4;
    case GL_DOUBLE: return 8;
    case GL_DOUBLE_VEC2: return 8 * 2;
    case GL_DOUBLE_VEC3: return 8 * 3;
    case GL_DOUBLE_VEC4: return 8 * 4;

    case GL_FLOAT_MAT2: case GL_FLOAT_MAT3: case GL_FLOAT_MAT4:
    case GL_FLOAT_MAT2x3:case GL_FLOAT_MAT2x4: case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3x4: case GL_FLOAT_MAT4x2: case GL_FLOAT_MAT4x3:
    case GL_DOUBLE_MAT2: case GL_DOUBLE_MAT3: case GL_DOUBLE_MAT4:
    case GL_DOUBLE_MAT2x3: case GL_DOUBLE_MAT2x4: case GL_DOUBLE_MAT3x2:
    case GL_DOUBLE_MAT3x4: case GL_DOUBLE_MAT4x2: case GL_DOUBLE_MAT4x3: {
      MatrixDims md = GetMatrixDims( type );
      return md.rows * md.cols * GetMatrixElementSize( type );
    }
      
    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    case GL_IMAGE_1D:
    case GL_IMAGE_2D:
    case GL_IMAGE_3D:
    case GL_IMAGE_2D_RECT:
    case GL_IMAGE_CUBE:
    case GL_IMAGE_BUFFER:
    case GL_IMAGE_1D_ARRAY:
    case GL_IMAGE_2D_ARRAY:
    case GL_IMAGE_2D_MULTISAMPLE:
    case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_INT_IMAGE_1D:
    case GL_INT_IMAGE_2D:
    case GL_INT_IMAGE_3D:
    case GL_INT_IMAGE_2D_RECT:
    case GL_INT_IMAGE_CUBE:
    case GL_INT_IMAGE_BUFFER:
    case GL_INT_IMAGE_1D_ARRAY:
    case GL_INT_IMAGE_2D_ARRAY:
    case GL_INT_IMAGE_2D_MULTISAMPLE:
    case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_1D:
    case GL_UNSIGNED_INT_IMAGE_2D:
    case GL_UNSIGNED_INT_IMAGE_3D:
    case GL_UNSIGNED_INT_IMAGE_2D_RECT:
    case GL_UNSIGNED_INT_IMAGE_CUBE:
    case GL_UNSIGNED_INT_IMAGE_BUFFER:
    case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_ATOMIC_COUNTER:
      return 4;
    default: break;
  }
  return 0;
}

void GetUniform( GLuint program, GLint location, GLsizei count, GLenum type, void *value ) {
  GLint size = GetTypeSize( type );
  UniformApiType uat = GetUniformApiType( type );
  char * val = reinterpret_cast<char *>( value );
  switch( uat ) {
    case UAT_Double:
    case UAT_DoubleMatrix: {
      for( int j = 0; j < count; j++ ) {
        glGetUniformdv( program, location + j, reinterpret_cast<GLdouble *>( val + j * size ) );
      }
    } break;
    case UAT_Float:
    case UAT_FloatMatrix: {
      for( int j = 0; j < count; j++ ) {
        glGetUniformfv( program, location + j, reinterpret_cast<GLfloat *>( val + j * size ) );
      }
    } break;
    case UAT_Int: {
      for( int j = 0; j < count; j++ ) {
        glGetUniformiv( program, location + j, reinterpret_cast<GLint *>( val + j * size ) );
      }
    } break;
    case UAT_UnsignedInt: {
      for( int j = 0; j < count; j++ ) {
        glGetUniformuiv( program, location + j, reinterpret_cast<GLuint *>( val + j * size ) );
      }
    } break;
    default:
      break;
    }
}

void SetUniform( GLint location, GLsizei count, GLenum type, const void *value ) {
  GLint size = GetTypeSize( type );
  UniformApiType uat = GetUniformApiType( type );
  const char * val = reinterpret_cast<const char *>( value );
  switch( uat ) {
    case UAT_Double: {
      typedef GLdouble Type;
      typedef void (*Uniform)( GLint, GLsizei, const Type *);
      Uniform uniformv[] = { glUniform1dv, glUniform2dv, glUniform3dv, glUniform4dv };
      int idx = ( size / sizeof(Type) ) - 1;
      uniformv[idx]( location, count, reinterpret_cast<const Type *>(val) );
    } break;
    case UAT_Float: {
      typedef GLfloat Type;
      typedef void (*Uniform)( GLint, GLsizei, const Type *);
      Uniform uniformv[] = { glUniform1fv, glUniform2fv, glUniform3fv, glUniform4fv };
      int idx = ( size / sizeof(Type) ) - 1;
      uniformv[idx]( location, count, reinterpret_cast<const Type *>(val) );
    } break;
    case UAT_Int: {
      typedef GLint Type;
      typedef void (*Uniform)( GLint, GLsizei, const Type *);
      Uniform uniformv[] = { glUniform1iv, glUniform2iv, glUniform3iv, glUniform4iv };
      int idx = ( size / sizeof(Type) ) - 1;
      uniformv[idx]( location, count, reinterpret_cast<const Type *>(val) );
    } break;
    case UAT_UnsignedInt: {
      typedef GLuint Type;
      typedef void (*Uniform)( GLint, GLsizei, const Type *);
      Uniform uniformv[] = { glUniform1uiv, glUniform2uiv, glUniform3uiv, glUniform4uiv };
      int idx = ( size / sizeof(Type) ) - 1;
      uniformv[idx]( location, count, reinterpret_cast<const Type *>(val) );
    } break;
    case UAT_DoubleMatrix: {
      typedef GLdouble Type;
      typedef void (*Uniform)( GLint, GLsizei, GLboolean, const Type *);
      Uniform uniformv[3][3] = {
        { glUniformMatrix2dv,   glUniformMatrix2x3dv, glUniformMatrix2x4dv },
        { glUniformMatrix3x2dv, glUniformMatrix3dv,   glUniformMatrix3x4dv },
        { glUniformMatrix4x2dv, glUniformMatrix4x3dv, glUniformMatrix4dv   }
      };
      MatrixDims md = GetMatrixDims( type );
      uniformv[ md.rows - 1 ][ md.cols - 1 ]( location, count, GL_TRUE, reinterpret_cast<const Type *>(val) );
    } break;
    case UAT_FloatMatrix: {
      typedef GLfloat Type;
      typedef void (*Uniform)( GLint, GLsizei, GLboolean, const Type *);
      Uniform uniformv[3][3] = {
        { glUniformMatrix2fv,   glUniformMatrix2x3fv, glUniformMatrix2x4fv },
        { glUniformMatrix3x2fv, glUniformMatrix3fv,   glUniformMatrix3x4fv },
        { glUniformMatrix4x2fv, glUniformMatrix4x3fv, glUniformMatrix4fv   }
      };
      MatrixDims md = GetMatrixDims( type );
      uniformv[ md.rows - 1 ][ md.cols - 1 ]( location, count, GL_TRUE, reinterpret_cast<const Type *>(val) );
    } break;
    default:
      break;
  }

}

struct UniformElement {
  string name;
  GLint loc;
  GLint count;
  GLenum type;
  int offset; // byte offset into a uniform store
  int ver;
};

struct ProgramUniforms {

  void AddElement( const string & name, GLint loc, GLint count, GLenum type ) {
    UniformElement ue;
    ue.name = name;
    ue.loc = loc;
    ue.count = count;
    ue.type = type;
    ue.offset = int( store.size() * sizeof( GLuint ) );
    ue.ver = 0;
    // FIXME: deal with alignment
    int sz = GetTypeSize( ue.type );
    store.insert( store.end(), ( sz * ue.count ) / sizeof( GLint ), 0 );
    elements[ loc ] =  ue;
  }
  
  void UpdateStore( GLint location, const void * ptr ) {
    if( elements.find( location ) == elements.end() ) {
      return;
    }
    UniformElement &ue = elements[ location ];
    int sz = GetTypeSize( ue.type ) * ue.count;
    memcpy( reinterpret_cast<char *>(&store[0]) + ue.offset, ptr, sz );
    ue.ver++;
  }
  
  typedef map<GLint,UniformElement,greater<GLint> > Map;
  Map elements;
  vector<GLuint> store;
};

struct InstanceUniformElement {
  GLint programUniformLocation;
  GLint instanceUniformLocation;
  int ver;
};

struct ProgramInstanceUniforms {
  vector<InstanceUniformElement> uniforms;
  void Initialize( const ProgramUniforms & pu, GLuint prog ) {
    uniforms.clear();
    for( ProgramUniforms::Map::const_reverse_iterator it = pu.elements.crbegin(); it != pu.elements.crend(); ++it ) {
      const UniformElement & ue = it->second;
      InstanceUniformElement ie;
      ie.programUniformLocation = ue.loc;
      ie.instanceUniformLocation = glGetUniformLocation( prog, ue.name.c_str() );
      ie.ver = -1;
      uniforms.push_back( ie );
    }
  }
  void Update( const ProgramUniforms & pu ) {
   for( size_t i = 0; i < uniforms.size(); i++ ) {
      InstanceUniformElement & ie = uniforms[i];
      const UniformElement & ue = pu.elements.find( ie.programUniformLocation )->second;
      if( ie.ver != ue.ver ) {
        SetUniform( ie.instanceUniformLocation, ue.count, ue.type, reinterpret_cast<const char *>(&pu.store[0]) + ue.offset );
        ie.ver = ue.ver;
      }
    }
  }
};


GLuint InstanceShader( GLuint shader ) {
  GLint type = 0;
  glGetShaderiv( shader, GL_SHADER_TYPE, &type );
  GLuint s = glCreateShader( type );
  printf( "Instancing shader %d (%s) -> %d\n", shader, ShaderTypeToName(type), s );
  GLint sz = 0;
  glGetShaderiv( shader, GL_SHADER_SOURCE_LENGTH, &sz );
  GLchar *src = new GLchar[ sz + 1 ];
  GLsizei sz1 = 0;
  glGetShaderSource( shader, sz + 1, &sz1, src );
  GLchar *dumb[] =  { src, NULL };
  src[sz] = 0;
  //printf("Shader source (length = %d):\n%s\n", sz, src );
  GLsizei sizes[] = { sz, 0 };
  glShaderSource( s, 1, dumb, sizes );
  glCompileShader( s );
  char dbgLog[1<<15];
  int dbgLogLen = 0;
  glGetShaderInfoLog( s, (1<<15) - 2, &dbgLogLen, dbgLog );
  dbgLog[ dbgLogLen ] = 0;
  printf( "Shader Info Log %d:\n\n%s\n", s, dbgLog );

  delete [] src;
  return s;
}

GLuint InstanceProgram( GLuint program ) {
  GLuint p = glCreateProgram();
  printf( "Instancing program %d -> %d\n", program, p );
  
  GLuint shaders[5];
  GLsizei numShaders = 0;
  
  glGetAttachedShaders( program, 5, &numShaders, shaders );
  printf( "Prog %d has %d shaders { ", program, numShaders );
  for( int i = 0; i < numShaders; i++ ) {
    printf( "%d", shaders[i] );
    if( i != numShaders - 1 ) {
      printf(", ");
    }
  }
  printf( " }\n" );

  for( int i = 0; i < numShaders; i++ ) {
    GLuint s = InstanceShader( shaders[i] );
    glAttachShader( p, s );
  }

  glLinkProgram( p );
  
  GLint activeAttributes = 0;
  glGetProgramiv( program, GL_ACTIVE_ATTRIBUTES, &activeAttributes );
  printf( "Program %d has %d active attributes.\n", program, activeAttributes );
  for( int i = 0; i < activeAttributes; i++ ) {
    GLchar name[80];
    GLsizei nameLen = 0;
    GLint size;
    GLenum type;
    glGetActiveAttrib( program, i, 80, &nameLen, &size, &type, name );
    name[nameLen] = 0;
    GLint loc = glGetAttribLocation( program, name );
    printf("  %s loc = %d\n", name, loc );
    glBindAttribLocation( p, loc, name );
  }
  
  glLinkProgram( p );
  {
    char dbgLog[1<<15];
    int dbgLogLen = 0;
    glGetProgramInfoLog( p, (1<<15) - 2, &dbgLogLen, dbgLog );
    dbgLog[ dbgLogLen ] = 0;
    printf( "Program Info Log %d:\n\n %s\n", p, dbgLog );
  }

  GLint activeUniforms = 0;
  glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &activeUniforms );
  glGetError();
  printf( "Program %d has %d active uniforms.\n", program, activeUniforms );
  ProgramUniforms pu;
  for( int i = 0; i < activeUniforms; i++ ) {
    GLchar name[80];
    GLsizei nameLen = 0;
    GLint count;
    GLenum type;
    glGetActiveUniform( program, i, 80, &nameLen, &count, &type, name );
    name[nameLen] = 0;
    GLint loc = glGetUniformLocation( program, name );
    pu.AddElement( name, loc, count, type );
    char buf[4096];
    GetUniform( program, loc, count, type, buf );
    pu.UpdateStore( loc, buf );
  }
  
  ProgramInstanceUniforms pi;
  pi.Initialize( pu, p );
  glUseProgram( p );
  pi.Update( pu );
  glUseProgram( 0 );

  /*
  for( int i = 0; i < activeUniforms; i++ ) {
    GLchar name[80];
    GLsizei nameLen = 0;
    GLint count;
    GLenum type;
    glGetActiveUniform( program, i, 80, &nameLen, &count, &type, name );
    name[nameLen] = 0;
    printf( "  %s: type = %s, count = %d\n", name, TypeToName( type ), count );
    GLint loc = glGetUniformLocation( p, name );
    char buf[4096];
    GetUniform( program, loc, count, type, buf );
    SetUniform( loc, count, type, buf );
    GLint err = glGetError();
    if( err != GL_NO_ERROR ) {
      printf( "Got an error: %d.\n", err );
    }
  }
   */
  

  return p;
}
  


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

static void display() {
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
    "uniform vec4 myVec4;\n"
    "uniform float myFloat[8];\n"
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
    "  vec4 noop = vec4(myFloat[6]) * myVec4;\n"
    //"  noop = vec4( 1.0, 1.0, 1.0, 1.0 );\n"
    "  ocol = col * noop;\n"
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
  
  // set dummy uniforms to 1, so they pass through...
  glUseProgram( prog );
  GLfloat ones[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  glUniform4fv( glGetUniformLocation( prog, "myVec4" ), 1, ones);
  glUniform1fv( glGetUniformLocation( prog, "myFloat[0]" ), 8, ones );
  glUseProgram( 0 );
  
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

static void keyboard(unsigned char c, int x, int y) {
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
      break;
    case 'p':
      prog = InstanceProgram( prog );
      break;
    default:
      break;
  }
  glutPostRedisplay();
}


int main(int argc, const char * argv[]) {
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

