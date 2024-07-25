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

	void disperseNorm(int numFeature); //дисперсная нормировка

	void disperseGaussianNorm(int numFeature); //нормировка нормальным распределением для возраста

	void minimaxNorm(int numFeature); //нормировка минимаксная

	void arcsinTransform(int numFeature); //нормировка арксинусом??

	void boxCoxTransform(int numFeature, double lambda, double coeff1 = 1.0, double coeff2 = 1.0); //нормировка степенная/логарифмом

	void tanhMeanTransform(int numFeature, bool useMean = true); //стандартная сигмоида, сдвинутая на среднее

	void normalize(vector<vector<double>>* output);

	vector<string> getNames();

	~Normalizer();

};

#endif
