/*
 * SCC_LapackBandRoutines.h
 *
 *  Created on: Aug 17, 2018
 *      Author: anderson
 */

/*
#############################################################################
#
# Copyright  2018 Chris Anderson
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the Lesser GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# For a copy of the GNU General Public License see
# <http://www.gnu.org/licenses/>.
#
#############################################################################
*/



#include <vector>
#include <iostream>
#include <cstring>

#include "SCC_LapackBandMatrix.h"

#ifndef SCC_LAPACK_BAND_ROUTINES_
#define SCC_LAPACK_BAND_ROUTINES_

// Headers for band matrix routines

extern "C" void dgbsvx_(char* FACT, char* TRANS, long* N, long* KL, long*  	KU, long*  	NRHS,
				double* AB, long* LDAB, double* AFB, long* LDAFB, long* IPIV, char* EQUED,
				double* R, double* C, double* B, long* LDB, double* X, long* LDX, double* RCOND, double* FERR,
				double*   BERR, double *WORK, long*	IWORK, long* INFO);

namespace SCC
{

/*
*  DGBSVX uses the LU factorization to compute the solution to a real
*  system of linear equations A * X = B, A**T * X = B, or A**H * X = B,
*  where A is a band matrix of order N with KL subdiagonals and KU
*  superdiagonals, and X and B are N-by-NRHS matrices.
*
*  Error bounds on the solution and a condition estimate are also
*  provided.
*
*/

class DGBSVX
{
	public:

	DGBSVX()
	{
	FACT  = 'E'; // E = equilibrate  N =  no-equilibration
	EQUED = 'B';
	}

	void setEquilibration(bool val = true)
	{
	if(val) {FACT  = 'E';}
	else    {FACT  = 'N';}
	}

	void clearEquilibration()
	{
	FACT  = 'N';
	}

	 /*
          setEquilibrationType() specifies the form of equilibration that was done.
          = 'N':  No equilibration (always true if FACT = 'N').
          = 'R':  Row equilibration, i.e., A has been premultiplied by
                  diag(R).
          = 'C':  Column equilibration, i.e., A has been postmultiplied
                  by diag(C).
          = 'B':  Both row and column equilibration, i.e., A has been
                  replaced by diag(R) * A * diag(C).
    */


	void setEquilibrationType(char T)
	{
	EQUED = T;
	}

	void applyInverse(SCC::LapackBandMatrix& S, std::vector<double>& f)
	{
	applyInverse(S,&f[0]);
	}

	void applyInverse(SCC::LapackBandMatrix& S,double* f)
	{
    //char FACT  = 'E'; // E Or N for no-equilibration

    char TRANS = 'N';

    long N     = S.N;
    long KL    = S.kl;
    long KU    = S.ku;

    long NRHS  = 1;
    double* AB = S.getDataPointer();
    long LDAB  = KL + KU + 1;
    long LDAFB = 2*KL+KU+1;


    SCC::LapackMatrix AFB(LDAFB,N);
    std::vector<long>      IPIV(N);


    std::vector<double> R(N);
    std::vector<double> C(N);

    double* Bptr = &f[0];
    long LDB     = N;

    std::vector<double> X(N); // Now an output argument

    long LDX     = N;

    //double RCOND = 1.0;
    //double FERR;
    //double BERR;

    std::vector<double> WORK(3*N);
    std::vector<long>    IWORK(N);

    long INFO = 0;

    // coeffRHS

    dgbsvx_(&FACT, &TRANS, &N, &KL, &KU, &NRHS, AB, &LDAB, AFB.getDataPointer(), &LDAFB, &IPIV[0], &EQUED,
    		&R[0], &C[0], Bptr, &LDB, &X[0], &LDX,&RCOND, &FERR,
			&BERR, &WORK[0], &IWORK[0], &INFO);

    if(INFO != 0)
    {
    	if(INFO < 0) { std::cout << -INFO << "argument to dgbsvx had an illegal value " << std::endl;}
    	if(INFO > 0) { std::cout <<  "Error in dgbsvs INFO = " << INFO << " " << " N " << N << std::endl;}
    }

    // Capture the solution

    // f = X;

    std::memcpy(&f[0],&X[0],N*sizeof(double));
	}


	double getReciprocalCondNumber()
	{
	return RCOND;
	}

/*
*  FERR    (output) DOUBLE PRECISION array, dimension (NRHS)
*          The estimated forward error bound for each solution vector
*          X(j) (the j-th column of the solution matrix X).
*          If XTRUE is the true solution corresponding to X(j), FERR(j)
*          is an estimated upper bound for the magnitude of the largest
*          element in (X(j) - XTRUE) divided by the magnitude of the
*          largest element in X(j).  The estimate is as reliable as
*          the estimate for RCOND, and is almost always a slight
*          overestimate of the true error.
*/
	double getForwardErrEstimate()
	{
	return FERR;
	}

/*
*  BERR    (output) DOUBLE PRECISION array, dimension (NRHS)
*          The componentwise relative backward error of each solution
*          vector X(j) (i.e., the smallest relative change in
*          any element of A or B that makes X(j) an exact solution).
*/
	double getBackwardErrEstimate()
	{
	return BERR;
	}


	char   FACT;
	char   EQUED;

	double RCOND;
    double FERR;
    double BERR;
};

}

#endif /* SCC_LapackBandRoutines__ */
