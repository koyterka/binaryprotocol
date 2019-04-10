#include "pch.h"
#include <iostream>
#include<string>
#include "bin_protocol.h"
#include<WS2tcpip.h>
#include<fstream>

#pragma comment(lib, "Ws2_32.lib")

std::string lastsearch() {
	std::ifstream historia;
	historia.open("historia.txt");
	if (historia.good() == true)
	{
		historia.seekg(-1, std::ios_base::end);

		bool keepLooping = true;
		while (keepLooping) {
			char ch;
			historia.get(ch);

			if ((int)historia.tellg() <= 1) {
				historia.seekg(0);
				keepLooping = false;
			}
			else if (ch == '\n') {
				keepLooping = false;
			}
			else {
				historia.seekg(-2, std::ios_base::cur);
			}
		}

		std::string lastLine;
		std::getline(historia, lastLine);

		historia.close();
		return lastLine;
	}
}

void exit(SOCKET sock) {
	//zamkniecie polaczenia
	closesocket(sock);
	WSACleanup();
}

void operation(SOCKET sock, sockaddr_in hint, int op, int idses) {
	//ustalanie identyfikatora obliczenia
	std::string idses_s = std::to_string(idses);
	std::string last = lastsearch();
	int idobl;
	int lastobl = std::stoi(last.substr(2, 3));
	if (std::stoi(last.substr(0, 2)) != idses) idobl = 100;
	else idobl = lastobl + 1;

	std::string idobl_s = std::to_string(idobl);
	std::string wholeid_s = idses_s + idobl_s;
	int wholeid = std::stoi(wholeid_s);

	std::cout << "Identyfikator sesji: " << idses << std::endl;
	std::cout << "Identyfikator obliczenia: " << idobl << std::endl;

	//pobranie wartosci od uzytkownika
	std::cout << "Podaj dwie liczby, na ktorych chcesz wykonac operacje" << std::endl;

	long double l1, l2;
	bool cond = false;

	do {																			//obsluga podania blednych danych
		std::cout << "> ";
		std::cin >> l1 >> l2;
		unsigned long long int max = INT64_MAX;


		if (l1 < 0 || l2 < 0) {
			std::cout << std::endl << "Podaj liczby wieksze od 0. " << std::endl;
			continue;
		}

		else if (l1 > max || l2 > max) {
			std::cout << std::endl << "Liczba jest za duza. " << std::endl;
			continue;
		}

		else if (l1 != (long long)l1 || l2 != (long long)l2) {
			std::cout << std::endl << "Podaj liczby calkowite. " << std::endl;
			continue;
		}

		else {cond = true; }

	} while (cond == false);

	//wysylanie i odbieranie danych
	bin_protocol bin(op, (long long)l1, (long long)l2, wholeid);
	bin.pack();

	int sendResult = send(sock, bin.pakiet, sizeof(bin.pakiet), 0);

	if (sendResult != SOCKET_ERROR) {
		bin_protocol bin2(0, 0, 0, 0);
		int recvResult = recv(sock, bin2.pakiet, sizeof(bin.pakiet), 0);
		if (recvResult > 0) {
			bin2.unpack();

			int op = (int)bin2.OPERACJA.to_ulong();
			int status = (int)bin2.STATUS.to_ulong();
			int id = (int)bin2.ID.to_ulong();
			long long wynik = (long long)bin2.WYNIK.to_ullong();

			int dop = (int)bin2.DOP.to_ulong();

			//wpisywanie do historii
			std::ofstream historia;
			historia.open("historia.txt", std::ofstream::app);
			if (historia.good() == true)
			{
				historia << std::endl << id << " " << l1 << " " << l2 << " " << op << " " << status << " " << wynik;
			} historia.close();


			//wyswietlanie wyniku
			std::cout << "SERVER > ";
			switch (status) {
			case 0:
			{
				int operacja = (int)bin2.OPERACJA.to_ulong();
				switch (operacja) {
				case 0:
				{
					std::cout << l1 << "^" << l2 << " = " << wynik << std::endl;
					break;
				}

				case 1:
				{
					std::cout << l1 << "log" << l2 << " = " << wynik << std::endl;
					break;
				}

				case 2:
				{
					std::cout << l1 << "mod" << l2 << " = " << wynik << std::endl;
					break;
				}
				case 3:
				{
					std::cout << l1 << "/" << l2 << " = " << wynik << std::endl;
					break;
				}
				}
				break;
			}

			case 1:
			{
				std::cout << "Podano niepoprawne dane.";
				break;
			}

			case 2:
			{
				std::cout << "Wynik obliczen wykracza poza zakres zmiennej.";
				break;
			}
			case 3:
				std::cout << "Operacja nie istnieje.";
				break;
			}
		}
	}
	idobl++;

}

//przegladanie historii
int history() {
	bool discover = true;

		do {
			std::ifstream historia;
			historia.open("historia.txt");

			if (historia.good() == true){
			int token;
			std::cout << std::endl << "Podaj identyfikator sesji lub identyfikator obliczenia: ";
			std::cin >> token;

			std::string linia;
			std::string first;
			std::string first2;
			
			bool caption = false;
			std::string token_s = std::to_string(token);

			if (token_s.size() == 2) {
				//wyszukiwanie po id sesji
				int licznik = 0;

				while (std::getline(historia, linia)) {
					first = linia.substr(0, linia.find(" "));
					first2 = first.substr(0, 2);
					int pom1 = std::stoi(first2, nullptr, 10);
					if (pom1 == token) {
						if (caption == false) {
							std::cout << "ID    ||    LICZBA 1     ||    LICZBA 2    ||    OP    ||    STATUS    ||    WYNIK" << std::endl;
							caption = true;
						}
						std::cout << linia << std::endl;
						licznik++;
					}
				} if (licznik == 0) std::cout << "Nie znaleziono w historii wpisu o id sesji " << token << std::endl;
			}

			else if ((std::to_string(token)).size() == 5) {
				//wyszukiwanie po id obliczenia
				bool found = false;
				while (std::getline(historia, linia)) {
					first = linia.substr(0, linia.find(" "));
					int pom2 = std::stoi(first, nullptr, 10);
					if (pom2 == token) {
						std::cout << "ID    ||    LICZBA 1     ||    LICZBA 2    ||    OP    ||    STATUS    ||    WYNIK" << std::endl;
						std::cout << linia << std::endl;
						found = true;
						break;
					} 
				} if(found == false) std::cout << "Nie znaleziono w historii wpisu o id obliczenia " << token << std::endl;
			}
			else std::cout << "Podano nieprawidlowy identyfikator. Podaj dwucyfrowy identyfikator sesji lub 5-cyfrowy identyfikator obliczenia." << std::endl;

			//decyzja o dalszym przegladaniu
			std::cout << "Czy chcesz kontynuowac przegladanie historii? t/n ";
			char con;
			std::cin >> con;
			if (con == 'n') discover = false;
			if (con == 't') discover = true;
			
			historia.close();
			}

			else {
				std::cout << "Dostep do pliku zostal zabroniony!" << std::endl;
				return 0;
			}

		} while (discover == true);

	return 0;
}

int main()
{
	std::string ipAddress = "192.168.43.146";		//adres IP serwera
	int port = 27656;							//port nasluchujacy serwera

	//inicjalizacja winsocka
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		std::cerr << "can't start WinSock, Err #" << wsResult << std::endl;
		return 1;
	}

	//tworzenie socketa
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	//wypelnienie struktury hint
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//ustalanie identyfikatora sesji
	std::string last = lastsearch();
	std::string idses_s = last.substr(0, 2);

	int idses = std::stoi(idses_s);
	idses++;

	bool connected = false;

	do {
		//pobranie operacji od uzytkownika
		std::cout <<std::endl<< "Jaka operacje chcesz wykonac?" << std::endl;
		std::cout << "0. Potegowanie" << std::endl << "1. Logarytm" << std::endl << "2. Modulo" << std::endl << "3. Dzielenie" << std::endl << "4. Historia" << std::endl<<"5. EXIT"<<std::endl<< std::endl;
		std::cout << "> ";

		int op;
		std::cin >> op;

		if (op == 0 || op == 1 || op == 2 || op == 3) {
			if (connected == false) {
				//polaczenie z serwerem
				int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
				if (connResult == SOCKET_ERROR) {
					std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
					closesocket(sock);
					WSACleanup();
					return 0;
				}
				else connected = true;
				std::cout << "Polaczono z serwerem!" << std::endl << std::endl;
			}
			operation(sock, hint, op, idses);
		}
		else if (op == 4) history();
		else if (op == 5) {
			exit(sock);
			return 0;
		}
		else std::cout << "Operacja nie istnieje." << std::endl;
	} while (true);

	return 0;
}
