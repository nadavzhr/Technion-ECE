#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GRADE 100
#define MIN_GRADE 0

void get_grades_mean(FILE *f);

int main(int argc, char **argv){
    FILE *f;
    // 0 arguments, or one argument that is "-", 
    // ---> input is from stdin
    if(argc == 1 || !strcmp("-", argv[1])) {
        f = stdin;
    }
    else {
        f = fopen(argv[1], "r");
    }
    // Checking for errors
    if(f == NULL) {
        fprintf(stderr, "File not found: \"%s\"\n", argv[1]);
        return 1;
    }

    get_grades_mean(f);
    fclose(f);
    return 0;
}


/**
 * @brief Calculate the mean of all the valid grades given in a file.
 * @param f A file containing all the grades. Should be a txt file.
 * @return Prints the expected mean.
*/
void get_grades_mean(FILE *f) {
    int grade;
    double mean;
    int sum = 0;
    int line_num = 0;
    int num_valids = 0;

    /* Read in grades from the txt file */
    while(fscanf(f, "%d", &grade) == 1) {
        line_num++;
        // Ignore invalid grades
        if (grade < MIN_GRADE || grade > MAX_GRADE) { 
            fprintf(stderr, "Error: Invalid grade: %d on line "
             "%d\n", grade, line_num);
            continue; // Exclude invalid grade from calculations
        }
        num_valids++;
        sum += grade;
    }

    /* Calculate the mean */
    mean = (double)sum / num_valids;
    printf("%.2lf\n", mean);
}
