#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "externs.h"
#include "cephes.h"
#include "tm_stm32f4_sdram.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                     R A N D O M  E X C U R S I O N S  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
RandomExcursions(int n)
{
	int32_t b, i, j, k, J, x;
	int32_t cycleStart, cycleStop, *cycle = NULL, *S_k = NULL;
	int32_t stateX[8] = { -4, -3, -2, -1, 1, 2, 3, 4 };
	int32_t counter[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	float p_value, sum, constraint, nu[6][8];
	float pi[5][6] = { {0.0000000000, 0.00000000000, 0.00000000000, 0.00000000000, 0.00000000000, 0.0000000000},
						 {0.5000000000, 0.25000000000, 0.12500000000, 0.06250000000, 0.03125000000, 0.0312500000},
						 {0.7500000000, 0.06250000000, 0.04687500000, 0.03515625000, 0.02636718750, 0.0791015625},
						 {0.8333333333, 0.02777777778, 0.02314814815, 0.01929012346, 0.01607510288, 0.0803755143},
						 {0.8750000000, 0.01562500000, 0.01367187500, 0.01196289063, 0.01046752930, 0.0732727051} };
	
	if ( ((cycle = (int *)calloc(MAX(1000, n/100), sizeof(int))) == NULL) ) {
		//insufficient memory
		if ( cycle != NULL )
			free(cycle);
		return;
	}
	
	J = 0; 					/* DETERMINE CYCLES */
	S_k[0] = 2*(int)TM_SDRAM_Read8(0) - 1;
	for( i=1; i<n; i++ ) {
		S_k[i] = TM_SDRAM_Read2ndVector(i-1) + 2*TM_SDRAM_Read8(i) - 1;
		if ( TM_SDRAM_Read2ndVector(i) == 0 ) {
			J++;
			if ( J > MAX(1000, n/100) ) {
				// EXCEEDING THE MAX NUMBER OF CYCLES EXPECTED
				free(S_k);
				free(cycle);
				return;
			}
			cycle[J] = i;
		}
	}
	if ( TM_SDRAM_Read2ndVector(n-1) != 0 )
		J++;
	cycle[J] = n;

	constraint = MAX(0.005*pow(n, 0.5), 500);
	if (J < constraint) {
		//insufficient number of cycles
		for(i = 0; i < 8; i++)
		{
			//insufficien nr of cycles
		}
	}
	else {
		cycleStart = 0;
		cycleStop  = cycle[1];
		for ( k=0; k<6; k++ )
			for ( i=0; i<8; i++ )
				nu[k][i] = 0.;
		for ( j=1; j<=J; j++ ) {                           /* FOR EACH CYCLE */
			for ( i=0; i<8; i++ )
				counter[i] = 0;
			for ( i=cycleStart; i<cycleStop; i++ ) {
				if ( (TM_SDRAM_Read2ndVector(i) >= 1 && TM_SDRAM_Read2ndVector(i) <= 4) || (TM_SDRAM_Read2ndVector(i) >= -4 && TM_SDRAM_Read2ndVector(i) <= -1) ) {
					if ( TM_SDRAM_Read2ndVector(i) < 0 )
						b = 4;
					else
						b = 3;
					counter[TM_SDRAM_Read2ndVector(i)+b]++;
				}
			}
			cycleStart = cycle[j]+1;
			if ( j < J )
				cycleStop = cycle[j+1];
			
			for ( i=0; i<8; i++ ) {
				if ( (counter[i] >= 0) && (counter[i] <= 4) )
					nu[counter[i]][i]++;
				else if ( counter[i] >= 5 )
					nu[5][i]++;
			}
		}
		
		for ( i=0; i<8; i++ ) {
			x = stateX[i];
			sum = 0.;
			for ( k=0; k<6; k++ )
				sum += pow(nu[k][i] - J*pi[(int)fabs(x)][k], 2) / (J*pi[(int)fabs(x)][k]);
			p_value = cephes_igamc(2.5, sum/2.0);
			
			if ( isNegative(p_value) || isGreaterThanOne(p_value) )
			{
				//pval out of range
			}

			//decision
		}
	} 
	free(S_k);
	free(cycle);
}
