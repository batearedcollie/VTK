/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVASPAnimationReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkVASPAnimationReader - Reader for VASP animation files.
//
// .SECTION Description
// Reads VASP animation files (e.g. NPT_Z_ANIMATE.out).

#ifndef vtkVASPAnimationReader_h
#define vtkVASPAnimationReader_h

#include "vtkDomainsChemistryModule.h" // For export macro
#include "vtkMoleculeAlgorithm.h"
#include "vtkVector.h" // For vtkVector3f

namespace vtksys {
class RegularExpression;
}

class VTKDOMAINSCHEMISTRY_EXPORT vtkVASPAnimationReader:
    public vtkMoleculeAlgorithm
{
public:
  static vtkVASPAnimationReader* New();
  vtkTypeMacro(vtkVASPAnimationReader, vtkMoleculeAlgorithm)
  virtual void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // The name of the file to read.
  vtkSetStringMacro(FileName)
  vtkGetStringMacro(FileName)

protected:
  vtkVASPAnimationReader();
  ~vtkVASPAnimationReader();

  virtual int RequestData(vtkInformation *request,
                          vtkInformationVector **inInfoVecs,
                          vtkInformationVector *outInfoVec);
  virtual int RequestInformation(vtkInformation *request,
                                 vtkInformationVector **inInfoVecs,
                                 vtkInformationVector *outInfoVec);

  // Description:
  // Advance @a in to the start of the data for the next timestep. Parses the
  // "time = X" line, sets @a time to the timestamp, and returns true on
  // success. Returning false means either EOF was reached, or the timestamp
  // line could not be parsed.
  bool NextTimeStep(std::istream &in, double &time);

  // Description:
  // Called by RequestData to determine which timestep to read. If both
  // UPDATE_TIME_STEP and TIME_STEPS are defined, return the index of the
  // timestep in TIME_STEPS closest to UPDATE_TIME_STEP. If either is undefined,
  // return 0.
  size_t SelectTimeStepIndex(vtkInformation *info);

  bool ReadMolecule(std::istream &in, vtkMolecule *molecule);

  // Description:
  // Determine lattice vectors by analysing the segment boundary info. If this
  // method returns false, a parsing error occurred and the read should be
  // aborted. If latticeFound is false, parsing succeeded, but the lattice
  // could not be determined.
  bool DetermineLatticeVectors(std::istream &in, vtkVector3d lattice[3],
                               vtkVector3d &origin, bool &latticeFound);


  char *FileName;

  vtksys::RegularExpression *TimeParser;
  vtksys::RegularExpression *SegmentParser;
  vtksys::RegularExpression *AtomCountParser;
  vtksys::RegularExpression *AtomParser;

private:
  vtkVASPAnimationReader(const vtkVASPAnimationReader&) VTK_DELETE_FUNCTION;
  void operator=(const vtkVASPAnimationReader&) VTK_DELETE_FUNCTION;
};

#endif // vtkVASPAnimationReader_h
