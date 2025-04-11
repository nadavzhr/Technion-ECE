#ifndef PORT_H
#define PORT_H
#include "generic-field.h"
#include "string.h"




// Each port class will hold the legal range (FROM-TO)
// In addition, it will hold the field on which the law applies
class port : public GenericField {
    size_t high_limit;
    size_t low_limit;
    String def_rule; // Holds src-port or dst-port
    public:
    /* ADD DOXYGEN */	
    bool match(String packet);
    /* ADD DOXYGEN */
    bool set_value(String value);
    /* ADD DOXYGEN */
    void set_field(String rule);
};




#endif