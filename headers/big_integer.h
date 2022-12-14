#include <string>

//Class to represent arbitrarily large unsigned integers for IDs
class BigInteger {
	// The string representation of the integer
	std::string num;
public:
	static BigInteger ONE;
	BigInteger(const std::string& n);
	BigInteger(const char& d);

	//Override the bool condition to set zero to falsey, everything else truthy
	explicit operator bool() const;

	//Override math operators
	BigInteger operator*(const BigInteger& a) const;
	BigInteger operator%(const BigInteger& a) const;
	BigInteger operator+(const BigInteger& a) const;
	BigInteger operator-(const BigInteger& a) const;
	bool operator<(const BigInteger& a) const;
	bool operator==(const BigInteger& a) const;
	
	//To String
	std::string getNum() const { return num; }

private:
	//Multiply by a power of 10
	BigInteger timesPowerTen(unsigned int power) const;
};
