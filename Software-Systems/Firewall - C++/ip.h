#ifndef IP_H
#define IP_H

#include "generic-field.h"
#include "string.h"


class Ip : public GenericField {
    private:
        bool mask[32]; // 32bit binary bit array
        int num_rlvnt_bits; // number of bits in mask
        String rlvnt_field; // Holds "src-ip" or "dst-ip"
    public :
    	/**
         * @fn bool match(String packet)
         * @brief Given a packet that holds fields of ports & ip,
         * it checks if the packet fulfills the rule that was set on "this"
         * instance of the Ip class.
         * @param packet the packet to be checked.
         * @return TRUE if the packet fulfills the rule, FALSE otherwise.
        */
        bool match(String packet);
        
        /** 
         * @fn bool set_value(String value)
         * @brief Given a value (a rule set on an ip field),
         * it associates the rule to the instance of the class.
         * meaning it sets the given rule.
         * @param value the rule to be set.
         * @return TRUE if all operations were completed with no errors,
         * FALSE otherwise.
        */
        bool set_value(String value);
        /**
         * @fn void set_field(String rule)
         * @brief  Initializes the rlvnt_field class member of the Ip class,
         * sets it to either src-ip or dst-ip, according to the rule.
         * @param rule the rule given to the function.
         * @return void, but rlvnt_field is modified.
        */
        void set_field(String rule);

};


#endif