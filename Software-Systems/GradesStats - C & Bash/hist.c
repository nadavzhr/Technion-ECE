#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GRADE 100
#define MIN_GRADE 0

static FILE *f;
static int nbins = 10; // Default value of number of histogram bins
static int file_index; // Number of the last file input that was given by user

void build_hist(int *bins);
void anaylze_arg(int argc, char **argv);

int main(int argc, char **argv) {
    // Set bins and input format
    anaylze_arg(argc, argv);

    // Check for errors
    if (f == NULL) {
        fprintf(stderr, "File not found: \"%s\"\n", argv[file_index]);
        return 1;
    }
    // Initialize the histogram array 
    int *bins = (int*)calloc(nbins, sizeof(int));
    if (!bins) {
    	fprintf(stderr, "Memory allocation failed: exiting\n");
    	exit(1);
    }
    // Operate on the grades data
    build_hist(bins);
    free(bins);
    fclose(f);
}

/**
 * @brief Build a histogram for all the valid grades in the grades sheet,
 * by division to a number of specific bins (by input or by default value).
 * @param *bins The output array that represents the grades histogram.
 * @return Return value is void but the histogram is built and the bins array
 * is changed throught the process.
*/
void build_hist(int *bins) {
    int grade;
    int line_num = 0;
    double pace;

    // Read grades from file
    while(fscanf(f, "%d", &grade) == 1) {
        line_num++;
        // Ignore invalid grades
        if (grade < MIN_GRADE || grade > MAX_GRADE) { 
            fprintf(stderr, "Error: Invalid grade: %d on line "
             "%d\n", grade, line_num);
            continue; // Exclude invalid grade from calculations
        }
        
        /* Find the correct bin to place into */
        if (grade == MAX_GRADE) {
            bins[nbins-1]++;
        } else {
            int n = grade / (MAX_GRADE / nbins);
            bins[n]++;
        }
    }

    /* Print histogram */
    pace = 100.0 / nbins;
    for (int i=0; i < nbins; i++) {
        printf("%.0lf-%.0lf\t%d\n",
               i * pace,
               (i < nbins - 1) ? ((i+1)*pace - 1) : MAX_GRADE,
               bins[i]);
    }
}

/**
 * @brief Analayze the inputs that are given to the program.
 * @param argc number of arguments given to the program
 * @param argv array that holds the arguments given, as strings.
 * @return void, but the inputs and number of bins for the histogram
 * are being set.
*/
void anaylze_arg(int argc, char **argv) {
    f = stdin; // By default - incase the loop is unneccesary
    for(int i=1; i<argc; i++) {
        if (!strcmp(argv[i], "-")) {
            f = stdin;
        }
        // Set -n_bins flag value according to user input (if given)
        else if (!strcmp(argv[i], "-n_bins")) {
            nbins = i < argc-1 ? atoi(argv[i+1]) : 10; 
            i++; // Jump over to the next input and avoid value of n_bins
        } else {
            f = fopen(argv[i], "r");
            file_index = i;
        }
    }
}
