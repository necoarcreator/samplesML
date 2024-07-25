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
	vector<string> names;
	Normalizer(const Normalizer& something) = delete;
public:
	Normalizer(string fileName, vector<size_t>* uniqueCategoryFeatSizes);

	void convertHonkongToChina();

	double const countMean(int numFeature);

	double const countDispersion(int numFeature);

	void disperseNorm(int numFeature); //���������� ����������

	void disperseGaussianNorm(int numFeature); //���������� ���������� �������������� ��� ��������

	void minimaxNorm(int numFeature); //���������� �����������

	void arcsinTransform(int numFeature); //���������� ����������??

	void boxCoxTransform(int numFeature, double lambda, double coeff1 = 1.0, double coeff2 = 1.0); //���������� ���������/����������

	void tanhMeanTransform(int numFeature, bool useMean = true); //����������� ��������, ��������� �� �������

	void normalize(vector<vector<double>>* output);

	vector<string> getNames();

	~Normalizer();

};

#endif
