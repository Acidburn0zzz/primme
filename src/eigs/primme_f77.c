/*******************************************************************************
 * Copyright (c) 2016, College of William & Mary
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the College of William & Mary nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COLLEGE OF WILLIAM & MARY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * PRIMME: https://github.com/primme/primme
 * Contact: Andreas Stathopoulos, a n d r e a s _at_ c s . w m . e d u
 *******************************************************************************
 * File: primme_f77.c
 *
 * Purpose - Implementation of PRIMME F77 interface functions.
 *
 ******************************************************************************/

#include <stdlib.h>   /* free */
#include <limits.h>   /* INT_MAX */
#include "primme_f77_private.h"

/******************************************************************************/
/* The following differ only in the name mangling from C                      */
/******************************************************************************/

/*****************************************************************************
 * Wrapper for calling xprimme from Fortran 77/90. 
 * The only difference from primme: the return value passed as parameter 
 *****************************************************************************/

void AS_FORTRAN(Sprimme)(REAL *evals, SCALAR *evecs,
      REAL *rnorms, primme_params **primme, int *ierr) {

  *ierr = CONCAT(SCALAR_PRE,primme)(evals, evecs, rnorms, *primme);

} /* end of xprimme_f77 wrapper for calling from Fortran */


/* Only define these functions ones */
#ifdef USE_DOUBLE
#include "notemplate.h"

/*****************************************************************************
 * Initialize handles also the allocation of primme structure 
 *****************************************************************************/
void AS_FORTRAN(primme_initialize)(primme_params **primme) {

   *primme = NULL;
   if (MALLOC_PRIMME(1, primme) == 0)
      primme_initialize(*primme);
}

/*****************************************************************************
 *  * Free the internally allocated work arrays of the primme structure 
 *   *****************************************************************************/
void AS_FORTRAN(primme_free)(primme_params **primme) {

   primme_free(*primme);
   free(*primme);
   *primme = NULL;
}

/*****************************************************************************
 * Wrapper for displaying the primme parameters 
 *****************************************************************************/
void AS_FORTRAN(primme_display_params)(primme_params **primme) {

   primme_display_params(*(*primme));
}

/*************************************************************************
 * set method takes "int method" as input. On return:
 *    returnValue = 0 successful completion 
 *    returnValue < 0 no such method exists. If not defined by user, defaults 
 *    have been set for maxBasisSize, minRestartSize, and maxBlockSize   
 *************************************************************************/
void AS_FORTRAN(primme_set_method)(primme_params **primme,
      primme_preset_method *method, int *returnValue) {
  *returnValue = primme_set_method(*method, *primme);
}

/*************************************************************************
 * primme_set_member_f77(primme_params *primme, int *label, void *ptr)
 *
 * Sets any of the members of the primme data structure from a Fortran call.
 *
 * primme is a pointer to the primme data structure.
 *
 * void *ptr generic type pointer is used for input, and is cast to the 
 * required pointer type before it is assigned to a data structure member.
 *
 * The choice of member is through an integer label. The integers describing
 * the member are prefixed with PRIMMEF77_ followed by the actual name of 
 * the member. If the member is a structure, underscores are used for dots,
 *
 * Eg the label int for:    primme->correctionParams.projectors.LeftX
 * is:                    PRIMMEF77_correctionParams_projectors_LeftX
 *
 *************************************************************************/

void AS_FORTRAN(primme_set_member)(primme_params **primme, int *label,
      union f77_value v, int *ierr) {

   int i;

   *ierr = 0;
   switch (*label) {
      case PRIMMEF77_n:
              (*primme)->n = *v.int_v;
      break;
      case PRIMMEF77_matrixMatvec:
              (*primme)->matrixMatvec = v.matFunc_v;
      break;
      case PRIMMEF77_massMatrixMatvec:
              (*primme)->massMatrixMatvec = v.matFunc_v;
      break;
      case PRIMMEF77_applyPreconditioner:
              (*primme)->applyPreconditioner = v.matFunc_v;
      break;
      case PRIMMEF77_numProcs:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->numProcs = (int)*v.int_v;
      break;
      case PRIMMEF77_procID:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->procID = (int)*v.int_v;
      break;
      case PRIMMEF77_commInfo:
              (*primme)->commInfo = v.ptr_v;
      break;
      case PRIMMEF77_nLocal:
              (*primme)->nLocal = *v.int_v;
      break;
      case PRIMMEF77_globalSumReal:
              (*primme)->globalSumReal = v.globalSumRealFunc_v;
      break;
      case PRIMMEF77_numEvals:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->numEvals = (int)*v.int_v;
      break;
      case PRIMMEF77_target:
              (*primme)->target = *v.target_v;
      break;
      case PRIMMEF77_numTargetShifts:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->numTargetShifts = (int)*v.int_v;
      break;
      case PRIMMEF77_targetShifts:
              (*primme)->targetShifts = v.double_v;
      break;
      case PRIMMEF77_locking:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->locking = (int)*v.int_v;
      break;
      case PRIMMEF77_initSize:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->initSize = (int)*v.int_v;
      break;
      case PRIMMEF77_numOrthoConst:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->numOrthoConst = (int)*v.int_v;
      break;
      case PRIMMEF77_dynamicMethodSwitch:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->dynamicMethodSwitch = (int)*v.int_v;
      break;
      case PRIMMEF77_maxBasisSize:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->maxBasisSize = (int)*v.int_v;
      break;
      case PRIMMEF77_minRestartSize:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->minRestartSize = (int)*v.int_v;
      break;
      case PRIMMEF77_maxBlockSize:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->maxBlockSize = (int)*v.int_v;
      break;
      case PRIMMEF77_maxMatvecs:
              (*primme)->maxMatvecs = *v.int_v;
      break;
      case PRIMMEF77_maxOuterIterations:
              (*primme)->maxOuterIterations = *v.int_v;
      break;
      case PRIMMEF77_intWorkSize:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->intWorkSize = (int)*v.int_v;
      break;
      case PRIMMEF77_realWorkSize:
              (*primme)->realWorkSize = (size_t)*v.int_v;
      break;
      case PRIMMEF77_iseed:
         for (i=0; i< 4; i++) {
            (*primme)->iseed[i] = v.int_v[i];
         }
      break;
      case PRIMMEF77_intWork:
              (*primme)->intWork = (int*)v.int_v;
      break;
      case PRIMMEF77_realWork:
              (*primme)->realWork = v.ptr_v;
      break;
      case PRIMMEF77_aNorm:
              (*primme)->aNorm = *v.double_v;
      break;
      case PRIMMEF77_eps:
              (*primme)->eps = *v.double_v;
      break;
      case PRIMMEF77_printLevel:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->printLevel = (int)*v.int_v;
      break;
      case PRIMMEF77_outputFile:
              (*primme)->outputFile = v.file_v;
      break;
      case PRIMMEF77_matrix:
              (*primme)->matrix = v.ptr_v;
      break;
      case PRIMMEF77_preconditioner:
              (*primme)->preconditioner = v.ptr_v;
      break;
      case PRIMMEF77_initBasisMode:
              (*primme)->initBasisMode = *v.init_v;
      break;
      case PRIMMEF77_projectionParams_projection:
              (*primme)->projectionParams.projection = *v.projection_v;
      break;
      case PRIMMEF77_restartingParams_scheme:
              (*primme)->restartingParams.scheme = *v.restartscheme_v;
      break;
      case PRIMMEF77_restartingParams_maxPrevRetain:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->restartingParams.maxPrevRetain = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_precondition:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.precondition = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_robustShifts:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.robustShifts = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_maxInnerIterations:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.maxInnerIterations = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_projectors_LeftQ:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.projectors.LeftQ = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_projectors_LeftX:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.projectors.LeftX = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_projectors_RightQ:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.projectors.RightQ = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_projectors_RightX:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.projectors.RightX = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_projectors_SkewQ:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.projectors.SkewQ = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_projectors_SkewX:
              if (*v.int_v > INT_MAX) *ierr = 1; else 
              (*primme)->correctionParams.projectors.SkewX = (int)*v.int_v;
      break;
      case PRIMMEF77_correctionParams_convTest:
              (*primme)->correctionParams.convTest = *v.convergencetest_v;
      break;
      case PRIMMEF77_correctionParams_relTolBase:
              (*primme)->correctionParams.relTolBase = *v.double_v;
      break;
      case PRIMMEF77_stats_numOuterIterations:
              (*primme)->stats.numOuterIterations = *v.int_v;
      break;
      case PRIMMEF77_stats_numRestarts:
              (*primme)->stats.numRestarts = *v.int_v;
      break;
      case PRIMMEF77_stats_numMatvecs:
              (*primme)->stats.numMatvecs = *v.int_v;
      break;
      case PRIMMEF77_stats_numPreconds:
              (*primme)->stats.numPreconds = *v.int_v;
      break;
      case PRIMMEF77_stats_elapsedTime:
              (*primme)->stats.elapsedTime = *v.double_v;
      break;
      case PRIMMEF77_stats_estimateMinEVal:
              (*primme)->stats.estimateMinEVal = *v.double_v;
      break;
      case PRIMMEF77_stats_estimateMaxEVal:
              (*primme)->stats.estimateMaxEVal = *v.double_v;
      break;
      case PRIMMEF77_stats_estimateLargestSVal:
              (*primme)->stats.estimateLargestSVal = *v.double_v;
      break;
      case PRIMMEF77_stats_maxConvTol:
              (*primme)->stats.maxConvTol = *v.double_v;
      break;
      case PRIMMEF77_convTestFun:
              (*primme)->convTestFun = v.convTestFun_v;
      break;
      case PRIMMEF77_ldevecs:
              (*primme)->ldevecs = *v.int_v;
      break;
      case PRIMMEF77_ldOPs:
              (*primme)->ldOPs = *v.int_v;
      break;
      default : 
      *ierr = 1;
   }

}

/*************************************************************************
 * primme_get_member_f77(primme_params *primme, int *label, void *ptr)
 *
 * Gets the value of any of the members of the primme data structure from 
 * a Fortran call.
 *
 * To be called ONLY from a f77 subroutine that is called by C PRIMME, 
 * (eg.,MV, PRECOND, GLOBALSUM) ---NOT from F77 driver.
 *
 *   F77 driver --> C PRIMME --> F77 function (MV) --> primme_get_member_f77
 *
 * For obtaining information from the driver, see below. The reason is that 
 * such f77 functions (MV, etc) receive the primme structure from PRIMME as
 * (primme_params *primme). However, the f77 driver has a pointer to **primme. 
 *
 * void *ptr generic type pointer is used for output, and is cast to the 
 * required pointer type before it is assigned the data structure member.
 *
 * The choice of member is through an integer label. The integers describing
 * the member are prefixed with PRIMMEF77_ followed by the actual name of 
 * the member. If the member is a structure, underscores are used for dots,
 *
 * Eg the label int for:    primme->correctionParams.projectors.LeftX
 * is:                    PRIMMEF77_correctionParams_projectors_LeftX
 *
 *************************************************************************/

void AS_FORTRAN(primme_get_member)(primme_params *primme, int *label,
      union f77_value_ptr *v, int *ierr) {

   int i;

   *ierr = 0;
   switch (*label) {
      case PRIMMEF77_n:
              v->int_v = primme->n;
      break;
      case PRIMMEF77_matrixMatvec:
              v->matFunc_v = primme->matrixMatvec;
      break;
      case PRIMMEF77_massMatrixMatvec:
              v->matFunc_v = primme->massMatrixMatvec;
      break;
      case PRIMMEF77_applyPreconditioner:
              v->matFunc_v = primme->applyPreconditioner;
      break;
      case PRIMMEF77_numProcs:
              v->int_v = primme->numProcs;
      break;
      case PRIMMEF77_procID:
              v->int_v = primme->procID;
      break;
      case PRIMMEF77_commInfo:
              v->ptr_v = primme->commInfo;
      break;
      case PRIMMEF77_nLocal:
              v->int_v = primme->nLocal;
      break;
      case PRIMMEF77_globalSumReal:
              v->globalSumRealFunc_v = primme->globalSumReal;
      break;
      case PRIMMEF77_numEvals:
              v->int_v = primme->numEvals;
      break;
      case PRIMMEF77_target:
              v->target_v = primme->target;
      break;
      case PRIMMEF77_numTargetShifts:
              v->int_v = primme->numTargetShifts;
      break;
      case PRIMMEF77_targetShifts:
         for (i=0; i< primme->numTargetShifts; i++) {
             (&v->double_v)[i] = primme->targetShifts[i];
         }
      break;
      case PRIMMEF77_locking:
              v->int_v = primme->locking;
      break;
      case PRIMMEF77_initSize:
              v->int_v = primme->initSize;
      break;
      case PRIMMEF77_numOrthoConst:
              v->int_v = primme->numOrthoConst;
      break;
      case PRIMMEF77_dynamicMethodSwitch:
              v->int_v = primme->dynamicMethodSwitch;
      break;
      case PRIMMEF77_maxBasisSize:
              v->int_v = primme->maxBasisSize;
      break;
      case PRIMMEF77_minRestartSize:
              v->int_v = primme->minRestartSize;
      break;
      case PRIMMEF77_maxBlockSize:
              v->int_v = primme->maxBlockSize;
      break;
      case PRIMMEF77_maxMatvecs:
              v->int_v = primme->maxMatvecs;
      break;
      case PRIMMEF77_maxOuterIterations:
              v->int_v = primme->maxOuterIterations;
      break;
      case PRIMMEF77_intWorkSize:
              v->int_v = primme->intWorkSize;
      break;
      case PRIMMEF77_realWorkSize:
              v->int_v = primme->realWorkSize;
      break;
      case PRIMMEF77_iseed:
         for (i=0; i< 4; i++) {
            (&v->int_v)[i] = primme->iseed[i];
         }
      break;
      case PRIMMEF77_intWork:
              v->ptr_v = primme->intWork;
      break;
      case PRIMMEF77_realWork:
              v->ptr_v = primme->realWork;
      break;
      case PRIMMEF77_aNorm:
              v->double_v = primme->aNorm;
      break;
      case PRIMMEF77_eps:
              v->double_v = primme->eps;
      break;
      case PRIMMEF77_printLevel:
              v->int_v = primme->printLevel;
      break;
      case PRIMMEF77_outputFile:
              v->file_v = primme->outputFile;
      break;
      case PRIMMEF77_matrix:
              v->ptr_v = primme->matrix;
      break;
      case PRIMMEF77_preconditioner:
              v->ptr_v = primme->preconditioner;
      break;
      case PRIMMEF77_restartingParams_scheme:
              v->restartscheme_v = primme->restartingParams.scheme;
      break;
      case PRIMMEF77_restartingParams_maxPrevRetain:
              v->int_v = primme->restartingParams.maxPrevRetain;
      break;
      case PRIMMEF77_correctionParams_precondition:
              v->int_v = primme->correctionParams.precondition;
      break;
      case PRIMMEF77_correctionParams_robustShifts:
              v->int_v = primme->correctionParams.robustShifts;
      break;
      case PRIMMEF77_correctionParams_maxInnerIterations:
              v->int_v = primme->correctionParams.maxInnerIterations;
      break;
      case PRIMMEF77_correctionParams_projectors_LeftQ:
              v->int_v = primme->correctionParams.projectors.LeftQ;
      break;
      case PRIMMEF77_correctionParams_projectors_LeftX:
              v->int_v = primme->correctionParams.projectors.LeftX;
      break;
      case PRIMMEF77_correctionParams_projectors_RightQ:
              v->int_v = primme->correctionParams.projectors.RightQ;
      break;
      case PRIMMEF77_correctionParams_projectors_RightX:
              v->int_v = primme->correctionParams.projectors.RightX;
      break;
      case PRIMMEF77_correctionParams_projectors_SkewQ:
              v->int_v = primme->correctionParams.projectors.SkewQ;
      break;
      case PRIMMEF77_correctionParams_projectors_SkewX:
              v->int_v = primme->correctionParams.projectors.SkewX;
      break;
      case PRIMMEF77_correctionParams_convTest:
              v->convergencetest_v = primme->correctionParams.convTest;
      break;
      case PRIMMEF77_correctionParams_relTolBase:
              v->double_v = primme->correctionParams.relTolBase;
      break;
      case PRIMMEF77_stats_numOuterIterations:
              v->int_v = primme->stats.numOuterIterations;
      break;
      case PRIMMEF77_stats_numRestarts:
              v->int_v = primme->stats.numRestarts;
      break;
      case PRIMMEF77_stats_numMatvecs:
              v->int_v = primme->stats.numMatvecs;
      break;
      case PRIMMEF77_stats_numPreconds:
              v->int_v = primme->stats.numPreconds;
      break;
      case PRIMMEF77_stats_elapsedTime:
              v->double_v = primme->stats.elapsedTime;
      break;
      case PRIMMEF77_ldevecs:
              v->int_v = primme->ldevecs;
      break;
      case PRIMMEF77_ldOPs:
              v->int_v = primme->ldOPs;
      break;
      default :
      *ierr = 1;
   }
}

/*************************************************************************
 *   If the user requires the current shift suggested by PRIMME to 
 *   be used in inverting (M-shift_i I), for the i-th eigenvalue
 *   this is at location (i-1) in C   (i-th location in Fortran)
 *   of the array primme->ShiftsForPreconditioner
 *************************************************************************/
void AS_FORTRAN(primme_get_prec_shift)(primme_params *primme, int *i,
      double *shift) {
   *shift = primme->ShiftsForPreconditioner[*i-1];
}

/*************************************************************************/
/* If the information from primme is needed from the calling driver in F77
 * the primme is passed as  (primme_params **), while any f77 function such
 * as MV or preconditioning that needs primme information it passes it as
 * (primme_params *). The reason is that PRIMME C code works with the pointer
 * to primme and that's what it calls MV/GLOBALSUM with.
 *
 *   F77 driver --> other F77 subs --> primmetop_get_member_f77
 * 
 * If the user needs to obtain structure info from the top function that 
 * initialized primme they should call the wrappers:
 *
 *      primmetop_get_member_f77
 *  or
 *      primmetop_get_prec_shift_f77   (probably not needed ever)
 *
 * From within MV, PRECONDITION, or GLOBALSUMDOUBLE call the above function
 *      primme_get_member_f77
 *
 *************************************************************************/
void AS_FORTRAN(primmetop_get_member)(primme_params **primme, int *label,
      union f77_value_ptr *ptr, int *ierr) {
   AS_FORTRAN(primme_get_member)(*primme, label, ptr, ierr);
}

void AS_FORTRAN(primmetop_get_prec_shift)(primme_params **primme, int *i,
      double *shift) {
   AS_FORTRAN(primme_get_prec_shift)(*primme, i, shift);
}

#endif /* USE_DOUBLE */
