#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <gsl/gsl_errno.h> 
#include <gsl/gsl_spline.h>
#include <gsl/gsl_integration.h>
#include <string.h>
#include "solvers.h"
#include "stls.h"
#include "qstls.h"

// -------------------------------------------------------------------
// FUNCTION USED TO ITERATIVELY SOLVE THE STLS-HNC EQUATIONS
// -------------------------------------------------------------------

void solve_qstls(input in, bool verbose) {

  // Arrays for STLS solution
  double *xx = NULL; 
  double *phi = NULL;
  double *GG = NULL;
  double *GG_new = NULL;
  double *SS = NULL;
  double *SSHF = NULL;
  double *psi = NULL;
  double *psi_xlw = NULL;

  // Allocate arrays
  alloc_stls_arrays(in, &xx, &phi, &GG, &GG_new, &SS, &SSHF);
  psi = malloc( sizeof(double) * in.nx * in.nl);  
  psi_xlw = malloc( sizeof(double) * in.nx * in.nl * in.nx);

  // Initialize arrays that are not modified with the iterative procedure
  init_fixed_stls_arrays(&in, xx, phi, SSHF, verbose);

  /* // Initial guess for Static structure factor (SSF) and static-local field correction (SLFC) */
  /* if (strcmp(in.guess_file,"NO_FILE")==0){ */
  /*   for (int ii=0; ii < in.nx; ii++) { */
  /*     GG[ii] = 0.0; */
  /*     GG_new[ii] = 1.0; */
  /*   } */
  /*   compute_ssf(SS, SSHF, GG, phi, xx, in); */
  /* } */
  /* else { */
  /*   read_guess(SS, GG, in); */
  /* } */
   
  /* // SSF and SLFC via iterative procedure */
  /* if (verbose) printf("SSF and SLFC calculation...\n"); */
  /* double iter_err = 1.0; */
  /* int iter_counter = 0; */
  /* while (iter_counter < in.nIter && iter_err > in.err_min_iter ) { */
    
  /*   // Start timing */
  /*   double tic = omp_get_wtime(); */
    
  /*   // Update SSF */
  /*   compute_ssf(SS, SSHF, GG, phi, xx, in); */
    
  /*   // Update SLFC */
  /*   compute_slfc_hnc(GG_new, GG, SS, bf, xx, in); */
    
  /*   // Update diagnostic */
  /*   iter_err = 0.0; */
  /*   iter_counter++; */
  /*   for (int ii=0; ii<in.nx; ii++) { */
  /*     iter_err += (GG_new[ii] - GG[ii]) * (GG_new[ii] - GG[ii]); */
  /*     GG[ii] = in.a_mix*GG_new[ii] + (1-in.a_mix)*GG[ii]; */
  /*   } */
  /*   iter_err = sqrt(iter_err); */
    
  /*   // End timing */
  /*   double toc = omp_get_wtime(); */
    
  /*   // Print diagnostic */
  /*   if (verbose) { */
  /*     printf("--- iteration %d ---\n", iter_counter); */
  /*     printf("Elapsed time: %f seconds\n", toc - tic); */
  /*     printf("Residual error: %.5e\n", iter_err); */
  /*     fflush(stdout); */
  /*   } */
  /* } */
  /* if (verbose) printf("Done.\n"); */
  
  /* // Internal energy */
  /* if (verbose) printf("Internal energy: %.10f\n",compute_uex(SS, xx, in)); */
  
  /* // Output to file */
  /* if (verbose) printf("Writing output files...\n"); */
  /* write_text(SS, GG, phi, SSHF, xx, in); */
  /* write_guess(SS, GG, in); */
  /* if (verbose) printf("Done.\n"); */

  // Free memory
  free_stls_arrays(xx, phi, GG, GG_new, SS, SSHF);
  free(psi);
  free(psi_xlw);

}


// -------------------------------------------------------------------
// FUNCTION USED TO COMPUTE THE ...
// -------------------------------------------------------------------

struct psi_xlw_q {

  double mu;
  double Theta;
  gsl_spline *t_int_sp_ptr;
  gsl_interp_accel *t_int_acc_ptr;

};

struct psi_xlw_t {

  double Theta;
  double ww;
  double xx;
  double ll;
  double qq;

};


void compute_psi_xlw(double *psi_xlw, double *xx, input in) {

  double err;
  size_t nevals;
  double wmax, wmin;
  double *GGu  = malloc( sizeof(double) * in.nx);

  // Declare accelerator and spline objects
  gsl_spline *t_int_sp_ptr;
  gsl_interp_accel *t_int_acc_ptr;
  
  // Allocate the accelerator and the spline objects
  t_int_sp_ptr = gsl_spline_alloc(gsl_interp_cspline, in.nx);
  t_int_acc_ptr = gsl_interp_accel_alloc();

  // Integration workspace
  gsl_integration_cquad_workspace *wsp
  = gsl_integration_cquad_workspace_alloc(100);

  // Loop over xx (wave-vector)
  for (int ii=0; ii<in.nx; ii++){

    // Loop over ll (Matsubara frequencies)
    for (int ll=0; ll<in.nl; ll++){
      
      // Integration function
      gsl_function ft_int, fq_int;
      if (ll == 0){
	ft_int.function = &psi_x0w_t;
	fq_int.function = &psi_x0w_q;
      }
      else {
	ft_int.function = &psi_xlw_t;
	fq_int.function = &psi_xlw_q;
      }

    }

  }

  /* gsl_function fu_int, fw_int; */
  /* fu_int.function = &slfc_u; */
  /* fw_int.function = &slfc_w; */

  /* // Static local field correction */
  /* // Integration over u */
  /* for (int ii=0; ii<in.nx; ii++) { */

  /*   if (xx[ii] > 0.0){ */

  /*     // Integration over w */
  /*     for (int jj=0; jj<in.nx; jj++){ */
	
  /* 	if (xx[jj] >  0.0) { */

  /* 	  struct slfcw_params slfcwp = {xx[ii], xx[jj],  */
  /* 					ssf_sp_ptr, ssf_acc_ptr}; */
  /* 	  wmin = xx[jj] - xx[ii]; */
  /* 	  if (wmin < 0.0) wmin = -wmin; */
  /* 	  // NOTE: The upper cutoff is at qm - dq for numerical reasons; */
  /* 	  // The quadrature formula attemps a tiny extrapolation which causes  */
  /* 	  // the interpolation routine to crash.  */
  /* 	  wmax = GSL_MIN(xx[in.nx-2], xx[ii]+xx[jj]); */
  /* 	  fw_int.params = &slfcwp; */
  /* 	  gsl_integration_cquad(&fw_int, */
  /* 				wmin, wmax, */
  /* 				0.0, 1e-5, */
  /* 				wsp, */
  /* 				&GGu[jj], &err, &nevals); */

  /* 	} */

  /* 	else GGu[jj] = 0.0;  */

  /*     } */

  /*     // Interpolate result of integration over w */
  /*     gsl_spline_init(GGu_sp_ptr, xx, GGu, in.nx);     */
      
  /*     // Evaluate integral over u */
  /*     struct slfcu_params slfcup = {ssf_sp_ptr, ssf_acc_ptr, */
  /* 				    slfc_sp_ptr, slfc_acc_ptr, */
  /* 				    GGu_sp_ptr, GGu_acc_ptr, */
  /* 				    bf_sp_ptr, bf_acc_ptr}; */
  /*     fu_int.params = &slfcup; */
  /*     gsl_integration_cquad(&fu_int, */
  /* 			    xx[0], xx[in.nx-1], */
  /* 			    0.0, 1e-5, */
  /* 			    wsp, */
  /* 			    &GG_new[ii], &err, &nevals); */
      
  /*     GG_new[ii] *= 3.0/(8.0*xx[ii]); */
  /*     GG_new[ii] += bf[ii]; */

  /*   } */
  /*   else  */
  /*     GG_new[ii] = 0.0; */

  /* } */

  /* // Free memory */
  /* free(GGu); */
  /* gsl_integration_cquad_workspace_free(wsp); */
  /* gsl_spline_free(ssf_sp_ptr); */
  /* gsl_interp_accel_free(ssf_acc_ptr); */
  /* gsl_spline_free(slfc_sp_ptr); */
  /* gsl_interp_accel_free(slfc_acc_ptr); */
  /* gsl_spline_free(GGu_sp_ptr); */
  /* gsl_interp_accel_free(GGu_acc_ptr); */
  /* gsl_spline_free(bf_sp_ptr); */
  /* gsl_interp_accel_free(bf_acc_ptr); */

}

double psi_x0w_t(double uu, void* pp) {
  return 0;
}

double psi_x0w_q(double uu, void* pp) {
  return 0;
}

double psi_xlw_t(double uu, void* pp) {
  return 0;
}

double psi_xlw_q(double uu, void* pp) {
  return 0;
}

/* double slfc_w(double ww, void* pp) { */

/*   struct slfcw_params* params = (struct slfcw_params*)pp; */
/*   double xx = (params->xx); */
/*   double uu = (params->uu); */
/*   gsl_spline* ssf_sp_ptr = (params->ssf_sp_ptr); */
/*   gsl_interp_accel* ssf_acc_ptr = (params->ssf_acc_ptr); */
/*   double ww2 = ww*ww, xx2 = xx*xx, uu2 = uu*uu; */
    
/*   return (ww2 - uu2 - xx2)*ww */
/*     *(gsl_spline_eval(ssf_sp_ptr, ww, ssf_acc_ptr) - 1.0); */

/* } */



// -------------------------------------------------------------------
// FUNCTION USED TO ACCESS ONE ELEMENT OF A THREE-DIMENSIONAL ARRAY
// -------------------------------------------------------------------

int idx3(int xx, int yy, int zz,
         int x_size, int y_size) {
  return (zz * x_size * y_size) + (yy * x_size) + xx;
}
