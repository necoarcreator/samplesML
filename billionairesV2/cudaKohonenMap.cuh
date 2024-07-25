#ifndef KOHONEN
#define KOHONEN

#include <vector>
#include <string>
#include <map>

#include <cuda.h>
#include "cuda_runtime.h"
using namespace std;


/*
class KohonenMap
{
private:

	enum E_ALGO { E_FAST, E_PRECISE };
	const string STR_ALGO[2] = { "fast", "precise" }; //??
	map<const unsigned int, unsigned int> clusterization;
	vector<vector<double>>* data;
	vector<vector<double>>* weights;
	vector<double> distanceSquared;
	int size;
	int featureSize;
	int numPoints;
	vector<string> namesData;
	vector<vector<string>> namesCells; //в каждой €чейке будет по несколько имЄн
	bool isTeached;
	map<size_t, vector<string>> resultOfWork;
	vector<string>* notChangedSamplesNames;
	size_t howMuchInOneCell;
	KohonenMap(const KohonenMap& something) = delete;

public:
	vector<size_t> findWinner(size_t i);
	double const findMaxOrMin(size_t numFeature, bool isMaxOrMin = true);
	KohonenMap(int _numPoints, bool countHonKong = false);
	KohonenMap(string fileToReadWeights);
	~KohonenMap();
	vector<vector<double>> littleChange(size_t whatSamplesToChange, double percent = 1.0);
	void training(int E_ALGO, int numEpoch); //по числу эпох
	void training(int E_ALGO, const double precision); //по точности прив€зки к узлам
	void countErr(vector<size_t> winnerIndex, double* err, int algorithm = 0);
	void updateWeights(size_t i, vector<size_t> winnerIndex, int epoch);
	double neighbourFunc(int winnerIndex, int featureIndex, int epoch, double sigma = 1);
	double alpha(int epoch);
	void const printClusterization();
	void const print(int featureIndex);
	void clusterize(const size_t whichOrder, const double percentOfChange, bool isRanJustAfterTrain);
	void writeToFile();
	void readData(string fromWhere, bool _countHonkong = false);
	bool check(char symb) const; //проверка на числовой символ
	double precisionRate();

	__device__  void findWinnerGPU();

};

#endif
*/