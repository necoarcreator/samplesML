#include "KohonenMap.h"
#include "Normalizer.h"

#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <fstream>
using namespace std;


KohonenMap::KohonenMap(int _numPoints, bool _countHonkong) : numPoints(_numPoints)
{
	data = new vector<vector<double>>;
	weights = new vector<vector<double>>;

	Normalizer A;
	if (not _countHonkong)
	{
		A.convertHonkongToChina();

	}
	A.normalize(data);
	size = data->size();
	featureSize = (*data)[0].size();

	vector<double> weightsRow(featureSize);
	srand((unsigned int)time(0));

	for (size_t i = 0; i < _numPoints; i++)
	{
		vector<double> weightsRow(featureSize);
		weights->push_back(weightsRow);
		for (size_t j = 0; j < featureSize; j++)
		{
			(*weights)[i][j] = (double)(rand()) / RAND_MAX;

		}
	}

	for (size_t i = 0; i < numPoints; i++)
	{
		distanceSquared.push_back(0);
	}
}

KohonenMap::~KohonenMap()
{
	data->clear();
	weights->clear();
	delete data;
	delete weights;
	size = 0;
	featureSize = 0;
	data = new vector<vector<double>>;
	weights = new vector<vector<double>>;
}

size_t KohonenMap::findWinner(size_t k)
{
	double winnerValue = DBL_MAX;
	int winnerIndex = 0;
	
	for (size_t i = 0; i < numPoints; i++)  //итерируемся по всем весам
	{
		double temporaryDist = 0.0;
		for (size_t j = 0; j < featureSize; j++)  //итерируемся по всем признакам
		{
			temporaryDist += pow((*weights)[i][j] - (*data)[k][j], 2);

		}
		distanceSquared[i] = pow(temporaryDist, 0.5);

		if (distanceSquared[i] < winnerValue)
		{
			winnerValue = distanceSquared[i];
			winnerIndex = i;
		}
	}
	return winnerIndex; //индекс матрицы весов, а не начальных данных
}

void KohonenMap::training(int E_ALGO, int numEpoch)
{
	cout << "\n" << E_ALGO;

	size_t winnerIndex(0);

	for (size_t epoch = 1; epoch <= numEpoch; epoch++)
	{
		for (size_t i = 0; i < size; i++) //проходим по каждому семплу
		{
			winnerIndex = findWinner(i); //смотрим, какой вектор весов ближе всего к i-му семплу
			updateWeights(i, winnerIndex, epoch);
		}
	}

	for (size_t i = 0; i < numPoints; i++)
	{
		for (size_t j = 0; j < featureSize; j++) {
			if ((*weights)[i][j] < 1e-7)
			{
				(*weights)[i][j] = 0.0;
			}
		}
	}

	return;
}
void KohonenMap::countErr(int winnerIndex, double* err, int algorithm)
{
	
	if (algorithm == 0) //метод МНК
	{
		double summErr = 0.0;
		
		for (size_t i = 0; i < size; i++)
		{
			double temporaryError = 0.0;
			for (size_t j = 0; j < featureSize; j++)
			{
				temporaryError += pow((*weights)[winnerIndex][j] - (*data)[i][j], 2);
			}
			summErr += pow(temporaryError, 0.5);
		}
		*err += summErr/size;
		return;
	}

}
void KohonenMap::training(int E_ALGO, double precision)
{
	double* err = new double(0);
	int epochAnalogue = 1; //дабы не переписывать updateWeights лишний раз
	size_t winnerIndex(0);

	while (*err + 1> precision)
	{
		*err = 0;
		for (size_t i = 0; i < size; i++) //проходим по каждому семплу
		{
			winnerIndex = findWinner(i); //смотрим, какой вектор весов ближе всего к i-му семплу
			updateWeights(i, winnerIndex, epochAnalogue);
			countErr(winnerIndex, err, 0); //вычисляем ошибку: код 0 - по МНК
		}
		epochAnalogue++;
	}
	delete err;
	for (size_t i = 0; i < numPoints; i++)
	{
		for (size_t j = 0; j < featureSize; j++) {
			if ((*weights)[i][j] < 1e-7)
			{
				(*weights)[i][j] = 0.0;
			}
		}
	}
	return;
}

void KohonenMap::updateWeights(size_t i, int winnerIndex, int epoch, double alpha)
{
	for (size_t j = 0; j < featureSize; j++)
	{
		(*weights)[winnerIndex][j] += alpha * ((*data)[i][j] - (*weights)[winnerIndex][j]);
	}
	return;
}

void KohonenMap::print(int featureIndex)
{
	cout << endl;
	for (size_t i = 0; i < numPoints; i++)
	{

		cout <<  (*weights)[i][featureIndex] << endl;
	}
}

void KohonenMap::writeToFile()
{
	ofstream out("KohonenMap.txt");
	if (out.is_open())
	{
		for (size_t i = 0; i < numPoints; i++)
		{
			for (size_t j = 0; j < featureSize; j++) {
				out << (*weights)[i][j] << " ";
			}
			out << "\n";

		}
		cout << "Program ended successefully!\n";
	}
	else
	{
		cout << "Error while reading file!\n";
	}
	out.close();
}