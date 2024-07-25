#ifndef KOHONEN
#define KOHONEN

#include <vector>
#include <string>

using namespace std;



class KohonenMap
{
private:

	enum E_ALGO { E_FAST, E_PRECISE };
	const string STR_ALGO[2] = {"fast", "precise"}; //??

	vector<vector<double>>* data;
	vector<vector<double>>* weights;
	vector<double> distanceSquared;
	int size;
	int featureSize;
	int numPoints;
	KohonenMap(const KohonenMap& something) = delete;

public:
	size_t findWinner(size_t i);
	KohonenMap(int _numPoints, bool countHonKong = false);
	~KohonenMap();
	void training(int E_ALGO, int numEpoch); //по числу эпох
	void training(int E_ALGO, double precision); //по точности привязки к узлам
	void countErr(int winnerIndex, double* err, int algorithm = 0);
	void updateWeights(size_t i, int winnerIndex, int epoch, double alpha = 0.5);
	
	void print(int featureIndex);

	void writeToFile();

};

#endif