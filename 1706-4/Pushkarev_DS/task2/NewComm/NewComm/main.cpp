#include <mpi.h>
#include <iostream>
#include <stdlib.h>
//#include <vector>
#include <stdio.h>
//#include <math.h>
//#include <cstring>
//#include <ctime>
//#include <algorithm>
//#include <stdexcept>
#include <random>
#include <string.h>
#include <string>

void SUM(void* a, void* b, int n);
void SUM_double(void* a, void* b, int n);
void PROD_int(void* a, void* b, int n);
void PROD_double(void* a, void* b, int n);
void MAX_int(void* a, void* b, int n);
void MAX_double(void* a, void* b, int n);
void MIN_int(void* a, void* b, int n);
void MIN_double(void* a, void* b, int n);
void LAND_int(void* a, void* b, int n);
void LAND_double(void* a, void* b, int n);
void LOR_int(void* a, void* b, int n);
void LOR_double(void* a, void* b, int n);
void LXOR_int(void* a, void* b, int n);
void LXOR_double(void* a, void* b, int n);
void BOR_int(void* a, void* b, int n);
void BXOR_int(void* a, void* b, int n);
void BAND_int(void* a, void* b, int n);
void MAXLOC_double(void* a, void* b, int n, MPI_Comm comm);
void MINLOC_double(void* a, void* b, int n, MPI_Comm fcomm);

struct A {

	double value; int proc;

}local_max[3], global_max[3];

struct B {

	double value; int proc;

}local_max1[3], global_max1[3];



void Reduce(void* where_to_send_from, void* where_to_send_to, int how_much, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	MPI_Status status;

	int rank_came, num_of_proc;

	MPI_Comm_rank(comm, &rank_came);//в каком сейчас процессе

	MPI_Comm_size(comm, &num_of_proc);



	int rank = rank_came, num = num_of_proc;////////////////////////////////можно оставить как было

	int ost = 0, type_size, step = 1;//остаток,



	while (num > 1)//количество процессов
	{

		if (rank < num)//если процесс, в котором сейчас < количества процессов(внизу условие)
		{

			ost = num % 2;

			if (rank % 2 != 0)//если процесс нечетный
			{

				MPI_Send(where_to_send_from, how_much, datatype, (rank - 1) * step, 5, comm);

				rank *= num;

				break;

			}
			else
			{

				if (rank != (num - 1))//если процесс четный и не последний
				{

					MPI_Recv(where_to_send_to, how_much, datatype, (rank + 1) * step, 5, comm, &status);
					//op(where_to_send_to, where_to_send_from, how_much);
					if (datatype == MPI_INT)
					{
						if (op == MPI_SUM)
						{
							SUM(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_PROD)
						{
							PROD_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_MAX)
						{
							MAX_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_MIN)
						{
							MIN_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LAND)
						{
							LAND_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LOR)
						{
							LOR_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LXOR)
						{
							LXOR_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_BOR)
						{
							BOR_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_BXOR)
						{
							BXOR_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_BAND)
						{
							BAND_int(where_to_send_to, where_to_send_from, how_much);
						}
					}
					else if (datatype == MPI_DOUBLE)
					{
						if (op == MPI_SUM)
						{
							SUM(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_PROD)
						{
							PROD_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_MAX)
						{
							MAX_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_MIN)
						{
							MIN_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LAND)
						{
							LAND_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LOR)
						{
							LOR_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LXOR)
						{
							LXOR_double(where_to_send_to, where_to_send_from, how_much);
						}
					}
					else if (datatype == MPI_DOUBLE_INT)
					{
						if (op == MPI_MAXLOC)
						{
							MAXLOC_double(where_to_send_to, where_to_send_from, how_much, comm);
						}
						else if (op == MPI_MINLOC)
						{
							MINLOC_double(where_to_send_to, where_to_send_from, how_much, comm);
						}
					}

					//op(where_to_send_to, where_to_send_from, how_much);

				}

				rank /= 2;/////////////////может в 2 раза меньше становится

			}

			step *= 2;

		}

		num = num / 2 + ost;

	}

	if (rank_came == 0)
	{

		MPI_Type_size(datatype, &type_size);

		for (int i = 0; i < how_much; i++)
			std::cout << "POSMOTRI " << ((A*)(double*)where_to_send_from)[i].value << std::endl;
		if (datatype != MPI_DOUBLE_INT)
			memcpy(where_to_send_to, where_to_send_from, how_much * type_size);
		else
			memcpy(where_to_send_to, where_to_send_from, how_much * sizeof(struct A));

	}

}

void Reduce_double(void* where_to_send_from, void* where_to_send_to, int how_much, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	MPI_Status status;

	int rank_came, num_of_proc;

	MPI_Comm_rank(comm, &rank_came);//в каком сейчас процессе

	MPI_Comm_size(comm, &num_of_proc);



	int rank = rank_came, num = num_of_proc;////////////////////////////////можно оставить как было

	int ost = 0, type_size, step = 1;//остаток,



	while (num > 1)//количество процессов
	{

		if (rank < num)//если процесс, в котором сейчас < количества процессов(внизу условие)
		{

			ost = num % 2;

			if (rank % 2 != 0)//если процесс нечетный
			{

				MPI_Send(where_to_send_from, how_much, datatype, (rank - 1) * step, 5, comm);

				rank *= num;

				break;

			}
			else
			{

				if (rank != (num - 1))//если процесс четный и не последний
				{

					MPI_Recv(where_to_send_to, how_much, datatype, (rank + 1) * step, 5, comm, &status);


					if (op == MPI_SUM)
					{
						SUM_double(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_PROD)
					{
						PROD_double(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_MAX)
					{
						MAX_double(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_MIN)
					{
						MIN_double(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_LAND)
					{
						LAND_double(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_LOR)
					{
						LOR_double(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_LXOR)
					{
						LXOR_double(where_to_send_to, where_to_send_from, how_much);
					}
				}

				rank /= 2;

			}

			step *= 2;

		}

		num = num / 2 + ost;

	}

	if (rank_came == 0)
	{
		MPI_Type_size(datatype, &type_size);
		memcpy(where_to_send_to, where_to_send_from, how_much * type_size);

	}

}

void Reduce_int(void* where_to_send_from, void* where_to_send_to, int how_much, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	MPI_Status status;

	int rank_came, num_of_proc;

	MPI_Comm_rank(comm, &rank_came);//в каком сейчас процессе

	MPI_Comm_size(comm, &num_of_proc);



	int rank = rank_came, num = num_of_proc;////////////////////////////////можно оставить как было

	int ost = 0, type_size, step = 1;//остаток,



	while (num > 1)//количество процессов
	{

		if (rank < num)//если процесс, в котором сейчас < количества процессов(внизу условие)
		{

			ost = num % 2;

			if (rank % 2 != 0)//если процесс нечетный
			{

				MPI_Send(where_to_send_from, how_much, datatype, (rank - 1) * step, 5, comm);

				rank *= num;

				break;

			}
			else
			{

				if (rank != (num - 1))//если процесс четный и не последний
				{

					MPI_Recv(where_to_send_to, how_much, datatype, (rank + 1) * step, 5, comm, &status);
					if (op == MPI_SUM)
					{
						SUM(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_PROD)
					{
						PROD_int(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_MAX)
					{
						MAX_int(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_MIN)
					{
						MIN_int(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_LAND)
					{
						LAND_int(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_LOR)
					{
						LOR_int(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_LXOR)
					{
						LXOR_int(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_BOR)
					{
						BOR_int(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_BXOR)
					{
						BXOR_int(where_to_send_to, where_to_send_from, how_much);
					}
					else if (op == MPI_BAND)
					{
						BAND_int(where_to_send_to, where_to_send_from, how_much);
					}

				}

				rank /= 2;

			}

			step *= 2;

		}

		num = num / 2 + ost;

	}

	if (rank_came == 0)
	{
		MPI_Type_size(datatype, &type_size);
		memcpy(where_to_send_to, where_to_send_from, how_much * type_size);

	}

}

void Reduce_double_int(void* where_to_send_from, void* where_to_send_to, int how_much, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	MPI_Status status;

	int rank_came, num_of_proc;

	MPI_Comm_rank(comm, &rank_came);//в каком сейчас процессе

	MPI_Comm_size(comm, &num_of_proc);



	int rank = rank_came, num = num_of_proc;////////////////////////////////можно оставить как было

	int ost = 0, type_size, step = 1;//остаток,



	while (num > 1)//количество процессов
	{

		if (rank < num)//если процесс, в котором сейчас < количества процессов(внизу условие)
		{

			ost = num % 2;

			if (rank % 2 != 0)//если процесс нечетный
			{

				MPI_Send(where_to_send_from, how_much, datatype, (rank - 1) * step, 5, comm);

				rank *= num;

				break;

			}
			else
			{

				if (rank != (num - 1))//если процесс четный и не последний
				{

					MPI_Recv(where_to_send_to, how_much, datatype, (rank + 1) * step, 5, comm, &status);

					if (op == MPI_MAXLOC)
					{
						MAXLOC_double(where_to_send_to, where_to_send_from, how_much, comm);
					}
					else if (op == MPI_MINLOC)
					{
						MINLOC_double(where_to_send_to, where_to_send_from, how_much, comm);
					}

				}

				rank /= 2;

			}

			step *= 2;

		}

		num = num / 2 + ost;

	}

	if (rank_came == 0)
	{
		MPI_Type_size(datatype, &type_size);
		memcpy(where_to_send_to, where_to_send_from, how_much * sizeof(struct A));

	}

}

void SUM(void* a, void* b, int n)
{

	//*b += *a;


	for (int i = 0; i < n; i++)
		((int*)b)[i] += ((int*)a)[i];

}

void SUM_double(void* a, void* b, int n)
{

	//*b += *a;


	for (int i = 0; i < n; i++)
		((double*)b)[i] += ((double*)a)[i];

}


void PROD_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
		((int*)b)[i] *= ((int*)a)[i];

}

void PROD_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
		((double*)b)[i] *= ((double*)a)[i];

}

void MAX_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (((int*)b)[i] < ((int*)a)[i])
			((int*)b)[i] = ((int*)a)[i];
	}

}

void MAX_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (((double*)b)[i] < ((double*)a)[i])
			((double*)b)[i] = ((double*)a)[i];
	}

}


void MIN_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (((int*)b)[i] > ((int*)a)[i])
			((int*)b)[i] = ((int*)a)[i];
	}

}

void MIN_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (((double*)b)[i] > ((double*)a)[i])
			((double*)b)[i] = ((double*)a)[i];
	}

}


void LAND_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] && ((int*)a)[i];
	}

}

void LAND_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((double*)b)[i] = ((double*)b)[i] && ((double*)a)[i];
	}

}


void LOR_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] || ((int*)a)[i];
	}

}

void LOR_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((double*)b)[i] = ((double*)b)[i] || ((double*)a)[i];
	}

}

void LXOR_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] != ((int*)a)[i];
	}

}

void LXOR_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((double*)b)[i] = ((double*)b)[i] != ((double*)a)[i];
	}

}

void BOR_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] | ((int*)a)[i];
	}

}

void BXOR_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] ^ ((int*)a)[i];
	}

}

void BAND_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] & ((int*)a)[i];
	}

}

void MAXLOC_double(void* a, void* b, int n, MPI_Comm fcomm)
{
	int rank;
	MPI_Comm_rank(fcomm, &rank);

	for (int i = 0; i < n; i++)
	{
		std::cout << rank << "BEFORE" << ((A*)(double*)b)[i].value << std::endl;
		std::cout << rank << "SRAVNI" << ((A*)(double*)a)[i].value << std::endl;
		if (((A*)(double*)b)[i].value < ((A*)(double*)a)[i].value)
		{
			((A*)(double*)b)[i].value = ((A*)(double*)a)[i].value;
			((A*)(int*)b)[i].proc = ((A*)(int*)a)[i].proc;
		}
		else if (((A*)(double*)b)[i].value == ((A*)(double*)a)[i].value)
			if (((A*)(int*)b)[i].proc > ((A*)(int*)a)[i].proc)
				((A*)(int*)b)[i].proc = ((A*)(int*)a)[i].proc;
		std::cout << "AFTER" << ((A*)(double*)b)[i].value << std::endl;
	}
	std::cout << std::endl << std::endl;
}

void MINLOC_double(void* a, void* b, int n, MPI_Comm fcomm)
{
	int rank;
	MPI_Comm_rank(fcomm, &rank);
	for (int i = 0; i < n; i++)
	{
		std::cout << rank << "PROCESSSSSSSS " << ((A*)(double*)b)[i].proc << std::endl;
		std::cout << rank << "VALUE_B " << ((A*)(double*)b)[i].value << std::endl;
		std::cout << rank << "VALUE_A " << ((A*)(double*)a)[i].value << std::endl;
		if (((A*)(double*)b)[i].value > ((A*)(double*)a)[i].value)
		{
			((A*)(double*)b)[i].value = ((A*)(double*)a)[i].value;
			((A*)(int*)b)[i].proc = ((A*)(int*)a)[i].proc;
		}
		else if (((A*)(double*)b)[i].value == ((A*)(double*)a)[i].value)
			if (((A*)(int*)b)[i].proc > ((A*)(int*)a)[i].proc)
				((A*)(int*)b)[i].proc = ((A*)(int*)a)[i].proc;
		std::cout << rank << "VALUE_B_AFTER " << ((A*)(double*)b)[i].value << std::endl;
		std::cout << i << " INDEX " << rank << "PROCESSSSSSSS" << ((A*)(double*)b)[i].proc << std::endl;

	}
	std::cout << std::endl << std::endl;
}


int main(int argc, char** argv) {
	int n = atoi(argv[1]);//размер массива
	int root = atoi(argv[2]);//процесс, который будет root
	char* stroka = argv[3];
	//std::string str = argv[3];
	//std::cout << str << std::endl;
	char* oper = argv[4];
	//std::cout << stroka << std::endl;
	//std::cout << root << std::endl << std::endl << std::endl;
	MPI_Init(&argc, &argv);
	int np, rank;//количество процессов и ранг
	//int chislo = 1, chislo1 = 1;
	//int sum, sum1;
	//int n = 3;//размер массива
	double tic1, ticTree1, ticMPI1, tic2, ticTree2, ticMPI2;/////////////////////////
	int size = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//chislo *= rank;//переменной присваивается ранг, чтобы в каждом процессе были разные числа
	//std::cout << "Chislo: " << chislo << " Rank: " << rank << std::endl;
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	//int root = 3;

	int* mas = new int[n];
	int* mas_res = new int[n];
	int* mas1 = new int[n];
	int* mas_res1 = new int[n];
	double* mas_double = new double[n];
	double* mas_double_res = new double[n];
	double* mas_double1 = new double[n];
	double* mas_double_res1 = new double[n];

	if (n == 3 & np == 4)
	{
		//mas[i] = i;
	//mas_double[i] = i + 0.1;
	//mas_double[i] = i + rank;
		if (rank == 0)
		{
			mas_double[0] = 1.1;
			mas_double1[0] = 1.1;
			mas_double[1] = 2.1;
			mas_double1[1] = 2.1;
			mas_double[2] = 2;
			mas_double1[2] = 2;
			for (int i = 0; i < n; i++)
			{
				local_max[i].value = mas_double[i];
				local_max[i].proc = rank;
				local_max1[i].value = mas_double[i];
				local_max1[i].proc = rank;
			}
		}
		else if (rank == 1)
		{
			mas_double[0] = -1;
			mas_double[1] = 1.1;
			mas_double[2] = -3;
			mas_double1[0] = -1;
			mas_double1[1] = 1.1;
			mas_double1[2] = -3;

			for (int i = 0; i < n; i++)
			{
				local_max[i].value = mas_double[i];
				local_max[i].proc = rank;
				local_max1[i].value = mas_double[i];
				local_max1[i].proc = rank;
			}
		}
		else if (rank == 2)
		{
			mas_double[0] = 1;
			mas_double[1] = -6;
			mas_double[2] = 3;
			mas_double1[0] = 1;
			mas_double1[1] = -6;
			mas_double1[2] = 3;

			for (int i = 0; i < n; i++)
			{
				local_max[i].value = mas_double[i];
				local_max[i].proc = rank;
				local_max1[i].value = mas_double[i];
				local_max1[i].proc = rank;
			}
		}
		else if (rank == 3)
		{
			mas_double[0] = -1;
			mas_double[1] = 7;
			mas_double[2] = -32.1;
			mas_double1[0] = -1;
			mas_double1[1] = 7;
			mas_double1[2] = -32.1;

			for (int i = 0; i < n; i++)
			{
				local_max[i].value = mas_double[i];
				local_max[i].proc = rank;
				local_max1[i].value = mas_double[i];
				local_max1[i].proc = rank;
			}
		}

		if (rank == 0)
		{
			mas[0] = 0;
			mas[1] = 1;
			mas[2] = 0;
			mas1[0] = 0;
			mas1[1] = 1;
			mas1[2] = 0;
		}
		else if (rank == 1)
		{
			mas[0] = 0;
			mas[1] = 1;
			mas[2] = 0;
			mas1[0] = 0;
			mas1[1] = 1;
			mas1[2] = 0;
		}
		else if (rank == 2)
		{
			mas[0] = 0;
			mas[1] = 1;
			mas[2] = 1;
			mas1[0] = 0;
			mas1[1] = 1;
			mas1[2] = 1;
		}
		else if (rank == 3)
		{
			mas[0] = 0;
			mas[1] = 1;
			mas[2] = 1;
			mas1[0] = 0;
			mas1[1] = 1;
			mas1[2] = 1;
		}
	}
	else
		for (int i = 0; i < n; i++)
		{
			mas[i] = rank;
			mas1[i] = rank;
			mas_double[i] = rank + 0.1 + i;//можно + i чтобы разные
			mas_double1[i] = rank + 0.1 + i;
		}
	/////////////////////////////////////////////////////
	MPI_Group MPI_GROUP_WORLD;
	MPI_Group group;//новая группа
	MPI_Comm fcomm;//новый коммуникатор

	int processov = np, q, proc;
	int* process_ranks;//массив процессов
	int rank_in_group;
	MPI_Status status;
	//MPI_Comm_size(MPI_COMM_WORLD, &size);//размер
	//MPI_Comm_rank(MPI_COMM_WORLD, &rank);//ранк

	//printf("New group contains processes:");
	//q = size - 1;
	process_ranks = new int[np];
	//process_ranks = (int*)malloc(q * sizeof(int));
	for (proc = 0; proc < np; proc++)
	{
		if (proc == 0)
			process_ranks[proc] = root;
		else if (proc == root)
			process_ranks[proc] = 0;
		else
			process_ranks[proc] = proc;
		//printf("%i ", process_ranks[proc]);
	}
	if (rank == 0)
		for (int i = 0; i < np; i++)
		{
			std::cout << "process_ranks  " << process_ranks[i] << std::endl;
		}

	MPI_Comm_group(MPI_COMM_WORLD, &MPI_GROUP_WORLD);
	MPI_Group_incl(MPI_GROUP_WORLD, np, process_ranks, &group);
	MPI_Comm_create(MPI_COMM_WORLD, group, &fcomm);

	//if (fcomm != MPI_COMM_NULL)
	//{
	int newprocs, newrank;
	MPI_Comm_size(fcomm, &newprocs);
	MPI_Comm_rank(fcomm, &newrank);

	/*if (newrank == 0)
		std::cout <<"DATAAAAAAAAAAAAAAAAAAAAAAAAA "<< chislo << std::endl;*/

	if (newrank == 0)
	{
		tic1 = MPI_Wtime();
	}

	if (strcmp(stroka, "MPI_DOUBLE") == 0)
	{
		if (strcmp(oper, "MPI_SUM") == 0)
			Reduce_double(mas_double, mas_double_res, n, MPI_DOUBLE, MPI_SUM, 0, fcomm);
		else if (strcmp(oper, "MPI_PROD") == 0)
			Reduce_double(mas_double, mas_double_res, n, MPI_DOUBLE, MPI_PROD, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_MAX") == 0)
			Reduce_double(mas_double, mas_double_res, n, MPI_DOUBLE, MPI_MAX, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_MIN") == 0)
			Reduce_double(mas_double, mas_double_res, n, MPI_DOUBLE, MPI_MIN, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LAND") == 0)
			Reduce_double(mas_double, mas_double_res, n, MPI_DOUBLE, MPI_LAND, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LOR") == 0)
			Reduce_double(mas_double, mas_double_res, n, MPI_DOUBLE, MPI_LOR, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LXOR") == 0)
			Reduce_double(mas_double, mas_double_res, n, MPI_DOUBLE, MPI_LXOR, 0, fcomm);//работает
	}
	else if (strcmp(stroka, "MPI_INT") == 0)
	{
		if (strcmp(oper, "MPI_SUM") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_SUM, 0, fcomm);
		else if (strcmp(oper, "MPI_PROD") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_PROD, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_MAX") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_MAX, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_MIN") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_MIN, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LAND") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_LAND, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LOR") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_LOR, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LXOR") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_LXOR, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_BOR") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_BOR, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_BXOR") == 0)
			Reduce_int(mas, mas_res, n, MPI_INT, MPI_BXOR, 0, fcomm);//работает
	}
	else if (strcmp(stroka, "MPI_DOUBLE_INT") == 0)
	{
		if (strcmp(oper, "MPI_MAXLOC") == 0)
			Reduce_double_int(local_max, global_max, n, MPI_DOUBLE_INT, MPI_MAXLOC, 0, fcomm);
		else if (strcmp(oper, "MPI_MINLOC") == 0)
			Reduce_double_int(local_max, global_max, n, MPI_DOUBLE_INT, MPI_MINLOC, 0, fcomm);
	}

	tic2 = MPI_Wtime();

	if (newrank == 0)
	{
		ticMPI1 = MPI_Wtime();
	}
	/////////////////////////////////MPI_REDUCE
	if (strcmp(stroka, "MPI_DOUBLE") == 0)
	{
		if (strcmp(oper, "MPI_SUM") == 0)
			MPI_Reduce(mas_double1, mas_double_res1, n, MPI_DOUBLE, MPI_SUM, 0, fcomm);
		else if (strcmp(oper, "MPI_PROD") == 0)
			MPI_Reduce(mas_double1, mas_double_res1, n, MPI_DOUBLE, MPI_PROD, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_MAX") == 0)
			MPI_Reduce(mas_double1, mas_double_res1, n, MPI_DOUBLE, MPI_MAX, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_MIN") == 0)
			MPI_Reduce(mas_double1, mas_double_res1, n, MPI_DOUBLE, MPI_MIN, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LAND") == 0)
			MPI_Reduce(mas_double1, mas_double_res1, n, MPI_DOUBLE, MPI_LAND, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LOR") == 0)
			MPI_Reduce(mas_double1, mas_double_res1, n, MPI_DOUBLE, MPI_LOR, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LXOR") == 0)
			MPI_Reduce(mas_double1, mas_double_res1, n, MPI_DOUBLE, MPI_LXOR, 0, fcomm);//работает
	}
	else if (strcmp(stroka, "MPI_INT") == 0)
	{
		if (strcmp(oper, "MPI_SUM") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_SUM, 0, fcomm);
		else if (strcmp(oper, "MPI_PROD") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_PROD, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_MAX") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_MAX, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_MIN") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_MIN, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LAND") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_LAND, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LOR") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_LOR, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_LXOR") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_LXOR, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_BOR") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_BOR, 0, fcomm);//работает
		else if (strcmp(oper, "MPI_BXOR") == 0)
			MPI_Reduce(mas1, mas_res1, n, MPI_INT, MPI_BXOR, 0, fcomm);//работает
	}
	else if (strcmp(stroka, "MPI_DOUBLE_INT") == 0)
	{
		if (strcmp(oper, "MPI_MAXLOC") == 0)
			Reduce_double_int(local_max1, global_max1, n, MPI_DOUBLE_INT, MPI_MAXLOC, 0, fcomm);
		else if (strcmp(oper, "MPI_MINLOC") == 0)
			Reduce_double_int(local_max1, global_max1, n, MPI_DOUBLE_INT, MPI_MINLOC, 0, fcomm);
	}

	if (newrank == 0) {
		ticMPI2 = MPI_Wtime();
		std::cout << "reduce realisation MPI: t =  " << ticMPI2 - ticMPI1 << std::endl;
		std::cout << "reduce realisation my: t =  " << tic2 - tic1 << std::endl;
		if ((ticMPI2 - ticMPI1) > (tic2 - tic1))
			std::cout << "YES";
		std::cout << std::endl;

		if (strcmp(stroka, "MPI_DOUBLE") == 0)
		{
			for (int i = 0; i < n; i++)
			{
				std::cout << "RES =  " << mas_double_res[i];
			}
		}
		else if (strcmp(stroka, "MPI_INT") == 0)
		{
			for (int i = 0; i < n; i++)
			{
				std::cout << "RES =  " << mas_res[i];
			}
		}
		else if (strcmp(stroka, "MPI_DOUBLE_INT") == 0)
		{
			for (int i = 0; i < n; i++)
			{
				std::cout << "RES =  " << global_max[i].value << " NUMBER = " << global_max[i].proc << " ";
			}
		}
		std::cout << std::endl;
		/////////////////////////////////////////MPI_REDUCE
		if (strcmp(stroka, "MPI_DOUBLE") == 0)
		{
			for (int i = 0; i < n; i++)
			{
				std::cout << "RES =  " << mas_double_res1[i];
			}
		}
		else if (strcmp(stroka, "MPI_INT") == 0)
		{
			for (int i = 0; i < n; i++)
			{
				std::cout << "RES =  " << mas_res1[i];
			}
		}
		else if (strcmp(stroka, "MPI_DOUBLE_INT") == 0)
		{
			for (int i = 0; i < n; i++)
			{
				std::cout << "RES =  " << global_max1[i].value << " NUMBER = " << global_max1[i].proc << " ";
			}
		}
	}

	MPI_Finalize();
	return 0;


}