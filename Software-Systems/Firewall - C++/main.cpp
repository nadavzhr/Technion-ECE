#include <stddef.h> // size_t
#include <iostream>
#include <cstring> // strlen, strcpy
#include "string.h"
#include "input.h"
#include "ip.h"
#include "port.h"


const int FIRST_ARG = 0;
const int SECOND_ARG = 1;
const int TWO_PARTS = 2;

int main(int argc, char* argv[]) {
    
    /* Check validity of program parameters */
    if(check_args(argc, argv)) {
        exit(1);
    }

    /* Input is valid - operate */
    // Save the rule given by parameter
    String rule = String(argv[1]);
    
    // Trim the rule
    String tmp = rule.trim();
    // Split into [FIELD][VALUE]
    String *output;
    size_t size;
    const char* delimiter = "=";
    tmp.split(delimiter, &output, &size);

    if (size != TWO_PARTS) {
        // Error - do something
    }

    // Divide the field into [SRC/DST][FIELD TYPE]
    String* divided_field;
    const char* delimiter2 = "-";
    // first arg is now src-field or dst-field
    output[FIRST_ARG].split(delimiter2, &divided_field, &size);
    // Trim the strings to enable comparisons 
    divided_field[FIRST_ARG] = divided_field[FIRST_ARG].trim();
    divided_field[SECOND_ARG] = divided_field[SECOND_ARG].trim();

    // If the rule is set on an IP field
    if (divided_field[SECOND_ARG].equals("ip")) {

        Ip ip_mask;
        ip_mask.set_field(output[FIRST_ARG]);
        ip_mask.set_value(output[SECOND_ARG]);
        parse_input(ip_mask);
    }

    // If the rule is set on a PORT field
    if(divided_field[SECOND_ARG].equals("port")){
        port port_rule;
        port_rule.set_field(output[FIRST_ARG]);
        port_rule.set_value(output[SECOND_ARG]);
        parse_input(port_rule);
    }

    delete[] output;
    delete[] divided_field;

    return 0;
}
