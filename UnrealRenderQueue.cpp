// UnrealRenderQueue.cpp : define o ponto de entrada para o aplicativo do console.
//
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "windows.h"
#include <cstdlib>
#include "wtypes.h"
#include <atlstr.h> 
#include <chrono>
#include <ctime>
//#include "concol.h"
//#include "rang.hpp"
#include "termcolor.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////
// Executes the given command using CreateProcess() and WaitForSingleObject().
// Returns FALSE if the command could not be executed or if the exit code could not be determined.
BOOL executeCommandLine(CString cmdLine, DWORD & exitCode)
{
	PROCESS_INFORMATION processInformation = { 0 };
	STARTUPINFO startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);
	int nStrBuffer = cmdLine.GetLength() + 50;


	// Create the process
	BOOL result = CreateProcess(NULL, cmdLine.GetBuffer(nStrBuffer),
		NULL, NULL, FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
		NULL, NULL, &startupInfo, &processInformation);
	cmdLine.ReleaseBuffer();


	if (!result)
	{
		// CreateProcess() failed
		// Get the error from the system
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		// Display the error
		CString strError = (LPTSTR)lpMsgBuf;
		//TRACE(_T("::executeCommandLine() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmdLine, strError);

		// Free resources created by the system
		LocalFree(lpMsgBuf);

		// We failed.
		return FALSE;
	}
	else
	{
		// Successfully created the process.  Wait for it to finish.
		WaitForSingleObject(processInformation.hProcess, INFINITE);

		// Get the exit code.
		result = GetExitCodeProcess(processInformation.hProcess, &exitCode);

		// Close the handles.
		CloseHandle(processInformation.hProcess);
		CloseHandle(processInformation.hThread);

		if (!result)
		{
			// Could not get exit code.
			//TRACE(_T("Executed command but couldn't get exit code.\nCommand=%s\n"), cmdLine);
			return FALSE;
		}


		// We succeeded.
		return TRUE;
	}
}

const string dataAtual()
{
	auto Agora = std::chrono::system_clock::now();
	std::time_t Data = std::chrono::system_clock::to_time_t(Agora);
	return std::ctime(&Data);
}

int main()
{
	cout << "\n\nUnreal Engine 4 - Fila de Render\nArtho Juan Pacini, Fuel 3D, 2018\n\n";
   

	vector<string> nomesCenas; //Guarda o nome das cenas
	vector<string> comandos; //Guarda as linhas de comando
	

	//INPUT DE RENDERS
	string input;
	do
	{
		cout << termcolor::magenta << "\n[FILA RENDER]: Digite o nome da cena: ";
		string nomeCena;
		getline(cin, nomeCena);

		string comando;
		cout << termcolor::magenta << "\n[FILA RENDER]: Digite a linha de comando para renderizar a cena: ";
		getline(cin, comando);

		nomesCenas.push_back(nomeCena);
		comandos.push_back(comando);

		cout << termcolor::green << "\n[FILA RENDER]: " << nomeCena << " adicionado a fila de render com sucesso!\nQuer adicionar mais algum render na fila? (y/n): ";
		

		getline(cin, input);
	} while (input == "y");

	cout << termcolor::cyan << "\n" << dataAtual() << "[FILA RENDER]: Comecando a renderizar...\n";

	auto startAllRenders = std::chrono::system_clock::now();

	//INICIO DO RENDER 
	for(int i = 0; i < nomesCenas.size(); i++)
	{
		//benchmark tempo inicio
		auto start = std::chrono::system_clock::now();

		cout << termcolor::magenta << "\n\n" << dataAtual() << "[INFO]: Comecando o render da cena << " << termcolor::on_green << termcolor::grey << nomesCenas[i] << termcolor::reset << termcolor::magenta << " >>...\n";
		
		DWORD a = 0;
		if (executeCommandLine(comandos[i].c_str(), a))
		{
			cout << termcolor::green << "\n"<< dataAtual() <<"[INFO]: "	<< termcolor::on_green << termcolor::grey << "Sucesso :)\n" << termcolor::reset;
		}
		else
		{
			cout << "\n" << dataAtual() << "\n[INFO]: " << termcolor::on_red << termcolor::green << "Ocorreu algum erro :(\n" << termcolor::reset;;
		}

		//benchmark tempo final
		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);


		cout << termcolor::green << "\n" << dataAtual() << "[INFO] Render da cena " << nomesCenas[i] << " demorou " << elapsed.count() << " segundos..\n\n\n";

		if (i == nomesCenas.size())
		{
			cout << termcolor::cyan << "\n" << dataAtual() << "[FILA RENDER]: Partindo para o proximo render em ";
			for (int i = 10; i > 0; i--)
			{
				cout << termcolor::cyan << i << " segundos...\n";
				Sleep(10000);
				cout << '\t';
			}
		}
		
		
	}


	auto endTotalRender = std::chrono::system_clock::now();
	auto elapsedTotalRender = std::chrono::duration_cast<std::chrono::seconds>(endTotalRender - startAllRenders);

	cout << termcolor::green << "\n\n\n" << dataAtual() << "[INFO] Tempo total de todos os renders juntos foi de " << elapsedTotalRender.count() << " segundos..\n";
	

	cout << termcolor::green << "\n" << dataAtual() << "[FILA RENDER]: Todos os renders finalizados...";

	cout << "\n" << dataAtual() << "[FILA RENDER]: tecle Enter para finalizar...";
	string wait;
	getline(cin, wait);

	return 0;
}

