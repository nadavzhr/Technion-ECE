#include <stddef.h> // size_t
#include <iostream>
#include <cstring> // strlen, strcpy
#include "string.h"


/*** CONSTRUCTORS ***/

String::String() : data(new char[1]), length(0) {

    data[0] = '\0';
}

String::String(const String &str)
: data(new char[str.length + 1]), length(str.length)
{
    strcpy(data, str.data);
}


String::String(const char *str)
: data(new char[strlen(str) + 1]), length(strlen(str))
{
    strcpy(data, str);

}


/*** DESTRUCTOR ***/

String::~String() {
    // Deallocate memory related to the class
    delete[] this->data;
}

/*** OPERATOR= ***/

String& String::operator=(const String &rhs) {
    if (this == &rhs) {
        return *this; // Check for self-assignment
    }

    // Deallocate the existing data (if any)
    delete[] data;

    // Copy the length
    length = rhs.length;

    // Allocate memory for the new data
    data = new char[length + 1];

    // Copy the data from rhs to this object
    strcpy(data, rhs.data);

    return *this; // Return the updated object
}


String& String::operator=(const char* str) {
    // Check for self-assignment
    if (strcmp(data, str) == 0) {
        return *this;
    }
    // Deallocate the existing data (if any)
    delete[] data;

    // Calculate the length of the input string
    length = strlen(str);

    // Allocate memory for the new string
    data = new char[length + 1];

    // Copy the input string to the data member
    strcpy(data, str);

    return *this;
}

/*** EQUALS  ***/

bool String::equals(const String& rhs) const {
    // Compare both Strings.data and check for diff.
    if(!strcmp(data, rhs.data)) {
        return true;
    }
    return false;
}

bool String::equals(const char *rhs) const {
    // Compare the two strings and check for diff.
    if(!strcmp(data, rhs)) {
        return true;
    }
    return false;
}


/***  SPLIT METHOD ***/

void String::split(const char *delimiters, String **output,
     size_t *size) const {

        char* dataCopy = new char[length + 1];
        strcpy(dataCopy, data);

        // Count the number of tokens by iterating over the string
        size_t count = 0;
        char* token = strtok(dataCopy, delimiters);
        while (token != nullptr) {
            ++count;
            token = strtok(nullptr, delimiters);
        }

        // If output is NULL, only compute the size
        if (output == nullptr) {
            *size = count;
            delete[] dataCopy;
            return;
        }

        // Allocate memory for the output array
        *output = new String[count]; // changed this - 06.04

        // Tokenize the string again and store the tokens in the output array
        strcpy(dataCopy, data);
        token = strtok(dataCopy, delimiters);
        for (size_t i = 0; i < count; ++i) {
            (*output)[i] = String(token);
            token = strtok(nullptr, delimiters);
        }
        // Assign size and delete the tmp memory allocation.
        *size = count;
        delete[] dataCopy;
    }


/*** TO INTEGER METHOD ***/

int String::to_integer() const {
    return atoi(data);
}


/*** TRIM METHOD ***/

String String::trim() const 
{
    size_t start = 0;
    size_t end = length - 1;

    // Find the first non-space character from the beginning
    while (start < end && data[start] == ' ') {
        start++;
    }

    // Find the first non-space character from the end
    while (end > start && data[end] == ' ') {
        end--;
    }
    char* new_data = new char[end - start + 2];
    strncpy(new_data, data + start, end - start + 1);
    new_data[end - start + 1] = '\0';
    // Build the output object of type class String
    String trimmedString = String(new_data);
    delete[] new_data;
    // Return the trimmed string
    return trimmedString;
}