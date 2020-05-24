#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <list>
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
		vv = vector<type>(row * col);
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
				type el = M[j][i];//берем элемент из поступившей матрицы
				if (el != 0.0)
				{
					curCount++;
					rows.push_back(i);//сохраняем в какой строке
					values.push_back(el);
				}
			}
			if (j != 0)
			{
				pointer[j] = pointer[j - 1] + lastCount;//индекс в values, начала в матрице
			}
			lastCount = curCount;
		}
		pointer[N] = (int)values.size();
	}
	void transposition()
	{
		vector<vector<pair<type, int>>> tmp(N, vector<pair<type, int>>());
		int pCount = 0;//номер столбца
		int numElementsInCol = 0;//сколько ненулевых элементов в столбце
		for (int i = 0; i < values.size(); i += numElementsInCol)
		{
			numElementsInCol = pointer[pCount + 1] - pointer[pCount];//вычисление количества элементов в столбце
			for (int z = i; z < i + numElementsInCol; z++)
			{
				int row = rows[z];//в какой строке ненулевой
				int col = pCount;
				type el = values[z];
				tmp[row].push_back(make_pair(el, col));
			}
			pCount++;
		}
		pCount = 0;//для pointer
		vector<int>* cols = &rows;
		int lastCount = 0;
		int vCount = 0;
		for (int i = 0; i < N; i++)
		{
			int numElementInCol = (int)tmp[i].size();//размер строки в матрице
			if (numElementInCol > 0)//если в данной строке есть элементы
			{
				for (int j = 0; j < numElementInCol; j++)//заполнение массивов для CCS
				{
					values[vCount] = tmp[i][j].first;//берем элемент
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
	void uniteMatrix(const MatrixCCS& m)
	{
		int numCol = pointer.size();
		for (int i = 0; i < m.values.size(); i++)
		{
			values.push_back(m.values[i]);
			rows.push_back(m.rows[i]);
		}
		for (int i = 1; i < m.pointer.size(); i++)
		{
			int start = pointer[pointer.size() - 1];
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
			FunctorTBB(MatrixCCS* matrix1_, MatrixCCS* matrix2_, vector<Task>* tasks_, vector<MatrixCCS>* tmp_, vector<int>* elCountM_)
			{
				m1 = matrix1_;
				m2 = matrix2_;
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
				//
			}
			~FunctorTBB()
			{

			}
		};

		
	}
};


int main()
{
	int numThreads = 1;


	int N  = 5;
	Matrix a(N, N), b(N, N), c(N, N);

	MatrixCCS a1(a), b1(b);
	a1.transposition();


	tbb::task_scheduler_init init(numThreads);

	double time = omp_get_wtime();
	a1.parallelMult(b1, numThreads);
	time = omp_get_wtime() - time;
	return 0;
}
