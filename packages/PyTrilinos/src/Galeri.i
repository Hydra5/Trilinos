// -*- c++ -*-

// @HEADER
// ***********************************************************************
//
//          PyTrilinos: Python Interfaces to Trilinos Packages
//                 Copyright (2014) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia
// Corporation, the U.S. Government retains certain rights in this
// software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact William F. Spotz (wfspotz@sandia.gov)
//
// ***********************************************************************
// @HEADER

%define %galeri_docstring
"
PyTrilinos.Galeri is the python interface to the Trilinos example
matrix package Galeri:

    http://trilinos.sandia.gov/packages/galeri

The purpose of Galeri is to provide a set of example matrices
distributed across a set of common processor decompositions for
testing purposes.

Galeri provides the following user-level functions:

    * CreateCartesianCoordinates  - Create cartesian coordinates
    * GetNeighboursCartesian2d    - Get neighbor nodes from a 2D grid
    * GetNeighboursCartesian3d    - Get neighbor nodes from a 3D grid
    * PrintStencil2D              - Output a 2D stencil
    * CreateMap                   - Create a Map object
    * CreateCrsMatrix             - Create a specified CrsMatrix
    * CreateVbrMatrix             - Create a specified VbrMatrix
    * ReadHB                      - Read a problem definition from an HB file

For examples of usage, please consult the following scripts in the
example subdirectory of the PyTrilinos package:

    * exGaleri.py
    * exGaleri_ReadHB.py
    * exAztecOO.py
    * exIFPACK.py
    * exMLAPI_Simple.py
"
%enddef

%module(package   = "PyTrilinos",
	autodoc   = "1",
	docstring = %galeri_docstring) Galeri

%{
// System includes
#include <sstream>

// Configuration includes
#include "PyTrilinos_config.h"
#ifdef HAVE_INTTYPES_H
#undef HAVE_INTTYPES_H
#endif
#ifdef HAVE_STDINT_H
#undef HAVE_STDINT_H
#endif

// Epetra includes
#ifdef HAVE_EPETRA
#include "Epetra_Comm.h"
#include "Epetra_SerialComm.h"
#ifdef HAVE_MPI
#include "Epetra_MpiComm.h"
#endif
#include "Epetra_BlockMap.h"
#include "Epetra_Map.h"
#include "Epetra_LocalMap.h"
#include "Epetra_MapColoring.h"
#include "Epetra_MultiVector.h"
#include "Epetra_Vector.h"
#include "Epetra_FEVector.h"
#include "Epetra_IntVector.h"
#include "Epetra_InvOperator.h"
#include "Epetra_RowMatrix.h"
#include "Epetra_BasicRowMatrix.h"
#include "Epetra_CrsMatrix.h"
#include "Epetra_JadMatrix.h"
#include "Epetra_FECrsMatrix.h"
#include "Epetra_FEVbrMatrix.h"
#include "Epetra_LinearProblem.h"
#include "Epetra_SerialDenseSVD.h"
#include "Epetra_SerialDenseSolver.h"
#include "Epetra_SerialDistributor.h"
#include "Epetra_Import.h"
#include "Epetra_Export.h"
#include "Epetra_OffsetIndex.h"
#include "Epetra_Time.h"

// Epetra Python utility code
#define NO_IMPORT_ARRAY
#include "numpy_include.hpp"
// #include "Epetra_NumPyMultiVector.hpp"
// #include "Epetra_NumPyVector.hpp"
// #include "Epetra_NumPyFEVector.hpp"
// #include "Epetra_NumPyIntVector.hpp"
// #include "Epetra_NumPySerialDenseMatrix.hpp"
// #include "Epetra_NumPySerialSymDenseMatrix.hpp"
// #include "Epetra_NumPySerialDenseVector.hpp"
// #include "Epetra_NumPyIntSerialDenseMatrix.hpp"
// #include "Epetra_NumPyIntSerialDenseVector.hpp"
#endif

// Teuchos includes
#include "Teuchos_Comm.hpp"
#include "Teuchos_DefaultSerialComm.hpp"
#ifdef HAVE_MPI
#include "Teuchos_DefaultMpiComm.hpp"
#endif
#include "PyTrilinos_Teuchos_Util.hpp"

// Galeri includes
#include "Galeri_Version.h"
#include "Galeri_Utils.h"
#include "Galeri_Maps.h"
#include "Galeri_CrsMatrices.h"
#include "Galeri_VbrMatrices.h"
#include "Galeri_ReadHB.h"
%}

// Include PyTrilinos configuration
%include "PyTrilinos_config.h"

// Standard exception handling
%include "exception.i"

// Turn on autodocumentation
%feature("autodoc", "1");

// Include Galeri documentation
%include "Galeri_dox.i"

// Typemap support for STL
%include "stl.i"

// Trilinos package imports
%import "Teuchos.i"
#ifdef HAVE_EPETRA
%import "Epetra.i"
#endif

// General exception handling
%feature("director:except")
{
  if ($error != NULL)
  {
    throw Swig::DirectorMethodException();
  }
}

%exception
{
  try
  {
    $action
  }
  catch(Swig::DirectorException &e)
  {
    SWIG_fail;
  }
  SWIG_CATCH_STDEXCEPT
  catch(...)
  {
    SWIG_exception(SWIG_UnknownError, "Unknown C++ exception");
  }
}

////////////////////////////
// Galeri_Version support //
////////////////////////////
%include "Galeri_Version.h"
%pythoncode
%{
__version__ = Galeri_Version().split()[2]
%}

//////////////////////////
// Galeri_Utils support //
//////////////////////////
%newobject Galeri::CreateCartesianCoordinates;
%include "Galeri_Utils.h"

/////////////////////////
// Galeri_Maps support //
/////////////////////////
%newobject Galeri::CreateMap;
%include "Galeri_Maps.h"

////////////////////////////////
// Galeri_CrsMatrices support //
////////////////////////////////
%newobject Galeri::CreatCrsMatrix;
%include "Galeri_CrsMatrices.h"

////////////////////////////////
// Galeri_VbrMatrices support //
////////////////////////////////
%newobject Galeri::CreateVbrMatrix;
%include "Galeri_VbrMatrices.h"

///////////////////////////
// Galeri_ReadHB support //
///////////////////////////
#ifdef HAVE_EPETRA
%feature("autodoc",
"ReadHB(str filename, Epetra.Comm comm) -> (Epetra.Map map, Epetra.CrsMatrix A,
                                           Epetra.Vector x, Epetra.Vector b,
                                           Epetra.Vector exact)

Given an HB filename and an Epetra communicator, return a tuple of
Epetra objects generated from the problem described in the file.  This
tuple contains an Epetra.Map, Epetra.CrsMatrix, and Epetra.Vectors for
the solution, right-hand side and the exact solution.")
Galeri::ReadHB;
%include "Galeri_ReadHB.h"
#endif

// Turn off the exception handling
%exception;
