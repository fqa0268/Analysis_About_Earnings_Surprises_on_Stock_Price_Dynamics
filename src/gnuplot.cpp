//
//  gnuplot_linux.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Bootstrap.h>
#include <utils.h>


void plotResults(int N, Matrix avgCAAR) {
    
    FILE *gnuplotPipe=popen("gnuplot -persist", "w");;

    const char *title = "Avg CAAR for 3 groups";
    const char *yLabel = "CAAR";
    fprintf(gnuplotPipe, "set grid\n");
    fprintf(gnuplotPipe, "set title '%s'\n", title);
    fprintf(gnuplotPipe, "set arrow from 0,graph(0,0) to 0,graph(1,1) nohead lc rgb 'red'\n");
    fprintf(gnuplotPipe, "set xlabel 'Date'\nset ylabel '%s'\n", yLabel);
    fprintf(gnuplotPipe, "set xrange [-%d:%d]\n", N, N);

    
    if (gnuplotPipe){
        fprintf(gnuplotPipe,"plot '-' with lines title 'Beat', '-' with lines title 'Meet',  '-' with lines title 'Miss'\n");
        fflush(gnuplotPipe);
        
        // Beat
        for (int i = 0; i < (N*2); i++) {
            fprintf(gnuplotPipe,"%lf %lf\n",static_cast<double>(-N+i), avgCAAR[0][i]);
        }
        fprintf(gnuplotPipe, "e\n");
        // Meet
        for (int i = 0; i < (N*2); i++) {
            fprintf(gnuplotPipe,"%lf %lf\n",static_cast<double>(-N+i), avgCAAR[1][i]);
        }
        fprintf(gnuplotPipe, "e\n");
        // Miss
        for (int i = 0; i < (N*2); i++) {
            fprintf(gnuplotPipe,"%lf %lf\n",static_cast<double>(-N+i), avgCAAR[2][i]);
        }
        fprintf(gnuplotPipe, "e\n");

        //fclose(gnuplotPipe);
        pclose(gnuplotPipe);
        


    }else {
        printf("gnuplot not found...");
    }
}
