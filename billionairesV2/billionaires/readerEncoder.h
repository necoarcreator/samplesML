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
	readerEncoder(); //������� �����������

	void writeFile(); //�������� ����� �������

	bool checkIfNotVoid(string input); //�������� �� ������� ��������

	bool check(char symb); //�������� �� �������� ������

	void checkUniqueFeatures(vector<string>* uniqueSphere, int featureNum); //����� ���������� �������� ��������

	void readParce(int numFeatures); //���������� �����

	void deleteCommas(string* input); //�������� ������� � �������� "��� ������"

	void deleteRow(int num); //�������� ��������

	void deleteLine(int num); //�������� ������� ������ ������

	void deleteAllUseless(); //�������� ���� ����������� ���������

	void deleteUselessSamples(double threshold = 0.05); //�������� ���� ������ ����� (�� ��������� ���� ������ ��������� � �� > 20%)

	void constructFeatureOneHot(vector<string> uniqueKeys, int featureNum); //��� ������ ������ �������������� ���������

	void constructFeatureTarget(vector<string> uniqueKeys, int numFeature); //��� �������� ������ �������������� ���������

	void turnIntoNumbers(vector<vector<double>>* output); //�������������� �������� � �����

	~readerEncoder();

};

#endif