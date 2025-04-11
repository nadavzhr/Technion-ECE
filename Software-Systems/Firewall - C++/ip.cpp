#include "ip.h" // ip object interface
#include "string.h" // for parsing input
#include <cstring>
#include <iostream>

/* Define constants */
// Longest possible ip rule: XXX.XXX.XXX.XXX + '\0'
// Seperate terms are 4 bytes + mask = tot 5
const int DONT_CARE = 0;
const int RULE_TERMS_NUM = 5;
const int PACKET_TERMS = 8;
const int NUM_BYTES = 4;
const int INT_BITS = 32;

/*** AUXILIARY FUNCTIONS ***/
static void convert_ip_to_binary(const int* bytes, int* binary);



/*** Ip class methods implementation ***/

bool Ip::set_value(String value) {

    // Set groundwork elements for parsing input
    String *output;
    const char* delimiter = "./";
    size_t size;
    String tmp;

    tmp = value.trim();
    tmp.split(delimiter, &output, &size);
    // Now, output contains:
    //  1st  2nd  3rd  4th  mask size
    // [XXX][XXX][XXX][XXX][XX]
    // Each element is a class String

    // Split method failed == ERROR
    if(size != RULE_TERMS_NUM || output == nullptr) {
        delete[] output;
        return false;
    }

    // Size of the mask - last String in the output array
    num_rlvnt_bits = output[size-1].to_integer();

    
    /*** Assign the mask rule. ***/
    // Convert the IP address bytes to integers
    int bytes[NUM_BYTES] = {0};
    for (int i = 0; i < NUM_BYTES; i++) {
        bytes[i] = output[i].to_integer();
    }
    // bytes array is:
    // [XXX][XXX][XXX][XXX][XX]
    // Each element is an int

    // Represent the ip address in binary - 32bits.
    int binary[INT_BITS] = {0};
    convert_ip_to_binary(bytes, binary);
    /* binary now holds the binary representation of the ip address */

    // Set up this->mask member field as a zeros array
    for (int i = 0; i < INT_BITS; i++) {
        mask[i] = 0;
    }
    // Finally - set the mask field
    for (int i = 0; i < INT_BITS; i++) {
        if (i < num_rlvnt_bits) {
            mask[i] = binary[i];
        }
        else {
            mask[i] = DONT_CARE;
        }
    }

    delete[] output;
    return true;

}

bool Ip::match(String packet) {

    // Set groundwork elements for parsing the packet
    String *output;
    const char* delimiter = ",=";
    size_t size;
    String tmp;

    // Split to fields and field values/ranges
    packet.split(delimiter, &output, &size);
    // Check for errors
    if(size != PACKET_TERMS || output == nullptr) {
        delete[] output; 
        return false;
    }
    
    String tmp_ip;
    // Locate the relevant field inside the packet
    for (size_t i = 0; i < size; i++) {

        output[i] = output[i].trim();
        if (output[i].equals(rlvnt_field)) {
            
            tmp_ip = output[i+1];
        }
    }

    // Parse the ip address
    String *ip_output;
    const char* ip_delimiter = ".";
    size_t ip_size;

    tmp_ip.split(ip_delimiter, &ip_output, &ip_size);    

    // Convert the relevant IP address bytes to integers
    int bytes[NUM_BYTES] = {0};
    for (int i = 0; i < NUM_BYTES; i++) {
        bytes[i] = ip_output[i].to_integer();
    }

    int pckt_binary[INT_BITS] = {0};
    convert_ip_to_binary(bytes, pckt_binary);

    // Check if the given packet fulfills the rule
    for (int i = 0; i < num_rlvnt_bits; i++) {
        if(mask[i] != pckt_binary[i]) {
            delete[] output;
            delete[] ip_output;
            return false;
        }
    }

    // Clean up
    delete[] output;
    delete[] ip_output;
    // Got here - all good, return true
    return true;
}


void Ip::set_field(String rule) {
    String tmp = rule.trim();
    rlvnt_field = tmp;
}



/*** AUXILIARY FUNCTION IMPLEMENTATION ***/
// Fills a binary 32bit array of the corresponding ip address
static void convert_ip_to_binary(const int* bytes, int* binary)
{
    for (int i = 0; i < 4; i++)
    {
        int currentByte = bytes[i];
        int currentIndex = i * 8;

        for (int j = 7; j >= 0; j--)
        {
            binary[currentIndex + j] = currentByte % 2;
            currentByte /= 2;
        }
    }
}


