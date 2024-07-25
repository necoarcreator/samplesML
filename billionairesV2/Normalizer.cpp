#include "Normalizer.h"

#include "readerEncoder.h"
#include <cfloat>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <functional>
using namespace std;
using namespace std::placeholders;
Normalizer::Normalizer(string fileName, vector<size_t>* uniqueCategoryFeatSizes)
{
	data = new vector<vector<double>>;

	readerEncoder A(fileName);
	
	
	A.readParce(35);
	names = { A.saveNames() };
	A.turnIntoNumbers(data);
	*uniqueCategoryFeatSizes = { A.getCategoryFeturesSize() };
	size = data->size(); 
	
	featureSize = (*data)[0].size();
}

Normalizer::~Normalizer()
{
	data->clear();
	delete data;
	size = 0;
	featureSize = 0;
}

void Normalizer::convertHonkongToChina() {
	//цель - заполнить пустые поля гонконгцев китайскими данными
	double usaValue = DBL_MIN;
	double chinaValue = DBL_MIN;
	int chinaIndex = 0;
	for_each(data->begin(), data->begin() + 100, [&](auto const& _n)
		{
			if (_n[30] > usaValue) //доля богатств США
			{
				usaValue = _n[30];
			}
			else if ((_n[30] > chinaValue) and (_n[30] <= usaValue)) //если это вторая по богатству страна в мире - то это Китай
			{
				chinaValue = _n[30];
				chinaIndex = find(data->begin(), data->begin() + 100, _n) - data->begin(); //для обращения к семплу китайских данных
			}
		});
	
	/*for (size_t i = 0; i < 100; i++) //достаточно прохода по 100 первым олигархам
	{
		if ((*data)[i][20] > usaValue) //доля богатств США
		{
			usaValue = (*data)[i][20];
		}
		else if (((*data)[i][20] > chinaValue) and ((*data)[i][20] <= usaValue)) //если это вторая по богатству страна в мире - то это Китай
		{
			chinaValue = (*data)[i][20];
			chinaIndex = i; //для обращения к семплу китайских данных
		}
	}
*/
	for_each(data->begin(), data->end(), [&](auto & _n)
		{
			auto g = bind (equal_to<double>(), _1, 0.0); //сравниваем с 0
			copy_if((*data)[chinaIndex].begin() + 30, (*data)[chinaIndex].end(), _n.begin() + 30, g); //если сравнивается, заменяем на китай
		});

	/*for (size_t i = 0; i < size; i++) //достаточно прохода по 100 первым олигархам
	{
		for (size_t j = 80; j < featureSize; j++) //костыльно начинаем с первого государственного (не относящегося к отдельному буржую) экономического признака 
		{ //лучше было бы написать функцию в readerEncoder, которая возвращала бы номер этого признака универсально после one-hot encoding
			if ((*data)[i][j] == 0) //заполняем только пустые поля
			{
				(*data)[i][j] = (*data)[chinaIndex][j];
			}
		}
	}
	*/
	return;
}

double const Normalizer::countMean(int numFeature)
{
	double mean = 0;
	for_each(data->begin(), data->end(), [&](auto const& _n)
		{
			mean += (_n[numFeature]);
			return;
		});
	/*
	for (size_t i = 0; i < size; i++)
	{
		mean += ((*data)[i][numFeature]);
	}
	*/
	mean /= size;
	if (mean < 1e-7)
	{
		return 0;
	}
	return mean;
}

double const Normalizer::countDispersion(int numFeature)
{
	double meanOfSquare = 0;
	double squareOfMean = pow(countMean(numFeature), 2);
	for_each(data->begin(), data->end(), [&](auto const& _n)
		{
			meanOfSquare += pow(_n[numFeature], 2);
			return;
		});
	/*
	for (size_t i = 0; i < size; i++)
	{
		meanOfSquare += pow((*data)[i][numFeature], 2);
	}
	*/
	meanOfSquare /= size - 1;

	return pow(meanOfSquare - squareOfMean, 0.5);
}

void Normalizer::minimaxNorm(int numFeature)
{
	double minValue = DBL_MAX;
	double maxValue = DBL_MIN;
	for_each(data->begin(), data->end(), [&](auto const& _n)
		{
			if ((_n[numFeature] < minValue) and (_n[numFeature] != 0))
			{
				minValue = _n[numFeature];
				return;
			}
			if (_n[numFeature] > maxValue)
			{
				maxValue = _n[numFeature];
				return;
			}
		});
	/*
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
	*/
	double mean = countMean(numFeature);

	for_each(data->begin(), data->end(), [&](auto  & _n)
		{
			if (_n[numFeature] == 0)
			{
				_n[numFeature] = mean / (maxValue - minValue);
				return;
			}
			else
			{
				_n[numFeature] = (_n[numFeature] - minValue) / (maxValue - minValue);
				return;
			}
		});

	/*for (size_t i = 0; i < size; i++)
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
	*/
	return;
}

void Normalizer::disperseNorm(int numFeature)
{
	double meanValue = countMean(numFeature);
	double dispersion = countDispersion(numFeature);

	for_each(data->begin(), data->end(), [&](auto& _n)
		{
			_n[numFeature] = (_n[numFeature] - meanValue) / dispersion;
			return;
		});
	
	/*for (size_t i = 0; i < size; i++)
	{
		(*data)[i][numFeature] = ((*data)[i][numFeature] - meanValue) / dispersion ;
	}*/
	return;
}

void Normalizer::disperseGaussianNorm(int numFeature)
{
	double meanValue = countMean(numFeature);
	double dispersion = countDispersion(numFeature);


	for_each(data->begin(), data->end(), [&](auto& _n)
		{
			_n[numFeature] = exp(-pow((_n[numFeature] - meanValue) / dispersion, 2));
			return;
		});
	/*for (size_t i = 0; i < size; i++)
	{
		(*data)[i][numFeature] = exp(-pow(((*data)[i][numFeature] - meanValue) / dispersion,2));
	}*/
	return;
}

void Normalizer::arcsinTransform(int numFeature) //для непрерывно распр данных, далёких от норм распр
{
	for_each(data->begin(), data->end(), [&](auto& _n)
		{
			if (abs(_n[numFeature]) <= 1)
			{
				_n[numFeature] = asin(_n[numFeature]);
				return;
			}
			else
			{
				cerr << "Error! Here's value > 1!\n";
				return;
			}
		});

	/*for (size_t i = 0; i < size; i++)
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
	}*/
	return;
}

void Normalizer::boxCoxTransform(int numFeature, double lambda, double coeff1, double coeff2) //для непрерывно распр данных, далёких от норм распр
{//lambda = 1/2 - преобразование по методу квадратного корня
	//lambda = 0 - преобразование по методу логарифма

	if (lambda == 0.0)
	{
		for_each(data->begin(), data->end(), [&](auto& _n)
			{
				_n[numFeature] = coeff2 * log(_n[numFeature] / coeff1);
				return;
			});
		
		/*for (size_t i = 0; i < size; i++)
		{
			(*data)[i][numFeature] = log((*data)[i][numFeature]/ coeff);
		}*/
		return;
	}
	for_each(data->begin(), data->end(), [&](auto& _n)
		{
			_n[numFeature] = (pow(_n[numFeature], lambda) - 1) / lambda;
			return;
		});
	/*
	for (size_t i = 0; i < size; i++)
	{
		(*data)[i][numFeature] = (pow((*data)[i][numFeature], lambda) - 1) / lambda;
	}*/
	return;
}
void Normalizer::tanhMeanTransform(int numFeature, bool useMean) //для непрерывно распр данных, далёких от норм распр
{
	if (useMean)
	{
		double mean = countMean(numFeature);
		double dispersion = countDispersion(numFeature);
		for_each(data->begin(), data->end(), [&](auto& _n)
			{
				_n[numFeature] = tanh((_n[numFeature] - mean) / dispersion);
				return;
			});

		/*for (size_t i = 0; i < size; i++)
		{
			(*data)[i][numFeature] = tanh(((*data)[i][numFeature] - mean)/dispersion);
		}*/
		return;
	}
	for_each(data->begin(), data->end(), [&](auto& _n)
		{
			_n[numFeature] = tanh(_n[numFeature]);
			return;
		});
	/*for (size_t i = 0; i < size; i++)
	{
		(*data)[i][numFeature] = tanh((*data)[i][numFeature]);
	}*/
	return;
		
}
void Normalizer::normalize(vector<vector<double>>* output)
{
	double minValue = DBL_MAX;
	double maxValue = DBL_MIN;
	boxCoxTransform(0, 0, 1000.0, 1/5.36); //преобразование логарифмом богатств с нормировкой на 1000;
	disperseGaussianNorm(19); // преобразование гауссовой функцией от разности значения - среднее / дисперсию для возраста: получается, что люди среднего возраста больше всего влияют?
	disperseNorm(30); //для индекса потребительских цен (он показывает величину инфляции, а чем она в сравнении с другими странами выше, тем лучше вести бизнес) применили сигмоид, сдвинутую на среднее/дисперсия
	disperseNorm(31); //для роста потребительских цен так же
	disperseNorm(32);
	disperseNorm(33);
	disperseNorm(34);
	disperseNorm(35);
	disperseNorm(36);
	disperseNorm(37);
	disperseNorm(38);
	
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
	for_each(data->begin(), data->end(), [&](auto& _n)
		{
			vector<double> outputRow(featureSize, 0.0);
			copy(_n.begin(), _n.end(), outputRow.begin());
			output->push_back(outputRow);
		});
	
	/*for (size_t i = 0; i < size; i++)
	{
		auto row = next(data->begin(), i);
		output->push_back(*row);
		//(*output)[i].insert((*output)[i].begin(), row->begin(), row->end());
	}*/
	return;
}

vector<string> Normalizer::getNames()
{
	return names;
}