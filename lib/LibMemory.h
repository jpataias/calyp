/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2015  by Luis Lucas      (luisfrlucas@gmail.com)
 *                                Joao Carreira   (jfmcarreira@gmail.com)
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * \file     LibMemory.cpp
 * \brief    Memory allocation functions
 */

#ifndef __LIBMEMORY_H__
#define __LIBMEMORY_H__

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace plaYUVer
{

typedef void Void;
typedef char Char;
typedef int Int;
typedef size_t SizeT;

#ifdef _MSC_VER
typedef __int64 Int64;
#if _MSC_VER <= 1200 // MS VC6
typedef __int64 UInt64;   // MS VC6 does not support unsigned __int64 to double conversion
#else
typedef unsigned __int64 UInt64;
#endif
#else
typedef long long Int64;
typedef unsigned long long UInt64;
#endif

static inline void* mem_malloc( size_t nitems )
{
  void *d;
  if( ( d = malloc( nitems ) ) == NULL )
  {
    return NULL;
  }
  return d;
}


static inline void mem_free( void *ptr )
{
  if( ptr != NULL )
  {
    free( ptr );
    ptr = NULL;
  }
}


static inline Void* xMallocMem( SizeT nitems )
{
  Void *d;
  if( ( d = malloc( nitems ) ) == NULL )
  {
    printf( "malloc failed.\n" );
    return NULL;
  }
  return d;
}

static inline Void* xCallocMem( SizeT nitems, SizeT size )
{
  size_t padded_size = nitems * size;
  Void *d = xMallocMem( padded_size );
  memset( d, 0, ( int )padded_size );
  return d;
}

static inline Void xFreePointer( Void *pointer )
{
  if( pointer != NULL )
  {
    free( pointer );
    pointer = NULL;
  }
}

static inline Void xFreeMem( Void *mem )
{
  xFreePointer( mem );
}

template<typename T>
Int getMem1D( T **array1D, Int dim0 )
{
  if( ( *array1D = ( T* )xCallocMem( dim0, sizeof(T) ) ) == NULL )
    return 0;

  return ( sizeof(T*) + dim0 * sizeof(T) );
}

template<typename T>
Int getMem2D( T*** array2D, Int dim0, Int dim1 )
{
  Int i;

  if( ( *array2D = ( T** )xMallocMem( dim0 * sizeof(T*) ) ) == NULL )
    printf( "get_mem2Dint: array2D" );
  if( ( *( *array2D ) = ( T* )xCallocMem( dim0 * dim1, sizeof(T) ) ) == NULL )
    printf( "get_mem2Dint: array2D" );

  for( i = 1; i < dim0; i++ )
    ( *array2D )[i] = ( *array2D )[i - 1] + dim1;

  return dim0 * ( sizeof(T*) + dim1 * sizeof(T) );
}

template<typename T>
Int getMem3D( T**** array3D, Int dim0, Int dim1, Int dim2 )
{
  Int i, mem_size = dim0 * sizeof(T**);

  if( ( ( *array3D ) = ( T*** )xMallocMem( dim0 * sizeof(T**) ) ) == NULL )
    printf( "get_mem3Dint: array3D" );

  mem_size += getMem2D( *array3D, dim0 * dim1, dim2 );

  for( i = 1; i < dim0; i++ )
    ( *array3D )[i] = ( *array3D )[i - 1] + dim1;

  return mem_size;
}

template<typename T>
Int getMem4D( T***** array4D, Int dim0, Int dim1, Int dim2, Int dim3 )
{
  int i, mem_size = dim0 * sizeof(int***);

  if( ( ( *array4D ) = ( T**** )xMallocMem( dim0 * sizeof(T***) ) ) == NULL )
    printf( "get_mem4Dint: array4D" );

  mem_size += getMem3D( *array4D, dim0 * dim1, dim2, dim3 );

  for( i = 1; i < dim0; i++ )
    ( *array4D )[i] = ( *array4D )[i - 1] + dim1;

  return mem_size;
}

template<typename T>
Int getMem5D( T****** array5D, Int dim0, Int dim1, Int dim2, Int dim3, Int dim4 )
{
  int i, mem_size = dim0 * sizeof(T****);

  if( ( ( *array5D ) = ( T***** )xMallocMem( dim0 * sizeof(T****) ) ) == NULL )
    printf( "get_mem5Dint: array5D" );

  mem_size += getMem4D( *array5D, dim0 * dim1, dim2, dim3, dim4 );

  for( i = 1; i < dim0; i++ )
    ( *array5D )[i] = ( *array5D )[i - 1] + dim1;

  return mem_size;
}

template<typename T>
Int getMem3ImageComponents( T**** array3D, Int dim1, Int dim2, Int ratioDim1Chroma, Int ratioDim2Chroma )
{
  Int dim0 = 3;
  Int i, mem_size = ( dim1 * dim2 + dim1 / ratioDim1Chroma * dim2 / ratioDim2Chroma * 2 ) * sizeof(T);

  mem_size += getMem2D<T*>( array3D, dim0, dim1 );

  if( ( ( *array3D )[0][0] = ( T* )xCallocMem( dim1 * dim2 + dim1 / ratioDim1Chroma * dim2 / ratioDim2Chroma * 2, sizeof(T) ) ) == NULL )
    printf( "getMem3DImageComponents: array1D" );

  // Luma
  for( i = 1; i < dim1; i++ )
    ( *array3D )[0][i] = ( *array3D )[0][i - 1] + dim2;

  // Chroma
  ( *array3D )[1][0] = ( *array3D )[0][0] + dim2 * dim1;
  for( i = 1; i < dim1 / ratioDim1Chroma; i++ )
    ( *array3D )[1][i] = ( *array3D )[1][i - 1] + dim2 / ratioDim2Chroma;

  ( *array3D )[2][0] = ( *array3D )[1][0] + dim2 / ratioDim2Chroma * dim1 / ratioDim1Chroma;
  for( i = 1; i < dim1 / ratioDim1Chroma; i++ )
    ( *array3D )[2][i] = ( *array3D )[2][i - 1] + dim2 / ratioDim2Chroma;

  return mem_size;
}

template<typename T>
Void freeMem1D( T *array1D )
{
  if( array1D )
  {
    mem_free( array1D );
  }
}

template<typename T>
Void freeMem2D( T** array2D )
{
  if( array2D )
  {
    if( *array2D )
      xFreeMem( *array2D );
    else
      printf( "free_mem2Dint: trying to free unused memory" );

    xFreeMem( array2D );
  }
  else
  {
    printf( "free_mem2Dint: trying to free unused memory" );
  }
}

template<typename T>
Void freeMem3D( T*** array3D )
{
  if( array3D )
  {
    freeMem2D( *array3D );
    xFreeMem( array3D );
  }
  else
  {
    printf( "free_mem3Dint: trying to free unused memory" );
  }
}

template<typename T>
Void freeMem4D( T**** array4D )
{
  if( array4D )
  {
    freeMem3D( *array4D );
    xFreeMem( array4D );
  }
  else
  {
    printf( "free_mem4Dint: trying to free unused memory" );
  }
}

template<typename T>
Void freeMem5D( T***** array5D )
{
  if( array5D )
  {
    freeMem4D( *array5D );
    xFreeMem( array5D );
  }
  else
  {
    printf( "free_mem5Dint: trying to free unused memory" );
  }
}

template<typename T>
Void freeMem3ImageComponents( T*** array3D )
{
  if( array3D )
  {
    if( **array3D )
      xFreeMem( **array3D );

    freeMem2D<T*>( array3D );
  }
}


}  // NAMESPACE

#endif //  __LIBMEMORY_H__
