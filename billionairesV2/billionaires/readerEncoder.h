#ifndef READENCODE
#define READENCODE
using namespace std;
#include <vector>
#include <string>
class readerEncoder
{
private:
	vector<vector<string>>* result;
	int size;
	int featureSize;

	readerEncoder(const readerEncoder& something) = delete;

public:
	readerEncoder(); //обычный конструктор

	void writeFile(); //файловый вывод матрицы

	bool checkIfNotVoid(string input); //проверка на пустоту элемента

	bool check(char symb); //проверка на числовой символ

	void checkUniqueFeatures(vector<string>* uniqueSphere, int featureNum); //поиск уникальных значений признака

	void readParce(int numFeatures); //считывание файла

	void deleteCommas(string* input); //удаление запятых в признаке "ВВП страны"

	void deleteRow(int num); //удаление признака

	void deleteLine(int num); //удаление слишком пустой строки

	void deleteAllUseless(); //удаление всех бесполезных признаков

	void deleteUselessSamples(double threshold = 0.05); //удаление всех пустых строк (по умолчанию если пустых признаков у неё > 20%)

	void constructFeatureOneHot(vector<string> uniqueKeys, int featureNum); //для малого набора категориальных признаков

	void constructFeatureTarget(vector<string> uniqueKeys, int numFeature); //для большого набора категориальных признаков

	void turnIntoNumbers(vector<vector<double>>* output); //преобразование датасета в числа

	~readerEncoder();

};

#endif