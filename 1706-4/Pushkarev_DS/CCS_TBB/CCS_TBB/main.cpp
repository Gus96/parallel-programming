#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include "tbb\tbb.h"

using namespace std;

typedef double type;

struct Task
{
	int pointerStart;
	int pointerEnd;
	int taskIndex;
	Task() :pointerStart(0), pointerEnd(0), taskIndex(0) {}
	Task(int a, int b, int c) :pointerStart(a), pointerEnd(b), taskIndex(c) {}
};

class Matrix
{
protected:
	int row = 0;
	int col = 0;
public:
	std::vector<type> vv;
	Matrix()
	{
		vv = vector<double>();
	}
	Matrix(int col, int row)
	{
		this->row = row;
		this->col = col;
		vv = vector<type>(row * col);//�� ��������
	}
	Matrix(int col, int row, type val)
	{
		this->row = row;
		this->col = col;
		vv = vector<type>(row * col, val);
	}

	int gRow() const
	{
		return row;
	}
	int gCol() const
	{
		return col;
	}
	type* operator [] (int j)
	{
		return &vv[j * col];
	}
	void transposition()
	{
		Matrix A(row, col);
		for (int i = 0; i < col; i++)
			for (int j = 0; j < row; j++)
			{
				A[i][j] = (*this)[j][i];
			}
		std::swap(row, col);
		vv = A.vv;
	}

	bool operator == (const Matrix& m)
	{
		bool res = (row == m.row & col == m.col);
		if (res == false)
			return false;

		for (int i = 0; i < vv.size(); i++)
			if (vv[i] != m.vv[i]) return false;
		return true;
	}

	static void readMatrix(Matrix& A, Matrix& B, int N, string name)//��� ������ �������(������ ������� �� �����)
	{
		ifstream input(name);
		input >> A.vv[0];//��������� ������, �� ��� �������� �����
		for (int i = 0; i < A.vv.size() + B.vv.size(); i++)
		{
			if (i < A.vv.size())
				input >> A.vv[i];
			else
			{
				for (int j = 0; j < B.vv.size(); j++)
					input >> B.vv[j];
			}
		}
		input.close();
		A.transposition();
		B.transposition();
	}

	static void writeMatrix(Matrix& A, int N)//���������� ������� � ����
	{
		A.transposition();
		ofstream input("resMatrix.txt");
		for (int i = 0; i < A.vv.size(); i++)
		{
			if (i < A.vv.size())
				input << A.vv[i] << "\t ";
			if ((i + 1) % N == 0)
				input << endl;
		}
		input.close();
	}

	static void randMatrixdata(Matrix& A, Matrix& B, int N)//���������� ������� ��� ������ � ����
	{
		for (int i = 0; i < N * N; i++)
		{
			int prob;//�����������
			prob = rand() % 100;
			if (prob < 15)
				A.vv[i] = (rand() % 10) + (double)(rand() % 10) / 10;//���������� ����� � ������� �����
			else
				A.vv[i] = 0;
		}

		for (int i = 0; i < N * N; i++)
		{
			int prob;//�����������
			prob = rand() % 100;
			if (prob < 15)
				B.vv[i] = (rand() % 10) + (double)(rand() % 10) / 10;
			else
				B.vv[i] = 0;
		}
	}
};

class MatrixCCS
{
protected:
	vector<type> values;
	vector<int> rows;
	vector<int> pointer;
	int N;
public:
	friend class FunctorTBB;
	MatrixCCS() {}
	MatrixCCS(int n) :N(n)
	{
		pointer.push_back(0);
	}
	MatrixCCS(Matrix& M) :N(M.gRow())
	{
		pointer = vector<int>(N + 1);
		int lastCount = 0;
		for (int j = 0; j < N; j++)
		{
			int curCount = 0;
			for (int i = 0; i < N; i++)
			{
				type el = M[j][i];//����� ������� �� ����������� �������
				if (el != 0.0)
				{
					curCount++;
					rows.push_back(i);//��������� � ����� ������
					values.push_back(el);
				}
			}
			if (j != 0)
			{
				pointer[j] = pointer[j - 1] + lastCount;//������ � values, ������ � �������
			}
			lastCount = curCount;
		}
		pointer[N] = (int)values.size();
	}
	void transposition()
	{
		vector<vector<pair<type, int>>> tmp(N, vector<pair<type, int>>());
		int pCount = 0;//����� �������
		int numElementsInCol = 0;//������� ��������� ��������� � �������
		for (int i = 0; i < values.size(); i += numElementsInCol)
		{
			numElementsInCol = pointer[pCount + 1] - pointer[pCount];//���������� ���������� ��������� � �������
			for (int z = i; z < i + numElementsInCol; z++)//���� ��� ��������� ������ �������
			{
				int row = rows[z];//� ����� ������ ���������
				int col = pCount;
				type el = values[z];
				tmp[row].push_back(make_pair(el, col));
			}
			pCount++;
		}
		pCount = 0;//��� pointer
		vector<int>* cols = &rows;
		int lastCount = 0;
		int vCount = 0;
		for (int i = 0; i < N; i++)
		{
			int numElementInCol = (int)tmp[i].size();//������ ������ � �������
			if (numElementInCol > 0)//���� � ������ ������ ���� ��������
			{
				for (int j = 0; j < numElementInCol; j++)//���������� �������� ��� CCS
				{
					values[vCount] = tmp[i][j].first;//����� �������
					(*cols)[vCount] = tmp[i][j].second;
					vCount++;
				}
			}
			if (pCount != 0)
			{
				pointer[pCount] = pointer[pCount - 1] + lastCount;
			}
			lastCount = numElementInCol;
			pCount++;

		}
	}
	void uniteMatrix(const MatrixCCS& m)//����������� ������ � ���� �������
	{
		for (int i = 0; i < m.values.size(); i++)//���������� values � rows
		{
			values.push_back(m.values[i]);
			rows.push_back(m.rows[i]);
		}
		for (int i = 1; i < m.pointer.size(); i++)//���������� pointer
		{
			int start = pointer[pointer.size() - 1];//����� pointer ��� ���������
			pointer.push_back(m.pointer[i] - m.pointer[i - 1] + start);
		}
	}

	MatrixCCS parallelMult(MatrixCCS& m, const int numThreads)
	{
		class FunctorTBB
		{
		private:
			MatrixCCS* m1;
			MatrixCCS* m2;
			vector<Task>* tasks;
			vector<MatrixCCS>* tmp;
			vector<int>* elCountM;
		public:
			FunctorTBB()
			{
				m1 = nullptr;
				m2 = nullptr;
				tasks = nullptr;
				tmp = nullptr;
				elCountM = nullptr;
			}
			FunctorTBB(MatrixCCS* m1_, MatrixCCS* m2_, vector<Task>* tasks_, vector<MatrixCCS>* tmp_, vector<int>* elCountM_)
			{//��������� ������
				m1 = m1_;
				m2 = m2_;
				tasks = tasks_;
				tmp = tmp_;
				elCountM = elCountM_;
			}
			FunctorTBB(const FunctorTBB& f)
			{
				m1 = f.m1;
				m2 = f.m2;
				tasks = f.tasks;
				tmp = f.tmp;
				elCountM = f.elCountM;
			}
			void operator()(const tbb::blocked_range<size_t>& range) const
			{
				int N = m1->N;
				vector<int>* cols = &(m1->rows);
				for (size_t itask = range.begin(); itask < range.end(); itask++)
					for (int j = (*tasks)[itask].pointerStart; j < (*tasks)[itask].pointerEnd; j++)
					{
						int numElInResCol = 0;//���������� ��������� � �������������� �������
						const int numElementInCol = m2->pointer[j + 1] - m2->pointer[j];//���������� ��������� � �������
						if (numElementInCol == 0)//���� ������� ������ � ������
						{
							int size = (*tmp)[itask].pointer.size();
							(*tmp)[itask].pointer.push_back((*tmp)[itask].pointer[size - 1]);
							continue;
						}
						int elCountThis = 0;
						for (int i = 0; i < N; i++)
						{
							const int numElementInRow = m1->pointer[i + 1] - m1->pointer[i];
							if (numElementInRow == 0)
							{
								continue;
							}
							int tmpNumElRow = numElementInRow;//��� A
							int tmpNumElCol = numElementInCol;//��� B

							type sum = 0;
							int tmpElCountM = (*elCountM)[itask];
							for (int z = 0; z < min(tmpNumElCol, tmpNumElRow);)//�������� �� ������� � ��������
							{
								int colThis = (*cols)[elCountThis];//cols==rows(�� A)
								int rowM = m2->rows[tmpElCountM];//������ rows � �������� ����� �������� task(�������� �������� rows � B)
								if (colThis == rowM)
								{
									sum += m1->values[elCountThis] * m2->values[tmpElCountM];
									tmpNumElCol--;
									tmpNumElRow--;
									tmpElCountM++;
									elCountThis++;
								}
								else if (colThis < rowM)
								{
									tmpNumElRow--;//��������� ���������� ���������
									elCountThis++;//����������� ������
								}
								else
								{
									tmpNumElCol--;
									tmpElCountM++;
								}
							}
							for (int z = 0; z < tmpNumElRow; z++)//������� ������ � rows, ���� � ����� ������� � ������� ������ �����
								elCountThis++;

							if (sum != 0)//���������� � �������������� �������(�� ��������)
							{
								(*tmp)[itask].values.push_back(sum);
								(*tmp)[itask].rows.push_back(i);
								numElInResCol++;
							}
						}
						const int size = (*tmp)[itask].pointer.size();
						(*tmp)[itask].pointer.push_back((*tmp)[itask].pointer[size - 1] + numElInResCol);//���������� ���������� + ���������� ���������, ������� � �������
						(*elCountM)[itask] += numElementInCol;
					}
			}
			~FunctorTBB()
			{

			}
		};
		int numTask = numThreads;
		if (numTask > N)
			numTask = N;
		vector<MatrixCCS> tmpMatrix(numTask, MatrixCCS(N));
		vector<int> elCountM(numTask);
		vector<int>* cols = &rows;
		vector<Task> task(numTask);//�������� ������, � ������� numTask �����
		//�������� ����� �������
		{
			int sizeTask = m.N / numTask + (bool)(m.N % numTask);//���� �������, �� + 0, ����� + 1 (������������ ������� �������)
			for (int i = 0; i < numTask; i++)
				task[i] = Task(i * sizeTask, min((i + 1) * sizeTask, m.N), i % numTask);//(������, �����, ������)
			int lastPointerM = 0;
			for (int i = 0; i < numTask; i++)
			{
				elCountM[i] = lastPointerM;
				int jstart = task[i].pointerStart;
				const int jend = task[i].pointerEnd;
				for (jstart; jstart < jend; jstart++)
				{
					lastPointerM += m.pointer[jstart + 1] - m.pointer[jstart];//������� ���������� ��������� ���������
				}
			}
		}

		FunctorTBB fctor(this, &m, &task, &tmpMatrix, &elCountM);
		tbb::parallel_for(tbb::blocked_range<size_t>((size_t)0, task.size(), (size_t)4), fctor);

		for (int i = 1; i < tmpMatrix.size(); i++)
		{
			tmpMatrix[0].uniteMatrix(tmpMatrix[i]);//�������� ��������� ����� �������, ���������� ��� ����� � �������
		}
		if (tmpMatrix[0].pointer.size() < N + 1)
			tmpMatrix[0].pointer.push_back(tmpMatrix[0].values.size());
		return tmpMatrix[0];
	}

	Matrix CCStoMatrix()
	{
		Matrix res(N, N);

		int Elem = 0;
		int count = 0;
		int resCount = 0;

		for (int i = 0; i < N; i++)
		{
			int numOfElem = pointer[i + 1] - pointer[i];

			if (numOfElem != 0)
				for (int j = 0; j < N; j++)
				{
					if (numOfElem != 0 && rows[resCount] == j)
					{
						res.vv[count] = values[Elem];
						Elem++;
						resCount++;
						numOfElem--;
					}
					else
						res.vv[count] = 0;
					count++;
				}
			else
				for (int j = 0; j < N; j++)
				{
					res.vv[count] = 0;
					count++;
				}
		}

		return res;
	}
};


int main()
{
	int numThreads;
	int mSize;
	cout << "NumThreads = ";
	cin >> numThreads;

	tbb::task_scheduler_init init(numThreads);

	cout << "1.Rand" << endl;
	cout << "2.From file" << endl;
	int choice;
	cin >> choice;

	if (choice == 1)
	{
		cout << "Size of Matrix = ";
		cin >> mSize;
		Matrix a(mSize, mSize), b(mSize, mSize), c(mSize, mSize);//������� �������
		Matrix::randMatrixdata(a, b, mSize);//������������� �������
		MatrixCCS a1(a), b1(b), c1(c);//������������� ������� � CCS �������



		double time = omp_get_wtime();//������� �������
		c1 = a1.parallelMult(b1, numThreads);
		time = omp_get_wtime() - time;
		cout << "Time: " << time << endl;//���������� �������� ������� � �����
		c = c1.CCStoMatrix();

		Matrix::writeMatrix(c, mSize);//������ ���������� � ����

	}
	else if (choice == 2)
	{
		ifstream input("matrix.txt");//������ �� ����� ����������� �������
		input >> mSize;
		input.close();

		Matrix a(mSize, mSize), b(mSize, mSize), c(mSize, mSize);//������� �������
		Matrix::readMatrix(a, b, mSize, "matrix.txt");
		MatrixCCS a1(a), b1(b), c1(c);
		a1.transposition();

		double time = omp_get_wtime();//������� �������
		c1 = a1.parallelMult(b1, numThreads);
		time = omp_get_wtime() - time;
		cout << "Time: " << time << endl;//���������� �������� ������� � �����
		c = c1.CCStoMatrix();

		Matrix::writeMatrix(c, mSize);//������ ���������� � ����
	}
	else
		cout << "Error" << endl;

	return 0;
}