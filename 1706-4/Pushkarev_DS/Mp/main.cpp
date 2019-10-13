#include "mpi.h"
#include <iostream> 
#include "stdlib.h"
#include <random>

using namespace::std;

int procs; //число процессов
int rankprocs; //номер ранга(принимает разные значения в разных процессах)

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
	/*char str[128] = "";*/
	int n = 1000;
	double N = 0;
	//MPI_Status stat;
	MPI_Init(&argc, &argv);//Инициализация среды выполнения MPI программы, параметры - кол-во аргументов командной строки и сами аргументы
	MPI_Comm_size(MPI_COMM_WORLD, &procs);//количество процессов
	MPI_Comm_rank(MPI_COMM_WORLD, &rankprocs);//определяет ранг процесса(который вызвал эту функцию)

	//MPI_Barrier(MPI_COMM_WORLD);

	/*precision = MPI_Wtick();
	cout << precision << endl;*/
	//procs = 4;///////////////

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
			cin.clear();
		}
		}
		cin.getline(str, n);
		str = FillStr(n);////
		if (procs < 1)
		{
			cout << "size str < 0" << endl;
			return 0;
		}
		else
			cout << "Number proc : " << procs << endl;
	}
	if (rankprocs == 0)
	t1 = MPI_Wtime();
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); //рассылка числа

	MPI_Bcast(str, n, MPI_CHAR, 0, MPI_COMM_WORLD); //рассылка строки

	int size = n / procs;
	if ((n % procs) != 0)
		size++;
		//size += procs - 1;
	int i1 = size * rankprocs;
	int i2 = size * (rankprocs + 1);

	double summ = 0;
	cout << rankprocs << " proc start work" << endl;
	for (int i = i1; i < i2; i++)
	{
		if (str[i] == ' ')
			summ++;
		//if (str[i] == ' ' && (str[i + 1] == ' ' || str[i + 1] == 0)) // проверка на лишние пробелы//для случаев, когда посл символ пробел
		//if ((str[i] == ' ' && str[i + 1] == ' ') || (str[i] == ' ' && str[i + 1] == 0))
		if (str[i] == ' ' && str[i + 1] == ' ')
			summ--;
	}
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
		N++;//???????????????????????????если нет пробелов
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
	MPI_Finalize();//Последняя вызываемая ф-я MPI
	return 0;
}




