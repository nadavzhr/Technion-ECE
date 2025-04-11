#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GRADE 100
#define MIN_GRADE 0

void get_median(FILE *f);

int main(int argc, char **argv){
    FILE *f;
    // If we got 0 arguments, or one argument that is "-", 
    // ---> input is from stdin
    if (argc == 1 || !strcmp("-", argv[1])) {
        f = stdin;
    }
    else {
        f = fopen(argv[1], "r");
    }
    // Checking for errors
    if (!f) {
        fprintf(stderr, "File not found: \"%s\"\n", argv[1]);
        return 1;
    }

    get_median(f);
}

/**
 * @brief Calculate the median of all the valid grades given in a file.
 * @param f A file containing all the grades. Should be a txt file
 * @return Prints the expected median.
*/
void get_median (FILE *f) {
    int grades[MAX_GRADE] = {0};
    int line_num = 0;
    int median_location;
    int grade;
    int num_valids = 0;

    // Read grades from file
    while(fscanf(f, "%d", &grade) == 1) {
        line_num++;
        // Ignore invalid grades
        if (grade < MIN_GRADE || grade > MAX_GRADE) { 
            fprintf(stderr, "Error: Invalid grade: %d on line "
             "%d\n", grade, line_num);
            continue; // Exclude invalid grade from calculations
        }
        num_valids++;
        grades[grade]++;
    }

    // Calculate the median by locating it in the array
    median_location = ((num_valids + 1)/2); // Actual mathematical location
    for (int j=0; j<=100;j++){
        median_location -= grades[j]; // Decrement until zero
        if(median_location <= 0) { // Means we got to the median loaction.
            printf("%d\n", j); 
            break;
        }
    }
}
