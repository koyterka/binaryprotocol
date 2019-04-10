# binaryprotocol
Komunikacja pomiędzy klientem a serwerem (1:1), w oparciu o autorski protokół binarny. Serwer ma rolę kalkulatora, który wykonuje działania na liczbach podanych przez klienta.

Protokół połączeniowy, wszystkie dane są przesyłane binarnie.

Format komunikatu:
PO    Liczba1   Liczba2    PS   Identyfikator obliczenia    Wynik     DOP

● PO - 2-bitowe pole operacji. Możliwe wartości PO:

    ○ 00 - potęgowanie
    
    ○ 01 - logarytm
    
    ○ 10 - a modulo b
    
    ○ 11 - dzielenie
    

● Liczba1, Liczba2 - pola 64-bitowe będące binarnym przedstawieniem liczb, na
których wykonywane są operacje.

● PS - 2-bitowe pole statusu. Możliwe wartości PS:

    ○ 00 - potęgowanie
    
    ○ 00 - OK
    
    ○ 01 - podano niepoprawne dane (np. dzielenie przez 0, logarytm z 1 i 1)
    
    ○ 10 - wynik wykracza poza zakres (większy od 2^64 − 1)
    
    ○ 11 - operacja nie istnieje
    

● ID - identyfikator obliczenia - 16-bitowe pole będące 5-cyfrową liczbą, której
dwie pierwsze cyfry to identyfikator sesji, a cała tworzy identyfikator jednego
obliczenia. Np. identyfikator 10231 to identyfikator obliczenia 10231
wykonanego w sesji 10. Taki format ułatwia przeglądanie historii i
wyszukiwanie obliczeń po ich sesji.

● Wynik - 64-bitowe pole będące wynikiem działania na Liczbie 1 i Liczbie 2.

● DOP - 4-bitowe pole dopełnienia, składa się z samych zer. Dane w
komunikacie zajmują 212 bitów, więc aby dopełnić rozmiar komunikatu do 27
bajtów potrzebujemy 4 bity dopełnienia.


Schemat sesji komunikacyjnej:
1. Aplikacja klienta pobiera od użytkownika wartość PO, tworzy identyfikatory
sesji i obliczenia, pobiera Liczba1 i Liczba2. Jeśli dane są poprawne, to
tworzy komunikat z wypełnionymi polami PO, Liczba 1, Liczba 2 i ID. Reszta
pól wypełniona jest zerami. Taki komunikat klient wysyła do serwera.
2. Serwer odbiera komunikat i sprawdza, czy może wykonać na Liczba1 i
Liczba2 odpowiednią operację.
a. Jeśli nie, to wpisuje w pole statusu liczbę odpowiadającą napotkanemu
błędowi, pole wynik pozostawia wypełnionym zerami i taki komunikat
wysyła do klienta.
b. Jeżeli operacja na liczbach jest możliwa, to serwer wpisuje w pole
statusu 00, wynik operacji w pole Wynik i gotowy komunikat wysyła do
klienta.
3. Klient odbiera komunikat i sprawdza pole statusu. Jeżeli pole statusu to coś
innego niż 00(OK), to wyświetla informację o błędnych danych. Jeżeli status
to 00, aplikacja wyświetla wynik operacji.

