#include "pch.h"
#include "bin_protocol.h"


void bin_protocol::pack()
{
	std::string tmp = "";
	tmp += OPERACJA.to_string();
	tmp += LICZBA1.to_string();
	tmp += LICZBA2.to_string();
	tmp += STATUS.to_string();
	tmp += ID.to_string();
	tmp += WYNIK.to_string();
	tmp += DOP.to_string();

	std::vector<std::string> bajty;

	for (int i = 0; i <= 208; i += 8) {
		bajty.push_back(tmp.substr(i, 8));
	}

	int licznik = 0;
	for (auto e : bajty) {
		pakiet[licznik] = bit_to_int_conv(e);
		licznik++;
	}

}

void bin_protocol::unpack()
{
	std::string tmp = "";
	std::bitset<8> bity;

	for (int i = 0; i < 27; i++) {
		bity = pakiet[i];
		tmp += bity.to_string();
	}

	OPERACJA = bit_to_int_conv(tmp.substr(0, 2));
	LICZBA1 = bit_to_int_conv(tmp.substr(2, 64));
	LICZBA2 = bit_to_int_conv(tmp.substr(66, 64));
	STATUS = bit_to_int_conv(tmp.substr(130, 2));
	ID = bit_to_int_conv(tmp.substr(132, 16));
	WYNIK = bit_to_int_conv(tmp.substr(148, 64));
	DOP = bit_to_int_conv(tmp.substr(212, 4));
}


long long bin_protocol::bit_to_int_conv(const std::string &s) {
	static const std::size_t MaxSize = CHAR_BIT * sizeof(long long);
	if (s.size() > MaxSize) return 0;

	std::bitset<MaxSize> bits;
	std::istringstream is(s);
	is >> bits;
	return bits.to_ullong();
}

void bin_protocol::display()
{
	int op = (int)OPERACJA.to_ulong();
	long long l1 = (long long)LICZBA1.to_ulong();
	long long l2 = (long long)LICZBA2.to_ulong();
	int status = (int)STATUS.to_ulong();
	int id = (int)ID.to_ulong();
	long long wynik = (long long)WYNIK.to_ulong();
	int dop = (int)DOP.to_ulong();

	std::cout << id << " " << l1 << " " << l2 << " " << op << " " << status << " " << wynik << " " << dop << std::endl;
}

bin_protocol::bin_protocol(int op, long long l1, long long l2, int id)
{
	OPERACJA = op;
	LICZBA1 = l1;
	LICZBA2 = l2;
	ID = id;
}

bin_protocol::~bin_protocol()
{
}
