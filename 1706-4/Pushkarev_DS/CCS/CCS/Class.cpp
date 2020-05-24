#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
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
	type* getP()
	{
		return &vv[0];
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

	static void readMatrix(Matrix& A, Matrix& B, int N)//для вызова классом
	{
		ifstream input("matrix.txt");
		for (int i = 0; i < A.vv.size() + B.vv.size(); i++)
		{
			if (i < A.vv.size())
				input >> A.vv[i];
			else
			{
				for(int j = 0; j < B.vv.size(); j++)
				input >> B.vv[j];
			}
		}
		input.close();
		A.transposition();
		B.transposition();
	}


	static void writeMatrix(Matrix &A, int N)//для вызова классом
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

};

class MatrixCCS
{
protected:
	vector<type> values;
	vector<int> rows;
	vector<int> pointer;
	int N;
public:
	double* value;
	int* row;
	int* point;
	//int sizematrix;
	int value_size;
	int point_size;
	int SM;
	int SR;//количество строк
	int SC;//количество столбцов
	int count_val = 0;
	//MatrixCCS() {}
	MatrixCCS(int n) :N(n) 	{}
	MatrixCCS(int n1, int n2, int n3)//n2 - количество строк, n3 - количество столбцов
	{
		SR = n2;
		SC = n3;
		if (n2 < n3) SM = n2;
		else SM = n3;
		//SM = n2;//matrix size
		//value = new int[n1 * n2];//выделяется память для матрицы n1*n2(n1-строки)
		value = new double[n1];//количество ненулевых элементов
		row = new int[n1];
		point = new int[SM + 1];
		value_size = n1;
		point_size = SM + 1;
		//sizematrix = n1 * n2;
	}
	MatrixCCS(const MatrixCCS& m)
	{
		this->value_size = m.value_size;
		this->count_val = m.count_val;
		this->N = m.N;
		this->SM = m.SM;
		this->point_size = m.point_size;
		this->value = new double[m.value_size];
		for (int i = 0; i < m.value_size; i++)
		{
			this->value[i] = m.value[i];
		}
		this->row = new int[m.value_size];
		for (int i = 0; i < m.value_size; i++)
		{
			this->row[i] = m.row[i];
		}
		this->point = new int[m.point_size];
		for (int i = 0; i < m.value_size; i++)
		{
			this->point[i] = m.point[i];
		}
	}
	MatrixCCS operator = (const MatrixCCS& m)
	{
		count_val = m.count_val;
		this->value_size = m.value_size;
		this->N = m.N;
		this->SM = m.SM;
		delete[] value;
		delete[] row;
		delete[] point;
		this->value = new double[m.value_size];
		for (int i = 0; i < m.value_size; i++)
		{
			this->value[i] = m.value[i];
		}
		this->row = new int[m.value_size];
		for (int i = 0; i < m.value_size; i++)
		{
			this->row[i] = m.row[i];
		}
		this->point = new int[m.point_size];
		for (int i = 0; i < m.value_size; i++)
		{
			this->point[i] = m.point[i];
		}
		return *this;
	}

	~MatrixCCS()
	{
		delete[] value;
		delete[] row;
		delete[] point;
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



	MatrixCCS operator * (const MatrixCCS& m)
	{
		int res_size;
		if (this->value_size == m.value_size)
			res_size = m.value_size;
		else if (this->value_size < m.value_size)
			res_size = this->value_size;
		else
			res_size = m.value_size;

		//int size_res_matrix;

		//count_val = 0;
		int curr_value = 0;//текущая позиция values
		int curr_row = 0;//текущая позиция row
		int curr_point = 0;//текущая позиция pointer

		MatrixCCS res(res_size, SR, m.SC);//результирующая матрица
		res.point[curr_point] = 0; curr_point++;
		//res.pointer.push_back(0);
		//transposition();
		int* cols = row;
		//vector<int>* cols = &rows;
		int elCountM = 0;
		for (int j = 0; j < m.SC; j++)
		{
			int numElInResCol = 0;
			const int numElementInCol = m.point[j + 1] - m.point[j];//вычисляется количество элементов в столбце
			if (numElementInCol == 0)
			{
				//int size = res.pointer.size();//размер pointer в результирующем векторе
			//int size = point_size;//размер pointer в результирующем векторе
				//res.pointer.push_back(res.pointer[size - 1]);//дублируем предыдущий(если в данном столбце ноль)
				res.point[curr_point] = res.point[curr_point - 1];//дублируем предыдущий(если в данном столбце ноль)
				curr_point++;
				continue;
			}
			int elCountThis = 0;
			for (int i = 0; i < SR; i++)
			{
				const int numElementInRow = point[i + 1] - point[i];//сколько элементов в строке
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
					//int colThis = (*cols)[elCountThis];//получает позицию ненулевого
					int colThis = cols[elCountThis];//получает позицию ненулевого
					int rowM = m.row[tmpElCountM];//получает позицию ненулевого в M
					if (colThis == rowM)//сравнивается на соответсвие
					{
						sum += value[elCountThis] * m.value[tmpElCountM];
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
					//res.values.push_back(sum);
					res.value[curr_value] = sum; curr_value++; res.count_val++;
					//res.rows.push_back(i);
					res.row[curr_row] = i; curr_row++;
					numElInResCol++;//сколько элементов в столбце
				}
			}
			//const int size = res.pointer.size();
			const int size = curr_point;
			//res.pointer.push_back(res.pointer[size - 1] + numElInResCol);
			res.point[curr_point] = res.point[curr_point - 1] + numElInResCol;
			curr_point++;
			elCountM += numElementInCol;
		}
		//transposition();

		for (int i = 0; i < res.count_val; i++)
			cout << res.value[i] << " ";
			cout << endl;

			for (int i = 0; i < res.count_val; i++)
				cout << res.row[i] << " ";
			cout << endl;
			for (int i = 0; i < res.point_size; i++)
				cout << res.point[i] << " ";
			cout << endl;

			//return 1;
		return res;
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

	static void readCCSMatrix(MatrixCCS& A, MatrixCCS& B)//для вызова классом
	{
		ifstream input("A.txt");

		for (int i = 0; i < A.value_size; i++)//value
		{
				input >> A.value[i];
			//дальше для row, для pointer
		}
		for (int i = 0; i < A.value_size; i++)//row
		{
				input >> A.row[i];
		}
		for (int i = 0; i < A.point_size; i++)
		{
			input >> A.point[i];
		}
		input.close();


		ifstream input1("B.txt");

		for (int i = 0; i < B.value_size; i++)//value
		{
			input1 >> B.value[i];
			//дальше для row, для pointer
		}
		for (int i = 0; i < B.value_size; i++)//row
		{
			input1 >> B.row[i];
		}
		for (int i = 0; i < B.point_size; i++)
		{
			input1 >> B.point[i];
		}
		input1.close();


	}

	static void writeToFile(MatrixCCS& A)//для вызова классом
	{
		ofstream input("resMatrix.txt");
		for (int i = 0; i < A.count_val; i++)
		{
			input << A.value[i] << "\t ";
		}
		input << endl;
		for (int i = 0; i < A.count_val; i++)
		{
			input << A.row[i] << "\t ";
		}
		input << endl;
		for (int i = 0; i < A.point_size; i++)
		{
			input << A.point[i] << "\t ";
		}
		input << endl;

		input.close();
	}


};


int main(int argc, char **argv)
{
	//Matrix a(4, 4), b(4, 4), c(4, 4);

	//Matrix::readMatrix(a, b, 4);

	int size;//количество ненулевых
	int r;// = 4;//количество строк
	int c; //= 4;//количество столбцов
	size = atoi(argv[1]);
	r = atoi(argv[2]);
	c = atoi(argv[3]);

	int size_;//количество ненулевых
	int r_;// = 4;//количество строк
	int c_; //= 4;//количество столбцов
	size_ = atoi(argv[4]);
	r_ = atoi(argv[5]);
	c_ = atoi(argv[6]);

	cout << size<<" " << r<<" " << c<<" " << size_<<" " << r_<<" " << c_<<" " << endl;


	MatrixCCS a1(size, r, c), b1(size_, r_, c_);
	//MatrixCCS a1(a), b1(b), c1(c);
	//MatrixCCS a1(5, 3, 4), b1(4, 4, 2);//, c1(4, 4, 4);//первый параметр - размер value

	int res_size;
	if (a1.value_size == b1.value_size)
		res_size = b1.value_size;
	else if (a1.value_size < b1.value_size)
		res_size = a1.value_size;
	else
		res_size = b1.value_size;

	MatrixCCS c1(res_size, a1.SR, b1.SC);



	MatrixCCS::readCCSMatrix(a1, b1);


	{
		for (int i = 0; i < a1.value_size; i++)
		{
			cout << a1.value[i] << " ";
		}
		cout << endl;
		for (int i = 0; i < a1.value_size; i++)
		{
			cout << a1.row[i] << " ";
		}
		cout << endl;
		for (int i = 0; i < a1.point_size; i++)
		{
			cout << a1.point[i] << " ";
		}

		cout << endl;
		cout << endl;

		for (int i = 0; i < b1.value_size; i++)
		{
			cout << b1.value[i] << " ";
		}
		cout << endl;
		for (int i = 0; i < b1.value_size; i++)
		{
			cout << b1.row[i] << " ";
		}
		cout << endl;
		for (int i = 0; i < b1.point_size; i++)
		{
			cout << b1.point[i] << " ";
		}

	}

	cout << endl;
	
	auto begin = std::chrono::steady_clock::now();
	c1 = a1* b1;
	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	std::cout << "The time: " << elapsed_ms.count() << " ms\n";


	MatrixCCS::writeToFile(c1);

	cout << endl;
	//cout<<c1.value[0];
	//c1 = a1 * b1;
	//c = c1.CCStoMatrix();

	//Matrix::writeMatrix(c, 4);

	return 0;
}