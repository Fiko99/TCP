#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include <stdlib.h> 
#include <stdio.h> 
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#include <iphlpapi.h>
#include <conio.h>
#include <windows.h>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

void recvFile(int sockfd, char * filename)
{
	char buff[5];  // odebranie wielkosci bufora
	FILE *fp;
	errno_t err;
	unsigned MAX = 0;  // wielkosc pakietu
	unsigned long long int length = 0;  // rozmiar pliku
	strcat_s(filename, 32, ".7z");  // dołącza kopię nazwy pliku, który będzie adresem IP do ".7z"
	err = fopen_s(&fp, filename, "wb"); // zapisuje zawartość pliku binarnie w katalogu programu
	
	
	if (int recv_ = recv(sockfd, buff, 5, 0) < 0)
	{
		printf("Blad odebrania wiadomosci\n");
		if (recv_ == SOCKET_ERROR) {
			wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
			_getch();
		}
		_getch();
		exit(EXIT_FAILURE);
	}

	MAX = stoi(buff);  // konwertuje ciąg znaków na liczbę 
	if (MAX > 0)
	{
		char file_size[32]; // odebranie rozamiru pliku
		if (int recv_ = recv(sockfd, file_size, 32, 0) < 0)
		{
			printf("Blad odebrania wiadomosci\n");
			if (recv_ == SOCKET_ERROR) {
				wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
				_getch();
			}
			_getch();
			exit(EXIT_FAILURE);
		}
		length = stoi(file_size);
		
		// obliczanie liczby pakietów
		unsigned long long int pack_number = length / MAX + ((bool)(length % MAX));  // liczba pakietów. Jeśli rozmiar jest podzielny przez liczbę pakietów to nic się nie dzieję,  jak zostaje reszta, to dodawane jest 1
		unsigned long long int last_pack_size = length - ((length / MAX) * MAX); // rozmiar ostatniego pakietu
		// Warunek sprawdzający poprawność odczytu pliku
		if (err != NULL) {
			printf("Error podczas otwierania pliku .. ");
			return;
		}
		else 
		{
			unsigned long long int recv_packs = 0;  // liczba pakietów odebranych
			// bufor, na który wiadomości będą trafiać
			char * memory;   
			memory = new char[MAX + 1];
			fill_n(memory, MAX + 1, 0); // Przypisuje nową wartość do określonej liczby elementów w zakresie rozpoczynającym się od określonego elementu
			// pętla do wysyłania pliku
			do 
			{
				// funkcja, która odbiera wiadomości
				if (int recv_ = recv(sockfd, memory, MAX, 0) < 0)
				{
					printf("Blad odebrania wiadomosci\n");
					if (recv_ == SOCKET_ERROR) {
						wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
						_getch();
					}
					break;
				}
				recv_packs += 1;
				if (recv_packs != pack_number) // jeśli liczba odebranych pakietów nie równa się liczbie pakietów. . .
				{
					fwrite(memory, 1, MAX, fp); // Zapisuje dane w strumieniu
				}
				else
				{
					fwrite(memory, 1, last_pack_size, fp);
				}
				if(pack_number != 0)
					printf("Odebrano %lld z %llu kB\n", recv_packs, pack_number);
				
				
			} while (recv_packs < pack_number);
			fclose(fp);
			delete memory;
			printf("Rozmiar pliku: %s na %llu B\n", file_size, length);
			if (recv_packs == pack_number)
				printf("Plik pomyslnie odebrany!!! \n");
			else
				printf("Odebrano za malo o %llu kB", pack_number - recv_packs);
		}
	}
}

int main()
{
	char file_name[32]; // nazwa pliku
	WSADATA wsaData; // inicjalizacja żądanej wersji biblioteki WinSock
	// zabezpieczenie w przypadku niepowodzenia inicjalizacji 
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		_getch();
		exit(EXIT_FAILURE);
	}

	int sockfd, connfd, len;     // utworzenie deskryptorów gniazda
	struct sockaddr_in servaddr, cli;   // obiekty struktury sockaddr_in dla klienta i serwera
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);    // utworzenie gniazda TCP (SOCK_STREAM)

	if (sockfd == -1) {
		printf("Nieudane utworzenie gniazda...\n");
	}
	else 
		printf("Gniazdo utworzone pomyslnie..\n");
	 
	memset(&servaddr, 0, sizeof(servaddr)); // Ustawia pierwszą liczbę znaków miejsca docelowego na znak 0
	// przydzielenie IP i portu 
	servaddr.sin_family = AF_INET;   // IPv4  
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // wiąże do dowolnego adresu IP
	servaddr.sin_port = htons(8080);    

	// Wiązanie nowo utworzonego gniazda z danym adresem IP i weryfikacja
	if ((bind(sockfd, (SOCKADDR*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Niedane wiązanie ganizda...\n");
	}
	else
		printf("Gniazdo zostalo pomyslnie powiazane..\n");
	//Sprawdzenie gotowości serwera do nasłuchiwania
	if ((listen(sockfd, 5)) != 0) {
		printf("Nasluchwianie nieudane...\n");		
	}
	else
		printf("Serwer nasluchuje..\n");


	len = sizeof(cli);
	// Zaakceptacja pakietu danych od klienta i weryfikacja
	connfd = accept(sockfd, (SOCKADDR*)&cli, &len);  // akceptuje połączenie z gniazda
	if (connfd < 0) {
		printf("Akceptacja serwera nie powiodła się...\n");
	}
	else
		printf("Serwer akceptuje klienta...\n");
	// wywołanie funkcji odpowiadającej za komukację serwera z klientem
	strcpy_s(file_name, inet_ntoa(cli.sin_addr)); // kopiuje nazwe pliku do adresu IPv4 przekonwertowanego na ciąg znaków
	strcat_s(file_name, "\0");
	recvFile(connfd, inet_ntoa(cli.sin_addr)); // wywyołanie funkcji recvFile, która przyjmuje gniazdo i nazwe plkiu w postaci IP
	
	closesocket(sockfd);
	closesocket(connfd);
	WSACleanup();
	_getch();
	return 0;
}
