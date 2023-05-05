// $Id: bigint.cpp,v 1.76 2016-06-14 16:34:24-07 - - $
// bigint.cpp
// Andy Huang (ahuang44@ucsc.edu)
// Kody Fong (kbfong@ucsc.edu)

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;
#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint(long that): uvalue(that), is_negative(that < 0) {
    DEBUGF('~', this << " -> " << uvalue)
}

bigint::bigint(const ubigint & uvalue, bool is_negative):
    uvalue(uvalue), is_negative(is_negative) {}

bigint::bigint(const string & that) {
    is_negative = that.size() > 0 and that[0] == '_';
    uvalue = ubigint(that.substr(is_negative ? 1 : 0));
}

bigint bigint::operator + () const {
    return *this;
}

bigint bigint::operator - () const {
    return {
        uvalue,
        !is_negative
    };
}

bigint bigint::operator + (const bigint & that) const {
    //if same sign then just add normally
    if (is_negative == that.is_negative)
        return {
            uvalue + that.uvalue,
            is_negative
        };
    else { //else check which one is the negative one
        if (uvalue < that.uvalue) //uvalue<that.uvalue 
            //thus result has to be positive 
            //bigger number-smaller(negative number) 
            return {
                that.uvalue - uvalue,
                that.is_negative
            };
        else if (uvalue > that.uvalue) //uvalue>that.uvalue 
            //that.uvalue is negative so result is uvalue-that.uvalue. 
            //takes the sign of the bigger #
            return {
                uvalue - that.uvalue,
                is_negative
            };
    }
    //not same sign, and not bigger or smaller.
    //thus same number but one is pos and one is neg
    return {ubigint {0},false};
}

bigint bigint::operator - (const bigint & that) const {
    //check if same signs 
    if (is_negative == that.is_negative) {
        //check which number is the smaller one
        if (uvalue < that.uvalue)
            //since uvalue is smaller just subtract it from that.uvalue
            //take the opposite of the uvalue's sign
            return {
                that.uvalue - uvalue,
                !is_negative
            };
        else if (uvalue > that.uvalue)
            //since uvalue is bigger just subtract that.uvalue from it 
            //take the uvalue's sign because it was the bigger number
            return {
                uvalue - that.uvalue,
                is_negative
            };
        else
            //else they are the same sign and same number 
            //thus difference is 0
            return {ubigint{0},false};
    }
    //
    return {uvalue + that.uvalue, is_negative};
}

bigint bigint::operator * (const bigint & that) const {
    //if the signs are the same then the result of the multiplication 
    //between both numbers will be pos
    if (is_negative == that.is_negative) {
        return {
            uvalue * that.uvalue,
            false
        }; 

    }
    //if the signs are different then the result of the multiplication 
    //between both numbers will be negative

    return {
        uvalue * that.uvalue,
        true
    }; 
    
}

bigint bigint::operator / (const bigint & that) const {
    if (is_negative == that.is_negative) {
        return {
            uvalue / that.uvalue,
            false
        }; //if the signs are the same then the 
        //result of the division between both numbers will be pos
    }

    return {
        uvalue / that.uvalue,
        true
    }; //if the signs are different then the 
    //result of the division between both numbers will be negative

}

bigint bigint::operator % (const bigint & that) const {
    if (is_negative == that.is_negative) {
        if (that.is_negative == true) {
            //same sign but negative thus negative result
            return {
                uvalue % that.uvalue,
                true
            };
        } else {
            //same sign but positive thus positive result
            return {
                uvalue % that.uvalue,
                false
            };
        }
    }
    //not the same signs
    //if x%y if y is negative, result has to be negative
    if (that.is_negative == true) {
        return {
            uvalue % that.uvalue,
            true
        };
    }
    // y is not negative thus result not negative
    return {
        uvalue % that.uvalue,
        false
    }; //if the signs are the same then the result

}

bool bigint::operator == (const bigint & that) const {
    //signs and numbers are same
    return is_negative == that.is_negative and uvalue == that.uvalue; 
}

bool bigint::operator < (const bigint & that) const {
    if (is_negative != that.is_negative) {
        return is_negative;
    }
    return is_negative ? that.uvalue < uvalue :
        uvalue < that.uvalue;
}

ostream & operator << (ostream & out,
    const bigint & that) {
    return out << (that.is_negative ? "-" : "") << that.uvalue;
}
