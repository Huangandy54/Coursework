// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $
// ubigint.cpp
// Andy Huang (ahuang44@ucsc.edu)
// Kody Fong (kbfong@ucsc.edu) 

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;
#include "ubigint.h"
#include "debug.h"

ubigint::ubigint(unsigned long that) {
    while (that > 0) {
        ubig_value.push_back(that % 10);
        that /= 10;
    }
}

ubigint::ubigint(const string & that) {
    for (char digit: that) {
        if (not isdigit(digit)) {
            throw invalid_argument("ubigint::ubigint(" + that + ")");
        }
        ubig_value.insert(ubig_value.begin(), digit - '0');
    }
    //eliminate extra 0s
    while (ubig_value.size() > 0 && ubig_value.back() == 0){
        ubig_value.pop_back();
    }
}

ubigint ubigint::operator + (const ubigint & that) const {
    ubigint result {0};
    unsigned int i {0}; //counter
    bool carry {0}; //keeps track of carry over 
    int total {0}; //contains the sum for every addition of pairs
     //determine which is larger
    unsigned int length = (ubig_value.size() < that.ubig_value.size() ?
        ubig_value.size() : that.ubig_value.size());
    while (i < length || carry > 0) {
        if (i < ubig_value.size())
            total += ubig_value[i];
        if (i < that.ubig_value.size())
            total += that.ubig_value[i];
        total += carry;
        carry = 0;
        if (total > 9) //if sum is > 9 add carry to next digit
        {
            carry = 1;
            total -= 10;
        }
        result.ubig_value.push_back(total);
        total = 0; ////done with the shorter number
        i++;
    }
    //first digit is longer
    if (ubig_value.size() > that.ubig_value.size()) {
        while (i < ubig_value.size()) {
            result.ubig_value.push_back(ubig_value[i]);
            i++;
        }
    }
    //second digit is longer
    else if (ubig_value.size() < that.ubig_value.size()) {
        while (i < that.ubig_value.size()) {
            result.ubig_value.push_back(that.ubig_value[i]);
            i++;
        }
    }
    return result;
}

ubigint ubigint::operator - (const ubigint & that) const {
    if ( * this < that) throw domain_error("ubigint::operator-(a<b)");
    ubigint result {
        0
    };
    unsigned int i {
        0
    }; //keeps track of carry over value
    bool borrow {
        0
    }; //keeps track of borrow
    int total {
        0
    }; //contains the difference for each subtraction of pairs
    while (i < that.ubig_value.size()) {
        total = ubig_value[i] - borrow;
        borrow = 0;
        //a borrow needs to occur so
        if (total < that.ubig_value[i]) {
            //add 10 and set borrow to 1
            total += 10;
            borrow = 1;
        }
        result.ubig_value.push_back(total - that.ubig_value[i]);
        i++;
    }
    while (i < ubig_value.size()) {
        //if theres a borrow
        if (borrow > 0) {
            result.ubig_value.push_back(ubig_value[i] - 1);
            borrow = 0; //set the borrow back to 0
        } else
            result.ubig_value.push_back(ubig_value[i]);
        i++;
    }
    //eliminate extra 0s
    while (result.ubig_value.size() > 0 && 
            result.ubig_value.back() == 0) {
        result.ubig_value.pop_back();
    }

    return result;
}

//asg1.pdf format
ubigint ubigint::operator * (const ubigint & that) const {
    ubigint result {
        0
    };
    for (unsigned int i = 0; i < ubig_value.size(); i++) {
        ubigint current {
            0
        };
        int product {
            0
        };
        int carry {
            0
        };
        for (unsigned int j = 0; j < that.ubig_value.size(); j++) {
            product = (ubig_value[i] * that.ubig_value[j]) + carry;
            carry = 0;
            if (product > 9) {
                carry = product / 10;
                product = product % 10;
            }
            current.ubig_value.push_back(product);
        }
        if (carry > 0) {
            current.ubig_value.push_back(carry);
            carry = 0;
        }
        for (unsigned int k = 0; k < i; k++)
            current.ubig_value.insert(current.ubig_value.begin(), 0);
        result = result + current;
    }
    return result;
}

void ubigint::multiply_by_2() {
    ( * this) = ( * this) + ( * this);
}

void ubigint::divide_by_2() {
    int carry = 0;
    // go from the back
    for (int i = ubig_value.size() - 1; i >= 0; i--) {
        int digitQuotient = 0;
        //int current=ubig_value[i];
        //cout << "current = " << current << endl;
        // divide by 2
        digitQuotient += ubig_value.at(i) / 2;
        // if (ubig_value.at(i-1) is odd) 
        // carry =5
        if (i != 0 && ubig_value.at(i - 1) % 2 == 1) { 
            int next = ubig_value.at(i - 1);
            //cout << "next = " << next << endl;
            carry = 5;
        }
        //cout << "carry = " << carry<< endl;
        if (carry > 0) {
            digitQuotient += carry;
            carry = 0;
        }
        //cout << "new = " << digitQuotient << endl;
        ubig_value.at(i) = digitQuotient;
    }

    while (ubig_value.size() > 0 and ubig_value.back() == 0) {
        ubig_value.pop_back();
    }
}

ubigint::quot_rem ubigint::divide(const ubigint & that) const {
    ubigint zero {
        0
    };
    if (that == zero) throw domain_error("udivide by zero");
    ubigint power_of_2 {
        1
    };
    ubigint quotient {
        0
    };
    ubigint divisor = that;
    ubigint remainder = * this; //left operand, dividend
    while (divisor < remainder) {
        divisor.multiply_by_2();
        power_of_2.multiply_by_2();
    }
    while (power_of_2 > zero) {
        if (divisor <= remainder) {
            remainder = remainder - divisor;
            quotient = quotient + power_of_2;
        }
        divisor.divide_by_2();
        power_of_2.divide_by_2();
    }
    return {
        quotient,
        remainder
    };
}

ubigint ubigint::operator / (const ubigint & that) const {
    return divide(that).first;
}

ubigint ubigint::operator % (const ubigint & that) const {
    return divide(that).second;
}

bool ubigint::operator == (const ubigint & that) const {
    if (ubig_value.size() == that.ubig_value.size()) {
        for (unsigned int i = 0; i < ubig_value.size(); i++) {
          //values arent same
          //so return false
            if (ubig_value[i] != that.ubig_value[i])
                return false;
        }
        //values are the same so return true
        return true;
    }
    //values arent the same
    //size so automatically false
    return false;
}

bool ubigint::operator < (const ubigint & that) const {
    if ( * this == that)
    //if equal then
    //one can't be less than
        return false;
    if (ubig_value.size() < that.ubig_value.size())
    //the value of left value
    //is less than right value
    //so return true
        return true;
    else if (ubig_value.size() == that.ubig_value.size()) {
        for (int i = ubig_value.size() - 1; i >= 0; i--) {
            if (ubig_value[i] > that.ubig_value[i])
              //if left value is less
              //than right value return false
                return false;
        }
      //returns true since
      //left value was already determined
      //not less than right value
        return true;
    }
    return false;
}

ostream & operator << (ostream & out,
    const ubigint & that) {
    int line = 70; //length of line to wrap around
    if (that.ubig_value.size() > 0) {
        int count = 0; //keeps track of how many characters there are
        for (int i = that.ubig_value.size() - 1; i >= 0; i--) {
          //if the count reaches the line limit
            if (count % (line - 1) == 0 && count != 0) 
                out << "\\" << endl;
            out << static_cast < char > (that.ubig_value.at(i) + '0');
            count++;
        }
    } else
        out << '0';
    return out;
}
