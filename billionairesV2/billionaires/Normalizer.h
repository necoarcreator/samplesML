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

	void disperseNorm(int numFeature); //дисперсная нормировка

	void disperseGaussianNorm(int numFeature); //нормировка нормальным распределением для возраста

	void minimaxNorm(int numFeature); //нормировка минимаксная

	void arcsinTransform(int numFeature); //нормировка арксинусом??

	void boxCoxTransform(int numFeature, double lambda, double coeff = 1.0); //нормировка степенная/логарифмом

	void tanhMeanTransform(int numFeature, bool useMean = true); //стандартная сигмоида, сдвинутая на среднее

	void normalize(vector<vector<double>>* output);

	~Normalizer();

};

#endif
