#include <stdio.h>
#include <unistd.h>
/* ---------------------------------------------------------- */
#define P_NONE	"\033[m"
#define P_RED	"\033[0;32;31m"
#define P_GREEN	"\033[0;32;32m"
#define ADC_SYSTEM_CLK	24000000
/* ---------------------------------------------------------- */
int main(int argc, char* argv[])
{
	int REQ_freq = 0;
	int REQ_tolerance = 0;
	const int avg[] = {1, 2, 4, 8, 16};
	int opendly = 0;
	int clkdiv = 0;
	int avg_i, Odly_i, Sdly_i, div_i ;
	unsigned int freq ;
	unsigned int ob_fun ;	/* object function ,find max clock divider, and open delay*/
	int solution = 0;
	int op ;

	int obj_max = 0, obj_clkdiv, obj_Odly, obj_Sdly, obj_avg ;

	/* Get argument */
	if(argc >5 || argc < 2) {
		fprintf(stderr, "Operator error\n");
		return -1;
	}

	while((op = getopt(argc, argv, "f:t:")) != -1) {
		switch(op) {
		case 'f':	/*request frequency */
			REQ_freq = atoi (optarg);
			break;
		case 't':	/* frequency tolerance range */
			REQ_tolerance = atoi (optarg);
			break;
		default :
			fprintf(stderr, "Unknow Operator [%c]\n",op);
			break;
		}
	}

	/* Value check*/
	if(REQ_freq <= 0 || REQ_tolerance <0) {
		fprintf(stderr, "Argument error\n");
		return -1;
	}

	printf("Request Frequency : [%d] , Frequency Tolerance: [%d]\n", REQ_freq, REQ_tolerance);

	/* Start calcuation  */
	int hit =0;
	for(div_i = 1 ; div_i < 65536 ; div_i++) {
		if((ADC_SYSTEM_CLK / div_i) < REQ_freq)	break ;

		for(Odly_i = 0 ; Odly_i < 262144 ; Odly_i++) {
			if((ADC_SYSTEM_CLK / (div_i * (14 + Odly_i))) < REQ_freq) break ;

			for(avg_i = 0 ; avg_i < 5 ; avg_i++) {
				for(Sdly_i = 1 ; Sdly_i < 256 ; Sdly_i++) {
					hit =0;
					freq = ADC_SYSTEM_CLK / (div_i * (Odly_i + avg[avg_i] * (14 + Sdly_i))) ;

					if(freq < REQ_freq) break ;

					ob_fun = (div_i * 262144 + Odly_i) / avg[avg_i];

					if(freq == REQ_freq) {
						printf(P_GREEN"[Conform] "P_NONE);
						hit = 1;
					}
					else if(abs(freq - REQ_freq) < REQ_tolerance) {
						printf(P_RED"[Similar][%d] "P_NONE, freq);
						hit = 1;
					}
					if(hit == 1) {
						if(ob_fun > obj_max) {
							obj_max = ob_fun ;
							obj_clkdiv = div_i;
							obj_Odly = Odly_i;
							obj_Sdly = Sdly_i;
							obj_avg = avg[avg_i];
						}
						printf("Clock Divider : %d ,\t", div_i);
						printf("Open Dly : %d ,\t", Odly_i);
						printf("Sample Average : %d ,\t", avg[avg_i]);
						printf("Sample Dly : %d\n", Sdly_i);
						solution++;
					}
				}
			}
		}
	}
	printf("Feasible Solution count : %d\n",solution);
	printf("\n");
	printf("===============================================================\n");
	printf(P_GREEN"[Suggest Solution]\n"P_NONE);
	printf("\tClock Divider : %d ,\t", obj_clkdiv);
	printf("Open Dly : %d ,\t", obj_Odly);
	printf("Sample Average : %d ,\t", obj_avg);
	printf("Sample Dly : %d\n", obj_Sdly);
	printf("===============================================================\n");
}
