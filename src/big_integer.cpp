#include "big_integer.h"
#include <iostream>

BigInteger::BigInteger(const std::string& n) {
    auto first = n.find_first_not_of("0");
    if (first != -1) {
        num = n.substr(first);
    }
    else {
        num = "0";
    }
}

BigInteger::BigInteger(const char& d) : num{ d } {}

BigInteger BigInteger::ONE = BigInteger("1");

BigInteger::operator bool() const {
	return num != "0";
}

BigInteger BigInteger::operator%(const BigInteger& a) const {
    //Should never happen, but just in calse
    if (a == BigInteger("0")) {
        throw "Divide By Zero";
    }

    if (*this < a)
        return *this;

    std::string result;
    int idx = 0;
    BigInteger leftover("0");

    BigInteger temp(num[idx]);
    while (temp < a) {
        temp = temp.timesPowerTen(1) + BigInteger(num[++idx]);
    }
    //Long division while tracking remainder
    while (idx < num.length()) {

        int multNum = 0;
        leftover = temp;
        while (a < leftover || a == leftover) {
            leftover = leftover - a;
            multNum += 1;
        }

        leftover = temp - (BigInteger(multNum + '0') * a);

        temp = leftover.timesPowerTen(1) + BigInteger(num[++idx]);
    }

    return leftover;
}

BigInteger BigInteger::operator*(const BigInteger& a) const {
    BigInteger end("0");
    std::string result;
    BigInteger temp("0");


    int carry;
    int current;

    //Elementary place multiplication algorithm
    for (long i = num.length() - 1; i >= 0; i--) {
        carry = 0;
        result = "";
        for (long j = a.num.length() - 1; j >= 0; j--) {
            current = (num[i] - '0') * (a.num[j] - '0') + carry;
            result.insert(0, 1, (current % 10 + '0'));
            carry = current / 10;
        }

        if (carry > 0)
            result.insert(0, 1, (carry + '0'));

        temp = BigInteger(result);
        end = end + temp.timesPowerTen(num.length() - i - 1);
    }

    return end;
}

bool BigInteger::operator==(const BigInteger& a) const {
    return num == a.num;
}

bool BigInteger::operator<(const BigInteger& a) const {
    if (num.length() != a.num.length()) return num.length() < a.num.length();
    
    for (long i = 0; i < num.length(); i++) {
        if (num[i] == a.num[i])continue;
        return num[i] < a.num[i];
    }
    return false;
}

BigInteger BigInteger::operator+(const BigInteger& a) const {
    //Assumes a is longer than b
    if (num.length() < a.num.length())
        return a.operator+(*this);


    unsigned long numOfZeros = num.length() - a.num.length();
    //Make this and a are in the same length
    std::string aligned = std::string(numOfZeros, '0') + a.num;

    std::string result;
    int x = 0;
    int carry = 0;
    //Elementary place addition algorithm
    for (long i = num.length() - 1; i >= 0; i--) {
        x = num[i] + aligned[i] - (2 * '0') + carry;
        carry = x / 10;
        result.insert(0, 1, (x % 10 + '0'));
    }

    //Add in carry if needed
    if (carry)
        result.insert(0, "1");

    return BigInteger(result);

}

BigInteger BigInteger::operator-(const BigInteger& a) const {
    //We will only be subtracting when a <= this
    std::string result;
    int borrow = 0;
    //Elementary place subtraction algorithm
    for (long i = 0; i < a.num.length(); i++) {
        char tVal = num[num.length() - i - 1] - borrow;
        borrow == 0;
        if (a.num[a.num.length() - i - 1] > tVal) {
            tVal += 10;
            borrow = 1;
        }
        result.insert(0, 1, (tVal - a.num[a.num.length() - i - 1] + '0'));
    }
    for (long i = a.num.length(); i < num.length(); i++) {
        char tVal = num[num.length() - i - 1] - borrow;
        borrow == 0;
        result.insert(0, 1, tVal);
    }
    auto first = result.find_first_not_of("0");
    if(first != -1)
        result = result.substr(first);
    return BigInteger(result);
}

BigInteger BigInteger::timesPowerTen(unsigned int power) const {
    //Add power number zeros to the end of num
    return BigInteger(num + std::string(power, '0'));
}