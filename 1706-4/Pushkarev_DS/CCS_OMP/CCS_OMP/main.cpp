#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <omp.h>

typedef double type;

using namespace std;

class Matrix
{
protected:
	int row = 0;
	int col = 0;
public:
	vector<type> vv;
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

	int getRow() const
	{
		return row;
	}
	int getCol() const
	{
		return col;
	}
	type* operator [] (int j)
	{
		return &vv[j * col];
	}
	Matrix operator * (Matrix& m)
	{
		if (getCol() != m.getRow()) return Matrix();
		Matrix newMatr(getRow(), m.getCol());
		for (int j = 0; j < m.getRow(); j++)
			for (int i = 0; i < getCol(); i++)
				for (int z = 0; z < getCol(); z++)
					newMatr[j][i] += (*this)[z][i] * m[j][z];
		return newMatr;
	}
	void transposition()
	{
		Matrix A(row, col);
		for (int i = 0; i < col; i++)
			for (int j = 0; j < row; j++)
			{
				A[i][j] = (*this)[j][i];
			}
		swap(row, col);
		vv = A.vv;
	}

	bool operator == (const Matrix& m)
	{
		bool res = ((row == m.row) & (col == m.col));
		if (res == false) return false;
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

	static void randMatrix(int N)//������ ��� ������ � ����(�� ������������)
	{
		ofstream input("randMatrix.txt");
		input << N << endl;
		for (int i = 0; i < N * N; i++)
		{
			if (i < N * N)
			{
				int prob;//�����������
				prob = rand() % 100;
				if (prob < 40)
					input << (rand() % 10) + (double)(rand() % 10) / 10 << "\t";
				else
					input << 0 << "\t";
			}
			if ((i + 1) % N == 0)//���� ����� ������
				input << endl;
		}

		input << endl;//����������� ����� ���������

		for (int i = 0; i < N * N; i++)
		{
			if (i < N * N)
			{
				int prob;//�����������
				prob = rand() % 100;
				if (prob < 40)
					input << (rand() % 10) + (double)(rand() % 10)/10 << "\t";
				else
					input << 0 << "\t";
			}
			if ((i + 1) % N == 0)//���� ����� ������
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
			if (prob < 25)
				A.vv[i] = (rand() % 10) + (double)(rand() % 10) / 10;//���������� ����� � ������� �����
			else
				A.vv[i] = 0;
		}

		for (int i = 0; i < N * N; i++)
		{
			int prob;//�����������
			prob = rand() % 100;
			if (prob < 25)
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
	MatrixCCS(int n) :N(n)
	{
		pointer.push_back(0);
	}
	MatrixCCS(Matrix& M) :N(M.getRow())
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
		int numCol = pointer.size();
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


	MatrixCCS operator * (const MatrixCCS& m)
	{
		MatrixCCS res(N);//�������������� �������
		res.pointer.push_back(0);
		transposition();
		vector<int>* cols = &rows;
		int elCountM = 0;
		for (int j = 0; j < m.N; j++)
		{
			int numElInResCol = 0;
			const int numElementInCol = m.pointer[j + 1] - m.pointer[j];//����������� ���������� ��������� � �������
			if (numElementInCol == 0)
			{
				int size = res.pointer.size();//������ pointer � �������������� �������
				res.pointer.push_back(res.pointer[size - 1]);//��������� ����������(���� � ������ ������� ����)
				continue;
			}
			int elCountThis = 0;
			for (int i = 0; i < N; i++)
			{
				const int numElementInRow = pointer[i + 1] - pointer[i];//������� ��������� � ������
				if (numElementInRow == 0)//���� ���� ���������
				{
					continue;
				}
				int tmpNumElCol = numElementInCol;//���������� ��������� � ������� ������� m
				int tmpNumElRow = numElementInRow;//���������� ��������� � ������ ������� this

				type sum = 0;
				int tmpElCountM = elCountM;
				for (int z = 0; z < min(tmpNumElCol, tmpNumElRow);)//�������� min, �.�. ��������������� �������� ����������, � ������ ����� ����
				{
					int colThis = (*cols)[elCountThis];//�������� ������� ����������
					int rowM = m.rows[tmpElCountM];//�������� ������� ���������� � M
					if (colThis == rowM)//������������ �� �����������
					{
						sum += values[elCountThis] * m.values[tmpElCountM];
						tmpNumElCol--;
						tmpNumElRow--;
						tmpElCountM++;
						elCountThis++;
					}
					else if (colThis < rowM)//this
					{
						tmpNumElRow--;//���������� ���������� ���������
						elCountThis++;//�������
					}
					else//��� m
					{
						tmpNumElCol--;
						tmpElCountM++;
					}
				}
				for (int z = 0; z < tmpNumElRow; z++)
					elCountThis++;

				if (sum != 0)
				{
					res.values.push_back(sum);
					res.rows.push_back(i);
					numElInResCol++;//������� ��������� � �������
				}
			}
			const int size = res.pointer.size();
			res.pointer.push_back(res.pointer[size - 1] + numElInResCol);
			elCountM += numElementInCol;
		}
		transposition();
		return res;
	}

	MatrixCCS parallelMult(const MatrixCCS& m, const int numThreads)//������������ ���������
	{
		int numTask = numThreads;
		if (numTask > N)//���� ������� ������, ��� ����������� �������
			numTask = N;
		this->transposition();
		struct Task//������ ��� ������� ������
		{
			int pointerStart;
			int pointerEnd;
			int taskIndex;
			Task() :pointerStart(0), pointerEnd(0), taskIndex(0) {}
			Task(int a, int b, int c) :pointerStart(a), pointerEnd(b), taskIndex(c) {}
		};
		vector<MatrixCCS> tmpMatrix(numTask, MatrixCCS(N));
		vector<int> elCountM(numTask);
		vector<int>* cols = &rows;
		vector<Task> task(numTask);//�������� ������, � ������� numTask �����
		//�������� ����� �������
		{
			int sizeTask = m.N / numTask + (bool)(m.N % numTask);//���� �������, �� + 0, ����� + 1 (������������ ������� �������)
			for (int i = 0; i < numTask; i++)
				task[i] = Task(i * sizeTask, std::min((i + 1) * sizeTask, m.N), i % numTask);//(������, �����, ������)
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


#pragma omp parallel for
		for (int itask = 0; itask < task.size(); itask++)//������ itask �����
			for (int j = task[itask].pointerStart; j < task[itask].pointerEnd; j++)//�������� �� ��������� ��������� ��������
			{
				int indexTask = task[itask].taskIndex;//����� ������(����� ������)
				int numElInResCol = 0;//���������� ��������� � �������������� �������
				const int numElementInCol = m.pointer[j + 1] - m.pointer[j];//���������� ��������� � �������
				if (numElementInCol == 0)//���� ������� ������ � ������
				{
					int size = tmpMatrix[indexTask].pointer.size();
					tmpMatrix[indexTask].pointer.push_back(tmpMatrix[indexTask].pointer[size - 1]);
					continue;
				}
				int elCountThis = 0;
				for (int i = 0; i < N; i++)
				{
					const int numElementInRows = pointer[i + 1] - pointer[i];
					if (numElementInRows == 0)
					{
						continue;
					}
					int tmpNumElRow = numElementInRows;//��� A
					int tmpNumElCol = numElementInCol;//��� B

					double sum = 0;
					int tmpElCountM = elCountM[indexTask];
					for (int z = 0; z < std::min(tmpNumElCol, tmpNumElRow);)//�������� �� ������� � ��������
					{
						int colThis = (*cols)[elCountThis];//cols==rows(�� A)
						int rowM = m.rows[tmpElCountM];//������ rows � �������� ����� �������� task(�������� �������� rows � B)
						if (colThis == rowM)
						{
							sum += values[elCountThis] * m.values[tmpElCountM];
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
						tmpMatrix[indexTask].values.push_back(sum);
						tmpMatrix[indexTask].rows.push_back(i);
						numElInResCol++;
					}
				}

				const int size = tmpMatrix[indexTask].pointer.size();

				tmpMatrix[indexTask].pointer.push_back(tmpMatrix[indexTask].pointer[size - 1] + numElInResCol);//���������� ���������� + ���������� ���������, ������� � �������
				elCountM[indexTask] += numElementInCol;
			}

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

	omp_set_num_threads(numThreads);

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
		cout << time << endl;//���������� �������� ������� � �����
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

		double time = omp_get_wtime();//������� �������
		c1 = a1.parallelMult(b1, numThreads);
		time = omp_get_wtime() - time;
		cout << time << endl;//���������� �������� ������� � �����
		c = c1.CCStoMatrix();

		Matrix::writeMatrix(c, mSize);//������ ���������� � ����
	}
	else
		cout << "Error" << endl;

	return 0;
}