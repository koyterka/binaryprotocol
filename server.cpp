#include "pch.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include "bin_protocol.h"
#include "serverTCP_bin.h"
#include <cmath>
#include <limits.h>

#pragma comment (lib, "Ws2_32.lib")

//funckcja zajmująca się obsługą odebranego pakietu
void takecareofpack(bin_protocol& b, char* pck) {
	//przepisanie bufora do tablicy charów wewnątrz klasy
	for (int i = 0; i < 27; i++) {
		b.pakiet[i] = pck[i]; 
	}
	//rozpakowanie pakietu, czyli przypisanie odpowiednych danych do odpowiednich zmiennych
	b.unpack();

	// rzutowanie na inty
	int operacja = (int)b.OPERACJA.to_ulong();
	long long liczba1 = (long long)b.LICZBA1.to_ullong();
	long long liczba2 = (long long)b.LICZBA2.to_ullong();
	//inicjalizacja zmiennych pomocnicznych
	long long wynik;
	int status;
	int dop = 0;

	//potęgowanie
	if (operacja == 0) {
		long long tmp = pow(liczba1, liczba2);
		if (liczba2 == 0) {
			wynik = 1;
			status = 0;
		}
		else if (liczba1 == 0) {
			wynik = 0;
			status = 0;
		}
		else if (tmp >= INT64_MAX-1 || tmp <= 0) {
			wynik = 0;
			status = 2;
		}
		else {
			wynik = tmp;
			status = 0;
		}
		
	}
	//logarytmowanie
	else if (operacja == 1) {
		if (liczba1<=0 || liczba1 == 1 || liczba2 <= 0) {
			wynik = 0;
			status = 1;
		}
		else {
			long long tmp2 = (long long)(log(liczba2) / log(liczba1));
			status = 0;
			wynik = tmp2;
		}
		
	}
	//a modulo b
	else if (operacja == 2) {
		if (liczba2 == 0) {
			wynik = 0;
			status = 1;
		}
		else {
			long long tmp3 = liczba1 % liczba2;
			wynik = tmp3;
			status = 0;
		}
		
	}
	//dzielenie
	else if (operacja == 3) {
		if (liczba2 == 0) {
			wynik = 0;
			status = 1;
		}
		else {
			long long tmp4 = liczba1 / liczba2;
			wynik = tmp4;
			status = 0;
		}
	}
	//nieznana operacja
	else if (operacja > 3 || operacja < 0) {
		wynik = 0;
		status = 3;
	}

	b.WYNIK = wynik;
	b.STATUS = status;
	int id = (int)b.ID.to_ulong();

	//wpisanie danych do historii 
	std::ofstream historiaprzegl;
	historiaprzegl.open("historia.txt", std::ofstream::app);
	if (historiaprzegl) {
		historiaprzegl << id << " " << liczba1 << " " << liczba2 << " "
			<< operacja << " " << status << " " << wynik << std::endl;
	}
	historiaprzegl.close();

	//metoda pakująca dane do wewnętrznej tablicy charów, która zostanie wysłana do klienta
	b.pack();
}

//funkcja służąca do odczytywanie danych z zapisanej historii operacji
void przegladaniehistorii() {

	int taklubnie = 1; //pomocnicza zmienna do obsługi przeglądania historii

	while (taklubnie != 0) {
		std::cout << "Czy chcesz przegladac historie operacji? \n" << "Wpis 1 na tak lub 0 na nie: ";
		std::cin >> taklubnie;
	
		switch (taklubnie)
		{
		case 0:
			break;
		case 1:
			std::ifstream historia;
			historia.open("historia.txt");
			std::cout << "Przegladanie calej historii operacji (wpis 1) \n" << "Przegladanie historii operacji poprzez podanie identyfikatorze sesji (wpisz 2) \n"
				<< "Przegladanie historii operacji poprzez podanie identyfikatora obliczen (wpis 3)\n" << "Twoj wybor: ";
			int przegl;
			std::cin >> przegl;

			int token;
			std::string linia;
			std::string first;
			std::string first2;

			switch (przegl)
			{
			case 1:
				//jeśli plik jest otwarty to wypisuje całą jego zawartość
				if (historia.is_open()) {
					std::string tmp;
					while (std::getline(historia, tmp)) {
						std::cout << tmp << std::endl;
					}

				}
				std::cout << std::endl;
				continue;
			case 2:
				std::cout << "Podaj identyfikator sesji: ";
				std::cin >> token;
				//identyfikator sesji jest dwucyfrowy (pierwsze dwie cyfry w całym identyfikatorze)
				if ((std::to_string(token)).size() == 2) {
					while (std::getline(historia, linia)) {
						first = linia.substr(0, linia.find(" "));
						first2 = first.substr(0, 2);
						if (std::stoi(first2) == token) {
							std::cout << linia << std::endl;
						}
					}
				}
				std::cout << std::endl;
				continue;
			case 3:
				std::cout << "Podaj identyfikator obliczenia: ";
				std::cin >> token;
				//identyfikator obliczenia to cały identyfikator
				while (std::getline(historia, linia)) {
					first = linia.substr(0, linia.find(" "));
					int bump = std::stoi(first, nullptr, 10);
					if (bump == token) {
						std::cout << linia << std::endl;
						std::cout << std::endl;
						break;
					}
				}
				std::cout << std::endl;
				continue;
			}
		}
	}
	
}

int main()
{	
	//inicjalizacja winsocka
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	
	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0) {
		std::cerr << "Nie mozna zainicjalizowac polaczenia \n";
		return 1;
	}

	//tworzenie socketu do nasluchiwania
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		std::cerr << "Nie mozna utworzyc socketu do nasluchiwania \n";
		return 1;
	}

	//łączenie adresu ip i portu do socketu
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(27656);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)& hint, sizeof(hint));

	//nasluchiwanie
	listen(listening, SOMAXCONN);

	//socket klienta
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);
	//wyświetlanie informacji o kliencie
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " polaczony na porcie " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " polaczony na porcie " << ntohs(client.sin_port) << std::endl;
	}

	char buf[27]; //pomocniczy bufor do przesyłania pakietów
		
	while(true){
		ZeroMemory(buf, 27); 
		bin_protocol get; //inicjalizacja klasy pomocniczej, na której polach są wykonywane wszelkie operacje
		
		//odebranie pakietu
		int reply = recv(clientSocket, buf, 27, 0);
		if (reply == SOCKET_ERROR) { //błąd socketu
			std::cerr << "Blad w recv\n";
			break;
		}
		if (reply == 0) { //rozłączenie się ze strony klienta
			std::cout << "Klient sie rozlaczyl\n";
			break;
		}
		//wywołanie funkcji pomocniczej, która odpakowuje odebrany pakiet, wykonuje na nim operacje, po czym tworzy wewnątrz klasy gotową tablicę charów do wysłania odpowiedzi
		takecareofpack(get, buf);
		//wyświetlenie przetworzonego pakietu
		get.display();
		
		//wysłanie pakietu do klienta
		send(clientSocket, get.pakiet, sizeof(get.pakiet), 0);

		//przegladanie historii po kazdym wysylaniu
		przegladaniehistorii();
	}

	//zamykanie socketu i cleanup
	closesocket(clientSocket);
	WSACleanup();

	//przeglądanie historii po rozłączeniu klienta
	przegladaniehistorii();

	return 0;
}
