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
	readerEncoder(string filename); //������� �����������

	void writeFile() const; //�������� ����� �������

	bool checkIfNotVoid(string input) const; //�������� �� ������� ��������

	bool check(char symb) const; //�������� �� �������� ������

	void checkUniqueFeatures(vector<string>* uniqueSphere, int featureNum) const; //����� ���������� �������� ��������

	void readParce(int numFeatures); //���������� �����

	void deleteCommas(string* input); //�������� ������� � �������� "��� ������"

	void deleteRow(long int num); //�������� ��������

	void deleteLine(long int num); //�������� ������� ������ ������

	void deleteAllUseless(); //�������� ���� ����������� ���������

	void deleteUselessSamples(double threshold = 0.1); //�������� ���� ������ ����� (�� ��������� ���� ������ ��������� � �� > 5%)

	void constructFeatureOneHot(vector<string> uniqueKeys, int featureNum); //��� ������ ������ �������������� ���������

	void constructFeatureTarget(vector<string> uniqueKeys, int numFeature); //��� �������� ������ �������������� ���������

	void constructFeatureSimple(vector<string> uniqueKeys, int featureNum); //��� �������� ���������� ���������

	void turnIntoNumbers(vector<vector<double>>* output); //�������������� �������� � �����

	vector<string> saveNames(); //��������� ����� ������� 

	vector<size_t> getCategoryFeturesSize();
	~readerEncoder();

};

#endif