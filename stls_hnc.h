#ifndef STLS_HNC_H
#define STLS_HNC_H

#include "stls.h"

void solve_stls_hnc(input in, bool verbose);

void compute_slfc_hnc(double *GG_new, double *GG, 
		      double *SS, double *xx, input in);

void write_text_hnc(double *SS, double *GG,
		    double *xx, input in );


#endif
