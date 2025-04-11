#include <stddef.h> // size_t
#include <iostream>
#include <cstring> // strlen, strcpy
#include "string.h"
#include "port.h"


const int LOW = 0;
const int HIGH = 1;
const int RANGE_FIELDS = 2;


bool port::set_value(String value) {
    // Initialize a pointer to an empty list of pointers to Strings.
    String *output;
    // This value will later be filled with the number of puckets.
    size_t size;
    // Set the delimiters of port.
    const char* delimiters = "-";  

    int lower_lim, higher_lim;

    String tmp;

    tmp = value.trim();

    tmp.split(delimiters, &output, &size);

    if (size != RANGE_FIELDS || nullptr == output ) {
        return false;
    }

    // Cutting spaces in all of the trimmed objects.
    for (size_t i = 0; i < size; i++) {
        tmp = output[i].trim();
        output[i] = tmp;
    }

    lower_lim = output[LOW].to_integer();
    higher_lim = output[HIGH].to_integer();


    this->low_limit = lower_lim;
    this->high_limit = higher_lim;

    delete[] output;
    return true;
}

bool port::match(String packet) {
    // Initialize with a default value
    size_t current_port = 0;
    size_t high = 0;
    size_t low = 0;
    
     // Initialize a pointer to an empty list of pointers to Strings.
    String *output;
    // This value will later be filled with the number of puckets.
    size_t size;
    // Set the delimiters of port.
    const char* delimiters = "=,";  

    String rule = this->def_rule;

    packet.split(delimiters, &output, &size);
    String tmp;

    // Cutting spaces in all of the trimmed objects.
    for (size_t i = 0; i < size; i++) {
        tmp = output[i].trim();
        output[i] = tmp;
    }

    for (size_t i = 0; i < size; i++) {
        if (output[i].equals(rule)) {
            /* After the deletion of all spaces and "=" marks, 
            the port will be the next String after "dst/src-port". */
                        current_port = output[i + 1].to_integer();
        }
    }

    high = this->high_limit;
    low = this->low_limit;
    // Check if the given packets fulfills the rule
    if ((current_port <= high) && (current_port >= low)) {
        delete[] output;
        return true;
    }

    delete[] output;
    return false;
}

void port::set_field(String rule) {
    String tmp = rule.trim();
    this->def_rule = tmp;
}