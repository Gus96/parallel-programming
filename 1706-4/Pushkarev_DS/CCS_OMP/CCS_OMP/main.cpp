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
		vv = vector<type>(row * col);//по столбцам
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

	static void readMatrix(Matrix& A, Matrix& B, int N, string name)//для вызова классом(читает матрицу из файла)
	{
		ifstream input(name);
		input >> A.vv[0];//считывает размер, но его затирают далее
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


	static void writeMatrix(Matrix& A, int N)//записывает матрицу в файл
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

	static void randMatrix(int N)//рандом для записи в файл(не используется)
	{
		ofstream input("randMatrix.txt");
		input << N << endl;
		for (int i = 0; i < N * N; i++)
		{
			if (i < N * N)
			{
				int prob;//вероятность
				prob = rand() % 100;
				if (prob < 40)
					input << (rand() % 10) + (double)(rand() % 10) / 10 << "\t";
				else
					input << 0 << "\t";
			}
			if ((i + 1) % N == 0)//если конец строки
				input << endl;
		}

		input << endl;//разделитель между матрицами

		for (int i = 0; i < N * N; i++)
		{
			if (i < N * N)
			{
				int prob;//вероятность
				prob = rand() % 100;
				if (prob < 40)
					input << (rand() % 10) + (double)(rand() % 10)/10 << "\t";
				else
					input << 0 << "\t";
			}
			if ((i + 1) % N == 0)//если конец строки
				input << endl;
		}

		input.close();
	}


	static void randMatrixdata(Matrix& A, Matrix& B, int N)//генерирует матрицы без записи в файл
	{
		for (int i = 0; i < N * N; i++)
		{
			int prob;//вероятность
			prob = rand() % 100;
			if (prob < 25)
				A.vv[i] = (rand() % 10) + (double)(rand() % 10) / 10;//генерирует число и дробную часть
			else
				A.vv[i] = 0;
		}

		for (int i = 0; i < N * N; i++)
		{
			int prob;//вероятность
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
			for (int z = i; z < i + numElementsInCol; z++)//цикл для просмотра одного столбца
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


	void uniteMatrix(const MatrixCCS& m)//объединение частей в одну матрицу
	{
		int numCol = pointer.size();
		for (int i = 0; i < m.values.size(); i++)//добавление values и rows
		{
			values.push_back(m.values[i]);
			rows.push_back(m.rows[i]);
		}
		for (int i = 1; i < m.pointer.size(); i++)//вычисление pointer
		{
			int start = pointer[pointer.size() - 1];//какой pointer был последним
			pointer.push_back(m.pointer[i] - m.pointer[i - 1] + start);
		}
	}


	MatrixCCS operator * (const MatrixCCS& m)
	{
		MatrixCCS res(N);//результирующая матрица
		res.pointer.push_back(0);
		transposition();
		vector<int>* cols = &rows;
		int elCountM = 0;
		for (int j = 0; j < m.N; j++)
		{
			int numElInResCol = 0;
			const int numElementInCol = m.pointer[j + 1] - m.pointer[j];//вычисляется количество элементов в столбце
			if (numElementInCol == 0)
			{
				int size = res.pointer.size();//размер pointer в результирующем векторе
				res.pointer.push_back(res.pointer[size - 1]);//дублируем предыдущий(если в данном столбце ноль)
				continue;
			}
			int elCountThis = 0;
			for (int i = 0; i < N; i++)
			{
				const int numElementInRow = pointer[i + 1] - pointer[i];//сколько элементов в строке
				if (numElementInRow == 0)//если ноль элементов
				{
					continue;
				}
				int tmpNumElCol = numElementInCol;//количество элементов в столбце матрицы m
				int tmpNumElRow = numElementInRow;//количество элементов в строке матрицы this

				type sum = 0;
				int tmpElCountM = elCountM;
				for (int z = 0; z < min(tmpNumElCol, tmpNumElRow);)//выбираем min, т.к. соответствующие элементы умножаются, в других будет ноль
				{
					int colThis = (*cols)[elCountThis];//получает позицию ненулевого
					int rowM = m.rows[tmpElCountM];//получает позицию ненулевого в M
					if (colThis == rowM)//сравнивается на соответсвие
					{
						sum += values[elCountThis] * m.values[tmpElCountM];
						tmpNumElCol--;
						tmpNumElRow--;
						tmpElCountM++;
						elCountThis++;
					}
					else if (colThis < rowM)//this
					{
						tmpNumElRow--;//количество оставшихся ненулевых
						elCountThis++;//позиция
					}
					else//для m
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
					numElInResCol++;//сколько элементов в столбце
				}
			}
			const int size = res.pointer.size();
			res.pointer.push_back(res.pointer[size - 1] + numElInResCol);
			elCountM += numElementInCol;
		}
		transposition();
		return res;
	}

	MatrixCCS parallelMult(const MatrixCCS& m, const int numThreads)//параллельное умножение
	{
		int numTask = numThreads;
		if (numTask > N)//если потоков больше, чем размерность матрицы
			numTask = N;
		this->transposition();
		struct Task//задача для каждого потока
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
		vector<Task> task(numTask);//получаем вектор, в котором numTask задач
		//создание задач потокам
		{
			int sizeTask = m.N / numTask + (bool)(m.N % numTask);//если кратное, то + 0, иначе + 1 (распределяем столбцы потокам)
			for (int i = 0; i < numTask; i++)
				task[i] = Task(i * sizeTask, std::min((i + 1) * sizeTask, m.N), i % numTask);//(начало, конец, индекс)
			int lastPointerM = 0;
			for (int i = 0; i < numTask; i++)
			{
				elCountM[i] = lastPointerM;
				int jstart = task[i].pointerStart;
				const int jend = task[i].pointerEnd;
				for (jstart; jstart < jend; jstart++)
				{
					lastPointerM += m.pointer[jstart + 1] - m.pointer[jstart];//считаем количество ненулевых элементов
				}
			}
		}


#pragma omp parallel for
		for (int itask = 0; itask < task.size(); itask++)//каждый itask поток
			for (int j = task[itask].pointerStart; j < task[itask].pointerEnd; j++)//проходим по ненулевым элементам столбцов
			{
				int indexTask = task[itask].taskIndex;//номер потока(номер задачи)
				int numElInResCol = 0;//количество элементов в результирующем столбце
				const int numElementInCol = m.pointer[j + 1] - m.pointer[j];//количество элементов в столбце
				if (numElementInCol == 0)//если столбец только с нулями
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
					int tmpNumElRow = numElementInRows;//для A
					int tmpNumElCol = numElementInCol;//для B

					double sum = 0;
					int tmpElCountM = elCountM[indexTask];
					for (int z = 0; z < std::min(tmpNumElCol, tmpNumElRow);)//проходим по столбцу и умножаем
					{
						int colThis = (*cols)[elCountThis];//cols==rows(из A)
						int rowM = m.rows[tmpElCountM];//индекс rows с которого нужно начинать task(получаем значение rows в B)
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
							tmpNumElRow--;//уменьшаем количество элементов
							elCountThis++;//передвигаем индекс
						}
						else
						{
							tmpNumElCol--;
							tmpElCountM++;
						}
					}
					for (int z = 0; z < tmpNumElRow; z++)//двигаем индекс в rows, если у одной матрицы в столбце меньше чисел
						elCountThis++;

					if (sum != 0)//записываем в результирующую матрицу(по столбцам)
					{
						tmpMatrix[indexTask].values.push_back(sum);
						tmpMatrix[indexTask].rows.push_back(i);
						numElInResCol++;
					}
				}

				const int size = tmpMatrix[indexTask].pointer.size();

				tmpMatrix[indexTask].pointer.push_back(tmpMatrix[indexTask].pointer[size - 1] + numElInResCol);//записываем предыдущее + количество элементов, которые в столбце
				elCountM[indexTask] += numElementInCol;
			}

		for (int i = 1; i < tmpMatrix.size(); i++)
		{
			tmpMatrix[0].uniteMatrix(tmpMatrix[i]);//посылаем следующую часть матрицы, объединяем все части с нулевой
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
		Matrix a(mSize, mSize), b(mSize, mSize), c(mSize, mSize);//создаем матрицы
		Matrix::randMatrixdata(a, b, mSize);//сгенерировали матрицы
		MatrixCCS a1(a), b1(b), c1(c);//преобразовали обычную в CCS матрицы

		double time = omp_get_wtime();//подсчет времени
		c1 = a1.parallelMult(b1, numThreads);
		time = omp_get_wtime() - time;
		cout << time << endl;//завершение подсчета времени и вывод
		c = c1.CCStoMatrix();

		Matrix::writeMatrix(c, mSize);//запись результата в файл

	}
	else if (choice == 2)
	{
		ifstream input("matrix.txt");//читаем из файла размерность матрицы
		input >> mSize;
		input.close();

		Matrix a(mSize, mSize), b(mSize, mSize), c(mSize, mSize);//создаем матрицы
		Matrix::readMatrix(a, b, mSize, "matrix.txt");
		MatrixCCS a1(a), b1(b), c1(c);

		double time = omp_get_wtime();//подсчет времени
		c1 = a1.parallelMult(b1, numThreads);
		time = omp_get_wtime() - time;
		cout << time << endl;//завершение подсчета времени и вывод
		c = c1.CCStoMatrix();

		Matrix::writeMatrix(c, mSize);//запись результата в файл
	}
	else
		cout << "Error" << endl;

	return 0;
}