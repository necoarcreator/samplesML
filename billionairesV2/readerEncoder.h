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
	string fileName;

	vector<size_t> uniqueLabelSizes;
	readerEncoder(const readerEncoder& something) = delete;

public:
	readerEncoder(string filename); //обычный конструктор

	void writeFile() const; //файловый вывод матрицы

	bool checkIfNotVoid(string input) const; //проверка на пустоту элемента

	bool check(char symb) const; //проверка на числовой символ

	void checkUniqueFeatures(vector<string>* uniqueSphere, int featureNum) const; //поиск уникальных значений признака

	void readParce(int numFeatures); //считывание файла

	void deleteCommas(string* input); //удаление запятых в признаке "ВВП страны"

	void deleteRow(long int num); //удаление признака

	void deleteLine(long int num); //удаление слишком пустой строки

	void deleteAllUseless(); //удаление всех бесполезных признаков

	void deleteUselessSamples(double threshold = 0.1); //удаление всех пустых строк (по умолчанию если пустых признаков у неё > 5%)

	void constructFeatureOneHot(vector<string> uniqueKeys, int featureNum); //для малого набора категориальных признаков

	void constructFeatureTarget(vector<string> uniqueKeys, int numFeature); //для большого набора категориальных признаков

	void constructFeatureSimple(vector<string> uniqueKeys, int featureNum); //для бинарных нечисловых признаков

	void turnIntoNumbers(vector<vector<double>>* output); //преобразование датасета в числа

	vector<string> saveNames(); //сохранить имена буржуев 

	vector<size_t> getCategoryFeturesSize();
	~readerEncoder();

};

#endif