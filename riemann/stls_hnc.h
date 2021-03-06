#ifndef STLS_HNC_H
#define STLS_HNC_H

#include "solvers.h"

// -------------------------------------------------------------------
// FUNCTION USED TO COMPUTE THE STATIC LOCAL FIELD CORRECTION
// -------------------------------------------------------------------

void compute_slfc_hnc(double *GG_new, double *GG, double *SS,
                      double *bf, double *xx, input in);

void compute_bf(double *bf, double *xx, input in, bool iet);


#endif
