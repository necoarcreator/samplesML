#include "Normalizer.h"

#include "readerEncoder.h"
#include <vector>
#include <string>
#include <iostream>

#include <math.h>

using namespace std;

Normalizer::Normalizer()
{
	data = new vector<vector<double>>;

	readerEncoder A;
	A.readParce(35);

	A.turnIntoNumbers(data);

	size = data->size();
	featureSize = (*data)[0].size();
}

Normalizer::~Normalizer()
{
	data->clear();
	delete data;
	size = 0;
	featureSize = 0;
	data = new vector<vector<double>>;
}

void Normalizer::convertHonkongToChina() {
	//���� - ��������� ������ ���� ���������� ���������� �������
	double usaValue = DBL_MIN;
	double chinaValue = DBL_MIN;
	int chinaIndex = 0;
	for (size_t i = 0; i < 100; i++) //���������� ������� �� 100 ������ ���������
	{
		if ((*data)[i][20] > usaValue) //���� �������� ���
		{
			usaValue = (*data)[i][20];
		}
		else if (((*data)[i][20] > chinaValue) and ((*data)[i][20] <= usaValue)) //���� ��� ������ �� ��������� ������ � ���� - �� ��� �����
		{
			chinaValue = (*data)[i][20];
			chinaIndex = i; //��� ��������� � ������ ��������� ������
		}
	}

	for (size_t i = 0; i < size; i++) //���������� ������� �� 100 ������ ���������
	{
		for (size_t j = 86; j < featureSize; j++) //��������� �������� � ������� ���������������� (�� ������������ � ���������� ������) �������������� �������� 
		{ //����� ���� �� �������� ������� � readerEncoder, ������� ���������� �� ����� ����� �������� ������������ ����� one-hot encoding
			if ((*data)[i][j] == 0) //��������� ������ ������ ����
			{
				(*data)[i][j] = (*data)[chinaIndex][j];
			}
		}
	}

	return;
}

double Normalizer::countMean(int numFeature)
{
	double mean = 0;
	for (size_t i = 0; i < size; i++)
	{
		mean += ((*data)[i][numFeature]);
		if ((*data)[i][19] == 49.0)
		{
			cout << (*data)[i][20];
		}
	}
	mean /= size;
	if (mean < 1e-7)
	{
		return 0;
	}
	return mean;
}

double Normalizer::countDispersion(int numFeature)
{
	double meanOfSquare = 0;
	double squareOfMean = pow(countMean(numFeature), 2);
	for (size_t i = 0; i < size; i++)
	{
		meanOfSquare += pow((*data)[i][numFeature], 2);
	}
	meanOfSquare /= size - 1;

	return pow(meanOfSquare - squareOfMean, 0.5);
}

void Normalizer::minimaxNorm(int numFeature)
{
	double minValue = DBL_MAX;
	double maxValue = DBL_MIN;

	for (size_t i = 0; i < size; i++)
	{
		if (((*data)[i][numFeature] < minValue) and ((*data)[i][numFeature] != 0))
		{
			minValue = (*data)[i][numFeature];
		}
		if ((*data)[i][numFeature] > maxValue)
		{
			maxValue = (*data)[i][numFeature];
		}
	}
	double mean = countMean(numFeature);
	for (size_t i = 0; i < size; i++)
	{
		if ((*data)[i][numFeature] == 0)
		{
			(*data)[i][numFeature] = mean / (maxValue - minValue);
		}
		else
		{
			(*data)[i][numFeature] = ((*data)[i][numFeature] - minValue) / (maxValue - minValue);
		}
	}
	return;
}

void Normalizer::disperseNorm(int numFeature)
{
	double meanValue = countMean(numFeature);
	double dispersion = countDispersion(numFeature);

	
	for (size_t i = 0; i < size; i++)
	{
		(*data)[i][numFeature] = ((*data)[i][numFeature] - meanValue) / dispersion ;
	}
	return;
}

void Normalizer::disperseGaussianNorm(int numFeature)
{
	double meanValue = countMean(numFeature);
	double dispersion = countDispersion(numFeature);

	for (size_t i = 0; i < size; i++)
	{
		(*data)[i][numFeature] = exp(-pow(((*data)[i][numFeature] - meanValue) / dispersion,2));
	}
}

void Normalizer::arcsinTransform(int numFeature) //��� ���������� ����� ������, ������ �� ���� �����
{
	size_t i = 0;
	for (size_t i = 0; i < size; i++)
	{
		if (abs((*data)[i][numFeature]) <= 1)
		{
			(*data)[i][numFeature] = asin((*data)[i][numFeature]);

		}
		else
		{
			cerr << "Error! Here's value > 1!\n";
			return;
		}
		i++;
	}
	return;
}

void Normalizer::boxCoxTransform(int numFeature, double lambda, double coeff) //��� ���������� ����� ������, ������ �� ���� �����
{//lambda = 1/2 - �������������� �� ������ ����������� �����
	//lambda = 0 - �������������� �� ������ ���������
	if (lambda == 0.0)
	{
		for (size_t i = 0; i < size; i++)
		{
			(*data)[i][numFeature] = log((*data)[i][numFeature]/ coeff);
		}
		return;
	}
	for (size_t i = 0; i < size; i++)
	{
		(*data)[i][numFeature] = (pow((*data)[i][numFeature], lambda) - 1) / lambda;
	}
	return;
}
void Normalizer::tanhMeanTransform(int numFeature, bool useMean) //��� ���������� ����� ������, ������ �� ���� �����
{
	if (useMean)
	{
		double mean = countMean(numFeature);
		double dispersion = countDispersion(numFeature);
		for (size_t i = 0; i < size; i++)
		{
			(*data)[i][numFeature] = tanh(((*data)[i][numFeature] - mean)/dispersion);
		}
		return;
	}
	for (size_t i = 0; i < size; i++)
	{
		(*data)[i][numFeature] = tanh((*data)[i][numFeature]);
	}
	return;
		
}
void Normalizer::normalize(vector<vector<double>>* output)
{
	double minValue = DBL_MAX;
	double maxValue = DBL_MIN;
	boxCoxTransform(0, 0, 1000.0); //�������������� ���������� �������� � ����������� �� 1000;
	disperseGaussianNorm(19); // �������������� ��������� �������� �� �������� �������� - ������� / ��������� ��� ��������: ����������, ��� ���� �������� �������� ������ ����� ������?
	disperseNorm(86); //��� ������� ��������������� ��� (�� ���������� �������� ��������, � ��� ��� � ��������� � ������� �������� ����, ��� ����� ����� ������) ��������� �������, ��������� �� �������/���������
	disperseNorm(87); //��� ����� ��������������� ��� ��� ��
	minimaxNorm(88);
	disperseNorm(89);
	disperseNorm(90);
	disperseNorm(91);
	disperseNorm(92);
	disperseNorm(93);
	minimaxNorm(94);
	
	/*int k = 94;
	for (size_t i = 0; i < size; i++)
	{
		if ((*data)[i][k] <  minValue)
		{
			minValue = (*data)[i][k];
		}
		if ((*data)[i][k] > maxValue)
		{
			maxValue = (*data)[i][k];
		}
		cout << (*data)[i][k] << endl;
	}
	cout << maxValue << endl;
	cout << minValue << endl;
	cout << countMean(k) << endl;
	cout << countDispersion(k);
	*/
	for (size_t i = 0; i < size; i++)
	{
		auto row = next(data->begin(), i);
		output->push_back(*row);
		//(*output)[i].insert((*output)[i].begin(), row->begin(), row->end());
	}
	return;
}