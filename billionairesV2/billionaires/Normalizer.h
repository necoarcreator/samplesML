#ifndef NORMALIZER
#define NORMALIZER

#include <string>
#include <vector>
using namespace std;

class Normalizer
{
private:
	vector<vector<double>>* data;
	int size;
	int featureSize;
	Normalizer(const Normalizer& something) = delete;
public:
	Normalizer();

	void convertHonkongToChina();

	double countMean(int numFeature);

	double countDispersion(int numFeature);

	void disperseNorm(int numFeature); //���������� ����������

	void disperseGaussianNorm(int numFeature); //���������� ���������� �������������� ��� ��������

	void minimaxNorm(int numFeature); //���������� �����������

	void arcsinTransform(int numFeature); //���������� ����������??

	void boxCoxTransform(int numFeature, double lambda, double coeff = 1.0); //���������� ���������/����������

	void tanhMeanTransform(int numFeature, bool useMean = true); //����������� ��������, ��������� �� �������

	void normalize(vector<vector<double>>* output);

	~Normalizer();

};

#endif
