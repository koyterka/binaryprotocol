#pragma once
#include <bitset>
#include <vector>
#include <string>
#include <iostream>
#include <climits>
#include <sstream>

class bin_protocol {

public:
	std::bitset<2> OPERACJA;
	std::bitset<64> LICZBA1;
	std::bitset<64> LICZBA2;
	std::bitset<2> STATUS;
	std::bitset<16> ID;
	std::bitset<64> WYNIK;
	std::bitset<4> DOP;
	char pakiet[27];
	void pack();
	void unpack();
	long long bit_to_int_conv(const std::string &s);
	void display();

	bin_protocol(int op, long long l1, long long l2, int id);
	~bin_protocol();
};
