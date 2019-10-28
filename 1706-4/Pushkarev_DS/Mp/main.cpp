#include "mpi.h"
#include <iostream> 
#include <stdlib.h>
#include <random>

using namespace::std;

int procs; //число процессов
int rankprocs; //номер ранга(принимает разные значения в разных процессах)
int razmer = 0;
int proverka = 0;
//заполнение строки
char* FillStr(int n)
{
	char* str;
	str = new char[n];
	char ar[33];
	for (int i = 0, j = 97; i < 26; i++)
	{
		ar[i] = (char)j;
		j++;
	}
		ar[26] = ' ';
		ar[27] = ' ';
		ar[28] = ' ';
		ar[29] = ' ';
		ar[30] = ' ';
		ar[31] = ' ';
		ar[32] = ' ';
	for (int i = 0; i < n; i++)
		str[i] = ar[(rand() % 33)];
	str[n] = '\0';
	return str;
}

char* FillStr1(int n)
{
	char* str;
	str = new char[n];
	char stroka[1000] = "";
	cin.getline(stroka, n + 1);//выделяем для \0
	for (int i = 0; i < n; i++)
	{
		str[i] = stroka[i];
	}
	str[n] = '\0';

	//cin.getline(str, n);
	/*for (int i = 0; i < n; i++)
		cin >> str[i];*/
	return str;
}

int main(int argc, char *argv[])//количество процессов и ссылка на exe
{
	double t1 = 0.0, t2, dt;
	//double precision;
	//setlocale(LC_ALL, "ru");
	//подразумевается, что слова разделяются пробелами
	char* str;
	str = new char[1000];
	char* localArray;
	//localArray = new char[1000];
	/*char str[128] = "";*/
	int n = 1000;
	double N = 0;
	MPI_Status stat;
	MPI_Init(&argc, &argv);//Инициализация среды выполнения MPI программы, параметры - кол-во аргументов командной строки и сами аргументы
	MPI_Comm_size(MPI_COMM_WORLD, &procs);//количество процессов
	MPI_Comm_rank(MPI_COMM_WORLD, &rankprocs);//определяет ранг процесса(который вызвал эту функцию)

	//MPI_Barrier(MPI_COMM_WORLD);

	/*precision = MPI_Wtick();
	cout << precision << endl;*/
	//procs = 2;///////////////

	if (rankprocs == 0)
	{
		bool flag = false;
		
		while (flag == false)
		{
		cout << "size str " << endl;
		cin >> n;
		if (n > 0)
		flag = true;
		else {
			cout << "size str > 0" << endl;
			//cin.clear();
		}
		}
		cin.getline(str, n);
		t1 = MPI_Wtime();
		str = FillStr(n);////
		if (procs < 1)
		{
			cout << "size str < 0" << endl;
			return 0;
		}
		else
			cout << "Number proc : " << procs << endl;

		razmer = (n + 1) / procs;//5
		proverka = razmer;
		if (((n+1) % procs) != 0)
		{
			razmer++;
		}
			//razmer++;
		razmer += 1;//перекрытие
	}
	//t1 = MPI_Wtime();
	//MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); //рассылка числа


	//MPI_Bcast(&razmer, 1, MPI_INT, 0, MPI_COMM_WORLD); //рассылка числа


	if (rankprocs == 0)
	for (int i = 1; i < procs; i++)
	{
		if (i != (procs - 1)) {
			MPI_Send(&razmer, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		else {
			MPI_Send(&proverka, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		if (rankprocs != (procs - 1))
		{
			MPI_Recv(&razmer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
		}
		else
		{
			MPI_Recv(&razmer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
		}
	}

	/*if (rankprocs == 0)
	{
		MPI_Send(&proverka, 1, MPI_INT, procs, 0, MPI_COMM_WORLD);
	}*/
	/*else if (rankprocs == procs)
	MPI_Recv(&razmer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);*/
	//int razmer = 0;
	//	razmer = n / procs;//5
	//	if ((n % procs) != 0)
	//		razmer++;
	//	razmer += 2;//7
		//int s = razmer + 2;//для шестого //для \0
//для \0
	localArray = new char[razmer+1];
	localArray[razmer] = '\0';
	/*for (int i = 0; i < razmer + 1; i++)
		cout << localArray[i];
	cout << endl;*/
	//localArray[razmer] = 'A';
	/*if (!rankprocs)
	{
		for (int i = 0; i <= razmer; i++)
			cout << localArray[i];
		cout << endl;
	}*/
	///////

	//for (int i = 1; i < procs; i++)
	//MPI_Scatterv(str, razmer - 1, MPI_CHAR, localArray, razmer - 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	//MPI_Barrier(MPI_COMM_WORLD);
			//int kolichestvo = razmer * (rankprocs + 1);//
		if (rankprocs == 0) 
		{
			for (int i = 1; i < procs; i++)
			{
				/*MPI_Recv(&razmer, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &stat);
				MPI_Recv(&kolichestvo, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &stat);*/
				//MPI_Send(&str[razmer * i], razmer + 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
			//MPI_Send(&str[(razmer * i) - 2], razmer - 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
				if (i == (procs - 1))
				{
					MPI_Send(&str[((razmer - 1) * i)], proverka, MPI_CHAR, i, 0, MPI_COMM_WORLD);
				}
				else
				MPI_Send(&str[((razmer - 1) * i)], razmer, MPI_CHAR, i, 0, MPI_COMM_WORLD);
				//MPI_Scatter(&str[((razmer -2) *i)], razmer - 1, MPI_CHAR, localArray, razmer - 1, MPI_CHAR, 0, MPI_COMM_WORLD);
			}
			for (int i = 0; i < razmer; i++)
				localArray[i] = str[i];
			for (int i = 0; i < razmer; i++)
				cout << localArray[i];
			//localArray[razmer - 1] = '\0';
		}

			else
			{
				if (rankprocs == (procs - 1))
					MPI_Recv(localArray, razmer, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
					
			else
				MPI_Recv(localArray, razmer, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);

				/*cout << rankprocs << "MASSIV" << endl;
				for (int i = 0; i <= razmer; i++)
					cout << localArray[i];
				cout << endl;*/



			/*localArray[0] = ' ';
			localArray[1] = 'q';
			localArray[2] = ' ';
			localArray[3] = 'v';
				localArray[4] = '\0';*/
				//localArray[razmer - 1] = '\0';
			}
	///////////

	//MPI_Bcast(str, n, MPI_CHAR, 0, MPI_COMM_WORLD); //рассылка строки

			double summ = 0;
			cout << rankprocs << " proc start work" << endl;
			for (int i = 0; i < razmer-1; i++)
				{
					if (localArray[i] == ' ')
						summ++;
					if (localArray[i] == ' ' && localArray[i + 1] == ' ')
								summ--;
				}
			//cout << rankprocs << " PROCESS : " << summ << endl;

			/*cout << rankprocs << " PROCESS : " << summ << endl;
			for (int i = 0; i < razmer; i++)
			{
				cout << localArray[i];
			}
			cout << endl;*/

	//int size = n / procs;
	//if ((n % procs) != 0)
	//	size++;
	//	/////////size += procs - 1;
	//int i1 = size * rankprocs;
	//int i2 = size * (rankprocs + 1);

	//double summ = 0;
	//cout << rankprocs << " proc start work" << endl;
	//for (int i = 0; i < razmer; i++)
	//{
	//	if (str[i] == ' ')
	//		summ++;
	//	//if (str[i] == ' ' && (str[i + 1] == ' ' || str[i + 1] == 0)) // проверка на лишние пробелы//для случаев, когда посл символ пробел
	//	//if ((str[i] == ' ' && str[i + 1] == ' ') || (str[i] == ' ' && str[i + 1] == 0))
	//	if (str[i] == ' ' && str[i + 1] == ' ')
	//		summ--;
	//}
	double sums = 0;
	MPI_Reduce(&summ, &sums, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		//N++;
	if (rankprocs == 0)
	{
		N = sums;
		//for (int i = 1; i < procs; i++)
		//{
		//	MPI_Recv(&summ, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &stat);//Получить
		//	N += summ;
		//}
		//MPI_Reduce(&summ, &N, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		N++;//если нет пробелов
		if (str[0] == ' ')//проверка на первый нулевой
			N--;
		if (str[n - 1] == ' ')
			N--;
		t2 = MPI_Wtime();
		dt = t2 - t1;
		cout << "Procnum " << rankprocs<< " time= " << dt << endl;
		cout << "Words: " << N << endl;
		for (int i = 0; i < n; i++)
			cout << str[i];
		cout << endl;
	}
	else {
		//MPI_Send(&summ, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);//Отправить
	/*	t2 = MPI_Wtime();
		dt = t2 - t1;
		cout << "Procnum " << rankprocs << " time= " << dt << endl;*/
	}
	//cout << t2 - t1;
	//delete[] str;
	//delete[] localArray;
	MPI_Finalize();//Последняя вызываемая ф-я MPI
	return 0;
}




