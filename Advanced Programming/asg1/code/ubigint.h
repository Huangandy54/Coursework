// $Id: ubigint.h,v 1.11 2016-03-24 19:43:57-07 - - $
// bigint.cpp
// Andy Huang (ahuang44@ucsc.edu)
// Kody Fong (kbfong@ucsc.edu)

#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"

class ubigint {
   friend ostream& operator<< (ostream&, const ubigint&);
   private:
     // using unumber = unsigned long;
     // unumber uvalue {};
        using udigit_t = unsigned char;
        using ubigvalue_t = vector<udigit_t>;
        ubigvalue_t ubig_value;
        using quot_rem = pair <ubigint, ubigint>;
        quot_rem divide (const ubigint&) const;
   public:
      void multiply_by_2();
      void divide_by_2();

      ubigint() = default; // Need default ctor as well.
      ubigint (unsigned long);
      ubigint (const string&);
      ubigint (vector<unsigned char>);

      ubigint operator+ (const ubigint&) const;
      ubigint operator- (const ubigint&) const;
      ubigint operator* (const ubigint&) const;
      ubigint operator/ (const ubigint&) const;
      ubigint operator% (const ubigint&) const;

      bool operator== (const ubigint&) const;
      bool operator<  (const ubigint&) const;
};

#endif

