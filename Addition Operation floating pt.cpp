#include <iostream>
#include <cmath>
#include<string>

using namespace std;

/* structure to represent the IEEE 754 format - single precision (binary32)
Representation = (-1^ sign) * mantissa * (2^ exponent) */

struct IEEE754Format {
    int sign;// 1 bit for sign
    int exponent; // 8 bits for exponent
    float mantissa; // 23 bits for the multipicant
};

// Function to convert a float to IEEE 754 format
IEEE754Format convertToIEEE754(float num) {
    IEEE754Format result;
    result.sign = (num < 0) ? 1 : 0;
    
    num = abs(num);
    
    int diff;
    int exp = 0;
    
    float frac = frexp(num, &exp);// returns the multiplication factor or mantissa
    
    /* we get only 8 bits to represent the exponent 
    according to IEEE 754 single precision 
    hence we need to ensure the 
    exponent is <= 8 at all time */
    if(exp > 8){
    	diff = exp-8;
        exp = 8;
    	frac = frac* pow(2,diff);
    }
    result.exponent = exp ;
    result.mantissa = abs(frac);
    
    return result;
}

// Function to perform IEEE 754 addition
float addIEEE754(float num1, float num2, char operation) {

    // Converting to IEEE 754 format
    IEEE754Format format1 = convertToIEEE754(num1);
    IEEE754Format format2 = convertToIEEE754(num2);

    // Aligning  exponents
    int expDiff = format1.exponent - format2.exponent;
    
    if (expDiff > 0) {
        format2.exponent += expDiff;
        format2.mantissa /= pow(2, expDiff);
    } else {
        format1.exponent -= expDiff;
        format1.mantissa /= pow(2, -expDiff);
    }

    // Perform integer addition on aligned mantissas
    float resultMantissa =  operation == '+' ?
    							(format1.sign == format2.sign) ? 
                          			abs(format1.mantissa + format2.mantissa) : 
                            		abs(format1.mantissa - format2.mantissa)
                             :
                             (format1.sign == format2.sign) ? 
                          			abs(format1.mantissa - format2.mantissa) : 
                            		abs(format1.mantissa + format2.mantissa);
                                                     

    // Normalizing result, not exactly needed
    while (resultMantissa >= 2.0) {
        resultMantissa /= 2.0;
        format1.exponent++;
    }

    // Reconstructing the result
    IEEE754Format result;
    result.sign = operation == '+' ? 
    				abs(num1) >= abs(num2) ? format1.sign : format2.sign 
                    :
                  format1.sign != format2.sign ?
                  	format1.sign
                    :
                    abs(num1) > abs(num2) ?
                    format1.sign :
                    !format2.sign;
    
    result.exponent = format1.exponent;
    result.mantissa = resultMantissa;
    // Convert back to float
    return ldexp(result.mantissa, result.exponent)* (result.sign==1?-1:1) ;
            
}

int main() {
    float num1 = 3.25;
    float num2 = 7.5;
    float num3 = -10.25;
    float num4 = -7.5;
    float result1 = addIEEE754(num1, num2 , '+');
    float result2 = addIEEE754(num1, num2 , '-');
    float result3 = addIEEE754(num3, num4 , '+');
    float result4 = addIEEE754(num3, num4 , '-');

    cout << "Result of " << num1 << " + " << num2 << " = " << result1 << endl;
    cout << "Result of " << num1 << " - " << num2 << " = " << result2 << endl;
    cout << "Result of " << num3 << " + " << num4 << " = " << result3 << endl;
    cout << "Result of " << num3 << " - " << num4 << " = " << result4 << endl;


    return 0;
}
