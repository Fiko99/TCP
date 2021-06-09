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
#include <fstream>

using namespace std;

int main()
{
	WSADATA wsaData; // inicjalizacja żądanej wersji biblioteki WinSock
	// zabezpieczenie w przypadku niepowodzenia inicjalizacji 
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		_getch();
		exit(EXIT_FAILURE);
	}

	int sockfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);  // utworzenie gniazda TCP (SOCK_STREAM)

	if (sockfd == -1) {
		printf("Nieudane utworzenie gniazda...\n");
	}
	else
		printf("Gniazdo utworzone pomyslnie..\n");

	memset(&servaddr, 0, sizeof(servaddr)); // Ustawia pierwszą liczbę znaków miejsca docelowego na znak 0
	// przydzielenie IP i portu  
	servaddr.sin_family = AF_INET;  // IPv4  
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); // Konwersja adresu IP z postaci tekstowej na postać bitową
	servaddr.sin_port = htons(8080);
	// podłączenie gniazda klienta do gniazda serwera i weryfikacja
	if (connect(sockfd, (SOCKADDR*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Nieudane polaczenie z serwerm...\n");
	}
	else
		printf("Polaczono z serwerem..\n");


	char * buf; // Odpowiedź od klienta
	buf = new char[30];
	char * buf_out;
	int file_size;
	FILE *fp;
	errno_t err;

	err = fopen_s(&fp, "LibreOffice_6.4.2_Win_x64_helppack_pl.7z", "rb"); // odczyt pliku binarnie
	// Warunek sprawdzający poprawność odczytu pliku
	if (err != 0) {
		printf("Error podczas otwierania pliku .. \n");
		_getch();
	}
	fseek(fp, 0, SEEK_END);  // ustawia wskaźnik na koniec
	if (err == 0) // dopóki nie koniec pliku...
	{
		file_size = ftell(fp); // Pobranie bieżącej pozycji wskaźnika pliku i przypisanie jej do zmiennej określającej rozmiar pliku

		if (send(sockfd, to_string(file_size).c_str(), 32, 0) == SOCKET_ERROR)
		{
			printf("Blad wysylania wiadomosci\n");
			closesocket(sockfd);
			WSACleanup();
		}

		buf_out = new char[file_size];
		fseek(fp, 0, SEEK_SET); // ustawia wskaźnik na początek
		printf("Odczytano: %lu\n", fread(buf_out, 1, file_size, fp));
		if (send(sockfd, buf_out, file_size, 0) == SOCKET_ERROR)
		{
			printf("Blad wysylania pliku\n");
			closesocket(sockfd);
			WSACleanup();
		}

		printf("Rozmiar pliku: %d B\n", file_size);
		delete[] buf_out;
	}
	fclose(fp);
	closesocket(sockfd);
	WSACleanup();
	_getch();
	return 0;
}