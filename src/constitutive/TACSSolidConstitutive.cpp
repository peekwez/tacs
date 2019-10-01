/*
  This file is part of TACS: The Toolkit for the Analysis of Composite
  Structures, a parallel finite-element code for structural and
  multidisciplinary design optimization.

  Copyright (C) 2010 University of Toronto
  Copyright (C) 2012 University of Michigan
  Copyright (C) 2014 Georgia Tech Research Corporation
  Additional copyright (C) 2010 Graeme J. Kennedy and Joaquim
  R.R.A. Martins All rights reserved.

  TACS is licensed under the Apache License, Version 2.0 (the
  "License"); you may not use this software except in compliance with
  the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0
*/

#include "TACSSolidConstitutive.h"

const char* TACSSolidConstitutive::sName = "TACSSolidConstitutive";

const char* TACSSolidConstitutive::getObjectName(){
  return sName;
}

/*
  SolidStiffness member function definitions
*/
TACSSolidConstitutive::TACSSolidConstitutive( TACSMaterialProperties *props,
                                              TacsScalar _t,
                                              int _tNum,
                                              TacsScalar _tlb,
                                              TacsScalar _tub ){
  properties = props;
  if (properties){
    properties->incref();
  }
  t = _t;
  tNum = _tNum;
  tlb = _tlb;
  tub = _tub;
}

TACSSolidConstitutive::~TACSSolidConstitutive(){
  if (properties){
    properties->decref();
  }
}

int TACSSolidConstitutive::getNumStresses(){
  return NUM_STRESSES;
}

// Retrieve the global design variable numbers
int TACSSolidConstitutive::getDesignVarNums( int elemIndex,
                                             int dvLen, int dvNums[] ){
  if (tNum >= 0){
    if (dvNums && dvLen >= 1){
      dvNums[0] = tNum;
    }
    return 1;
  }
  return 0;
}

// Set the element design variable from the design vector
void TACSSolidConstitutive::setDesignVars( int elemIndex,
                                           int dvLen,
                                           const TacsScalar dvs[] ){
  if (tNum >= 0 && dvLen >= 1){
    t = dvs[0];
  }
}

// Get the element design variables values
void TACSSolidConstitutive::getDesignVars( int elemIndex,
                                           int dvLen,
                                           TacsScalar dvs[] ){
  if (tNum >= 0 && dvLen >= 1){
    dvs[0] = t;
  }
}

// Get the lower and upper bounds for the design variable values
void TACSSolidConstitutive::getDesignVarRange( int elemIndex,
                                               int dvLen,
                                               TacsScalar lb[],
                                               TacsScalar ub[] ){
  if (tNum >= 0 && dvLen >= 1){
    if (lb){ lb[0] = tlb; }
    if (ub){ ub[0] = tub; }
  }
}

// Evaluate the material density
TacsScalar TACSSolidConstitutive::evalDensity( int elemIndex,
                                               const double pt[],
                                               const TacsScalar X[] ){
  if (properties){
    return t*properties->getDensity();
  }
  return 0.0;
}


// Evaluate the specific heat
TacsScalar TACSSolidConstitutive::evalSpecificHeat( int elemIndex,
                                                    const double pt[],
                                                    const TacsScalar X[] ){
  if (properties){
    return t*properties->getSpecificHeat();
  }
  return 0.0;
}

// Evaluate the stresss
void TACSSolidConstitutive::evalStress( int elemIndex,
                                        const double pt[],
                                        const TacsScalar X[],
                                        const TacsScalar e[],
                                        TacsScalar s[] ){
  TacsScalar C[21];
  if (properties){
    properties->evalTangentStiffness3D(C);
    s[0] = C[0]*e[0] + C[1]*e[1]  + C[2]*e[2]  + C[3]*e[3]  + C[4]*e[4]  + C[5]*e[5];
    s[1] = C[1]*e[0] + C[6]*e[1]  + C[7]*e[2]  + C[8]*e[3]  + C[9]*e[4]  + C[10]*e[5];
    s[2] = C[2]*e[0] + C[7]*e[1]  + C[11]*e[2] + C[12]*e[3] + C[13]*e[4] + C[14]*e[5];
    s[3] = C[3]*e[0] + C[8]*e[1]  + C[12]*e[2] + C[15]*e[3] + C[16]*e[4] + C[17]*e[5];
    s[4] = C[4]*e[0] + C[9]*e[1]  + C[13]*e[2] + C[16]*e[3] + C[18]*e[4] + C[19]*e[5];
    s[5] = C[5]*e[0] + C[10]*e[1] + C[14]*e[2] + C[17]*e[3] + C[19]*e[4] + C[20]*e[5];
  }
  else {
    s[0] = s[1] = s[2] = s[3] = s[4] = s[5] = 0.0;
  }
}

// Evaluate the tangent stiffness
void TACSSolidConstitutive::evalTangentStiffness( int elemIndex,
                                                  const double pt[],
                                                  const TacsScalar X[],
                                                  TacsScalar C[] ){
  if (properties){
    properties->evalTangentStiffness3D(C);
    C[0] *= t;  C[1] *= t;  C[2] *= t;
    C[3] *= t;  C[4] *= t;  C[5] *= t;
  }
  else {
    C[0] = C[1] = C[2] = C[3] = C[4] = C[5] = 0.0;
  }
}

// Evaluate the thermal strain
void TACSSolidConstitutive::evalThermalStrain( int elemIndex,
                                               const double pt[],
                                               const TacsScalar X[],
                                               TacsScalar theta,
                                               TacsScalar e[] ){
  if (properties){
    properties->evalThermalStrain3D(e);
    e[0] *= theta;
    e[1] *= theta;
    e[2] *= theta;
    e[3] *= theta;
    e[4] *= theta;
    e[5] *= theta;
  }
  else {
    e[0] = e[1] = e[2] = e[3] = e[4] = e[5] = 0.0;
  }
}

// Evaluate the heat flux, given the thermal gradient
void TACSSolidConstitutive::evalHeatFlux( int elemIndex,
                                          const double pt[],
                                          const TacsScalar X[],
                                          const TacsScalar grad[],
                                          TacsScalar flux[] ){
  if (properties){
    TacsScalar C[3];
    properties->evalTangentHeatFlux2D(C);
    flux[0] = C[0]*grad[0] + C[1]*grad[1];
    flux[1] = C[1]*grad[1] + C[2]*grad[2];
  }
}

// Evaluate the tangent of the heat flux
void TACSSolidConstitutive::evalTangentHeatFlux( int elemIndex,
                                                 const double pt[],
                                                 const TacsScalar X[],
                                                 TacsScalar C[] ){
  if (properties){
    TacsScalar C[3];
    properties->evalTangentHeatFlux2D(C);
  }
}

// Evaluate the material failure index
TacsScalar TACSSolidConstitutive::evalFailure( int elemIndex,
                                               const double pt[],
                                               const TacsScalar X[],
                                               const TacsScalar e[] ){
  if (properties){
    TacsScalar C[21];
    properties->evalTangentStiffness3D(C);

    TacsScalar s[6];
    s[0] = C[0]*e[0] + C[1]*e[1]  + C[2]*e[2]  + C[3]*e[3]  + C[4]*e[4]  + C[5]*e[5];
    s[1] = C[1]*e[0] + C[6]*e[1]  + C[7]*e[2]  + C[8]*e[3]  + C[9]*e[4]  + C[10]*e[5];
    s[2] = C[2]*e[0] + C[7]*e[1]  + C[11]*e[2] + C[12]*e[3] + C[13]*e[4] + C[14]*e[5];
    s[3] = C[3]*e[0] + C[8]*e[1]  + C[12]*e[2] + C[15]*e[3] + C[16]*e[4] + C[17]*e[5];
    s[4] = C[4]*e[0] + C[9]*e[1]  + C[13]*e[2] + C[16]*e[3] + C[18]*e[4] + C[19]*e[5];
    s[5] = C[5]*e[0] + C[10]*e[1] + C[14]*e[2] + C[17]*e[3] + C[19]*e[4] + C[20]*e[5];

    return properties->vonMisesFailure3D(s);
  }
  return 0.0;
}

// Evaluate the derivative of the failure criteria w.r.t. strain
TacsScalar TACSSolidConstitutive::evalFailureStrainSens( int elemIndex,
                                                         const double pt[],
                                                         const TacsScalar X[],
                                                         const TacsScalar e[],
                                                         TacsScalar dfde[] ){
  if (properties){
    TacsScalar C[21];
    properties->evalTangentStiffness3D(C);

    TacsScalar s[6];
    s[0] = C[0]*e[0] + C[1]*e[1]  + C[2]*e[2]  + C[3]*e[3]  + C[4]*e[4]  + C[5]*e[5];
    s[1] = C[1]*e[0] + C[6]*e[1]  + C[7]*e[2]  + C[8]*e[3]  + C[9]*e[4]  + C[10]*e[5];
    s[2] = C[2]*e[0] + C[7]*e[1]  + C[11]*e[2] + C[12]*e[3] + C[13]*e[4] + C[14]*e[5];
    s[3] = C[3]*e[0] + C[8]*e[1]  + C[12]*e[2] + C[15]*e[3] + C[16]*e[4] + C[17]*e[5];
    s[4] = C[4]*e[0] + C[9]*e[1]  + C[13]*e[2] + C[16]*e[3] + C[18]*e[4] + C[19]*e[5];
    s[5] = C[5]*e[0] + C[10]*e[1] + C[14]*e[2] + C[17]*e[3] + C[19]*e[4] + C[20]*e[5];

    TacsScalar sens[6];
    TacsScalar fail = properties->vonMisesFailure3DStressSens(s, sens);

    dfde[0] = C[0]*sens[0] + C[1]*sens[1]  + C[2]*sens[2]  + C[3]*sens[3]  + C[4]*sens[4]  + C[5]*sens[5];
    dfde[1] = C[1]*sens[0] + C[6]*sens[1]  + C[7]*sens[2]  + C[8]*sens[3]  + C[9]*sens[4]  + C[10]*sens[5];
    dfde[2] = C[2]*sens[0] + C[7]*sens[1]  + C[11]*sens[2] + C[12]*sens[3] + C[13]*sens[4] + C[14]*sens[5];
    dfde[3] = C[3]*sens[0] + C[8]*sens[1]  + C[12]*sens[2] + C[15]*sens[3] + C[16]*sens[4] + C[17]*sens[5];
    dfde[4] = C[4]*sens[0] + C[9]*sens[1]  + C[13]*sens[2] + C[16]*sens[3] + C[18]*sens[4] + C[19]*sens[5];
    dfde[5] = C[5]*sens[0] + C[10]*sens[1] + C[14]*sens[2] + C[17]*sens[3] + C[19]*sens[4] + C[20]*sens[5];

    return fail;
  }
  return 0.0;
}
