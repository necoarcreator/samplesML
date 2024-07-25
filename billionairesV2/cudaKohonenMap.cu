#include <cuda.h>
#include "cuda_runtime.h"

#include "KohonenMap.h"
#include "Normalizer.h"

#include <cfloat>
#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <ctime>
#include <fstream>
#include <random>
#include <algorithm>
#include <functional>
#include <numeric>
#include <exception>
#include <new>
#include <sstream>
#include  <cctype>

//#define DEBUG

using namespace std;
using namespace std::placeholders;

KohonenMap::KohonenMap(int _numPoints, bool _countHonkong) : numPoints(_numPoints), isTeached(false)
{
	data = new vector<vector<double>>;
	weights = new vector<vector<double>>;
	Normalizer A("billionaires.csv");

	if (_countHonkong)
	{
		A.convertHonkongToChina();

	}
	A.normalize(data);
	size = data->size();
	featureSize = (*data)[0].size();
	howMuchInOneCell = size / numPoints; //сколько семплов кластеризуется в одной ячейке
	namesData = { A.getNames() };

	random_device rd;
	mt19937 g(rd());

	vector<size_t> index(size);
	iota(index.begin(), index.end(), 0); //создаём вектор индексов, начиная с 0
	shuffle(index.begin(), index.end(), g); //делаем его рандомным

	vector<vector<double>>* dataSafe = new vector<vector<double>>(size, vector<double>(featureSize)); //рандомное перемешивание по одному и тому же алгоритму
	vector<string> namesSafe = vector<string>(size);
	for_each(index.begin(), index.end(), [&](auto& _n)
		{
			size_t i = distance(index.begin(), find(index.begin(), index.end(), _n));

			(*dataSafe)[i] = (*data)[_n]; //вот здесь, собственно, орандомливание по одному закону
			namesSafe[i] = namesData[_n];
		});

	copy(dataSafe->begin(), dataSafe->end(), data->begin()); //меняем порядок даты и имён
	copy(namesSafe.begin(), namesSafe.end(), namesData.begin());


	vector<double> weightsRow(featureSize);
	random_device rd2;
	mt19937 gen(rd2());



	for (size_t i = 0; i < _numPoints; i++) //сори, это переписывать тупо лень
	{
		vector<double> weightsRow(featureSize);
		weights->push_back(weightsRow);
		clusterization[i + 1] = 0;

		for (size_t j = 0; j < featureSize; j++)
		{
			double maxFeat = findMaxOrMin(j, true);
			double minFeat = findMaxOrMin(j, false);
			if (maxFeat > 1e-7)
			{
				//(*weights)[i][j] = (double)(rand() % ((int)round(maxFeat))) + minFeat;
				if ((minFeat == 0.0) && (maxFeat == 1.0))
				{
					uniform_int_distribution<> dis((int)minFeat, (int)maxFeat);
					(*weights)[i][j] = (double)dis(gen);
				}
				else
				{
					uniform_real_distribution<> dis(minFeat, maxFeat);
					(*weights)[i][j] = dis(gen);
				}
			}
			else
			{
				(*weights)[i][j] = 0.0;
			}
			if (maxFeat != 0)
			{
				(*weights)[i][j] /= maxFeat;
			}


		}
	}

	//distanceSquared.push_back(0.0);
	fill_n(back_inserter(distanceSquared), numPoints, 0.0);

	/*for (size_t i = 0; i < numPoints; i++)
	{
		distanceSquared.push_back(0);
	}*/
	return;

}

vector<vector<double>> KohonenMap::littleChange(size_t whatSamplesToChange, double percent)
{

	vector<vector<double>> result(100, vector<double>(featureSize));
	for_each(data->begin() + (whatSamplesToChange - 1) * 100, data->begin() + (whatSamplesToChange) * 100, [&](auto& _n)
		{
			size_t i = distance(data->begin() + (whatSamplesToChange - 1) * 100,
			find(data->begin() + (whatSamplesToChange - 1) * 100, data->begin() + (whatSamplesToChange) * 100, _n));

	for_each(_n.begin(), _n.end(), [&](auto& _m)
		{
			size_t j = distance(_n.begin(), find(_n.begin(), _n.end(), _m));

			result[i][j] = _m + pow(-1, rand() % 2) * percent * _m / 100;//немного изменяем известные семплы в одну или другую сторону
			return;
		});
	return;
		});

	/*for (size_t i = (whatSamplesToChange - 1) * 100; whatSamplesToChange * 100 + 1; i++)
	{
		for (size_t j = 0; j < featureSize; j++)
		{
			(*data)[i][j] += pow(-1, rand() % 2) * (*data)[i][j] / 100; //немного изменяем известные семплы в одну или другую сторону
		}
	}*/
	return result;
}
double const KohonenMap::findMaxOrMin(size_t numFeature, bool isMaxOrMin)
{
	vector<double> arrayFeature(size);
	size_t i = 0;
	for_each(arrayFeature.begin(), arrayFeature.end(), [&](auto& _n)
		{

			_n = (*data)[i][numFeature];
			i++;
			return;
		});
	/*for (size_t i = 0; i < size; i++)
	{
		arrayFeature.push_back((*data)[i][numFeature]);
	}*/

	if (isMaxOrMin)
	{


		double maxValue = arrayFeature[max_element(arrayFeature.begin(), arrayFeature.end()) - arrayFeature.begin()];

		/*
		double maxValue = DBL_MIN;
		for (size_t i = 0; i < featureSize; i++)
		{
			if ((arrayFeature[i] > maxValue))
			{
				maxValue = arrayFeature[i];
			}
		}
		*/
		return maxValue;
	}
	//а если ищется минимальный элемент
	double minValue = arrayFeature[min_element(arrayFeature.begin(), arrayFeature.end()) - arrayFeature.begin()];
	/*
	double minValue = DBL_MAX;

	for (size_t i = 0; i < featureSize; i++)
	{

		if ((*data)[i][numFeature] < minValue)
		{
			minValue = arrayFeature[i];
		}
	}
	*/
	return minValue;

}
KohonenMap::~KohonenMap()
{
	data->clear();
	weights->clear();
	delete data;
	delete weights;
	size = 0;
	featureSize = 0;
	clusterization.clear();
}


vector<size_t> KohonenMap::findWinner(size_t k)
{
	size_t i = 0;

	for_each(weights->begin(), weights->end(), [&](auto& _n)
		{
			size_t j = 0;
			double temporaryDist = accumulate(_n.begin(), _n.end(), 0.0, [&](double acc, auto& _m) //сигнатура другая: первым аргументом идёт сумма, посчитанная ранее
				{
					j++;
					return acc + pow(_m - (*data)[k][j - 1], 2);
				});
			distanceSquared[i] = temporaryDist;
			//pow(temporaryDist, 0.5); //должно быть так, но относительные величины не особо поменяются, так что и квадрат расстояний сойдёт
			i++;
		});


	/*for (size_t i = 0; i < numPoints; i++)  //итерируемся по всем весам
	{
		double temporaryDist = 0.0;
		for (size_t j = 0; j < featureSize; j++)  //итерируемся по всем признакам
		{
			temporaryDist += pow((*weights)[i][j] - (*data)[k][j], 2);

		}
		distanceSquared[i] = pow(temporaryDist, 0.5);
	}*/

	//sort(distanceSquared.begin(), distanceSquared.end());
	vector<double> check = { distanceSquared };
	std::sort(check.begin(), check.end());
	vector<size_t> result;

	for (size_t i = 0; i < numPoints; i++)
	{
		double winnerValue = check[i];

		auto winnerIndex = find(distanceSquared.begin(), distanceSquared.end(), winnerValue);

		const auto pos = distance(distanceSquared.begin(), winnerIndex);

		result.push_back(pos);
	}
	clusterization[result[0]] += 1;
	return result; //индекс матрицы весов, а не начальных данных
}
/*
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
*/
void KohonenMap::countErr(vector<size_t> winnerIndex, double* err, int algorithm)
{

	if (algorithm == 0) //метод МНК
	{
		double summErr = accumulate(data->begin(), data->end(), 0.0, [&](double accc, auto const& _n)
			{
				size_t j = 0;
				double temporaryErr = accumulate(_n.begin(), _n.end(), 0.0, [&](double acc, auto const& _m) //сигнатура другая: первым аргументом идёт сумма, посчитанная ранее
					{
						j++;
						return acc + pow((*weights)[winnerIndex[0]][j - 1] - _n[j - 1], 2);

					});
				return accc + temporaryErr;
			});
		summErr = 0;

		for (size_t i = 0; i < size; i++)
		{
			double temporaryError = 0.0;
			for (size_t j = 0; j < featureSize; j++)
			{
				temporaryError += pow((*weights)[winnerIndex[0]][j] - (*data)[i][j], 2);
			}
			summErr += temporaryError;
			//pow(temporaryError, 0.5); //должно быть так, но если измерять квадрат ошибки, программа станет работать заметно быстрее, лол
		}

		*err += summErr / size;
		return;
	}

}
void KohonenMap::training(int E_ALGO, const double precision)
{
#ifdef DEBUG
	precision = 20.0;
#endif
	double* err = new double(0);
	double prevErr = DBL_MAX;
	int epochAnalogue = 1; //дабы не переписывать updateWeights лишний раз
	clock_t timeReq = clock();
	ofstream out("log.txt");
	while ((abs(prevErr - *err) > precision) && (epochAnalogue < 400))
	{
		//clusterization.clear(); //чтобы подсчёт был только по последней эпохе
		prevErr = *err;
		*err = 0;

		out << "%%%%%%%%%%%%%%%%%%%%%" << endl;
		out << "on epo = " << epochAnalogue << endl;
		for_each(weights->begin(), weights->end(), [&](auto const& _n)
			{
				out << _n[0] << endl;
			});

		/*for (auto x : (*weights))
		{
			out << x[0] << endl;

		}*/
		out << "%%%%%%%%%%%%%%%%%%%%%" << endl;
		for_each(data->begin(), data->end(), [&](auto& _n) //проходим по каждому семплу
			{
				size_t i = distance(data->begin(), find(data->begin(), data->end(), _n));
				vector<size_t> winnerIndex = { (*this).findWinner(i) };  //смотрим, какой вектор весов ближе всего к i-му семплу

				(*this).updateWeights(i, winnerIndex, epochAnalogue);
				(*this).countErr(winnerIndex, err, 0); //вычисляем ошибку: код 0 - по МНК
			});

		/*for (size_t i = 0; i < size; i++) //проходим по каждому семплу
		{
			vector<size_t> winnerIndex = { findWinner(i) };  //смотрим, какой вектор весов ближе всего к i-му семплу

			(*this).updateWeights(i, winnerIndex, epochAnalogue);
			(*this).countErr(winnerIndex, err, 0); //вычисляем ошибку: код 0 - по МНК
		}*/
		epochAnalogue++;
		if (! (epochAnalogue % 50))
		{
			timeReq = clock() - timeReq;
			cout << "It took " << timeReq << "to train our network for " << epochAnalogue << " epochs" << endl;
			cout << "Overall, the error is: " << *err << endl;
		}
		cout << prevErr - *err << endl;
#ifdef DEBUG
		break;
#endif

	}
	delete err;
	for_each(weights->begin(), weights->end(), [&](auto& _n)
		{
			auto g = bind(less<double>(), _1, 1e-7); //сравниваем с 1e-7
			replace_if(_n.begin(), _n.end(), g, 0.0); //если сравнивается, заменяем на 0
			return;
		});

	/*for (size_t i = 0; i < numPoints; i++)
	{
		for (size_t j = 0; j < featureSize; j++) {
			if ((*weights)[i][j] < 1e-7)
			{
				(*weights)[i][j] = 0.0;
			}
		}
	}
	*/
	out.close();
	isTeached = true;
	return;
}

void KohonenMap::updateWeights(size_t i, vector<size_t> winnerIndex, int epoch)
{
	double coeff = 1.0;
	if (epoch > 1) { coeff = 6.0; }

	for (size_t j = 0; j < featureSize; j++) //тоже проще записать через for
	{
		for (size_t k = 0; k < winnerIndex.size(); k++)
		{
			(*weights)[winnerIndex[k]][j] += (1 / pow(2, k)) * neighbourFunc(winnerIndex[k], j, epoch, 2) * ((*data)[i][j] - (*weights)[winnerIndex[k]][j]);
			if ((*weights)[winnerIndex[k]][j] > 1.0)
			{
				(*weights)[winnerIndex[k]][j] = 1.0;
			}
		}
	}
	return;
}
double KohonenMap::alpha(int epoch)
{
	double res = 100.0 + epoch;
	return 100.0 / res;
}

double KohonenMap::neighbourFunc(int winnerIndex, int featureIndex, int epoch, double sigma)
{
	/*double normSquared = accumulate(weights->begin(), weights->end(), 0.0, [&](double _m, auto& _n)
		{

			return _m + pow((*weights)[winnerIndex][featureIndex] - _n[featureIndex], 2);
		});
		*/


		/*for (size_t i = 0; i < numPoints; i++)
		{
			normSquared += pow((*weights)[winnerIndex][featureIndex] - (*weights)[i][featureIndex], 2);
		}*/
		//cout << alpha(epoch) * exp(-normSquared / (2 * pow(10, 2))) << endl;

	return alpha(epoch);
	//* exp(-normSquared / (2 * pow(sigma, 2)));


}

void const KohonenMap::printClusterization()
{
	cout << "********************************" << endl;
	for_each(clusterization.begin(), clusterization.end(), [](auto const& _n)
		{
			cout << "unit number " << _n.first << " had took " << _n.second << " samples in it" << endl;
			cout << "********************************" << endl;
		});
	/*for (auto it = clusterization.begin(); it != clusterization.end(); ++it)
	{
		cout << "unit number " << it->first << " had took " << it->second << " samples in it" << endl;
		cout << "********************************" << endl;
	}*/
}

void const KohonenMap::print(int featureIndex)
{
	cout << endl;
	for_each(weights->begin(), weights->end(), [&](auto const& _n)
		{
			cout << _n[featureIndex] << endl;
			return;
		});
	/*
	for (size_t i = 0; i < numPoints; i++)
	{
		cout <<  (*weights)[i][featureIndex] << endl;
	}
	*/
	return;
}

void KohonenMap::writeToFile()
{

	ofstream out("KohonenMap.txt");
	if (out.is_open())
	{
		for_each(weights->begin(), weights->end(), [&](auto const& _n)
			{
				size_t i = distance(weights->begin(), find(weights->begin(), weights->end(), _n));
				vector<size_t> winnerIndex = { (*this).findWinner(i) };  //смотрим, какой вектор весов ближе всего к i-му семплу

				for_each(_n.begin(), _n.end(), [&](auto const& _m)
					{
						out << _m << " ";

					});
				for (size_t k = 0; k < howMuchInOneCell; k++)
				{
					out << namesData[winnerIndex[k]] << " ,"; //чтобы не потерять имена буржуев
				}
				out << "\n";
			});

		/*for (size_t i = 0; i < numPoints; i++)
		{
			for (size_t j = 0; j < featureSize; j++) {
				out << (*weights)[i][j] << " ";
			}
			out << "\n";

		}*/
		cout << "Program ended successefully!\n";
	}
	else
	{
		cout << "Error while reading file!\n";
	}
	out.close();
}

KohonenMap::KohonenMap(string fileToReadWeights) : isTeached(true)
{

	data = new vector<vector<double>>;
	weights = new vector<vector<double>>;
	notChangedSamplesNames = new vector<string>(100, "");
	if (fileToReadWeights != "")
	{
		ifstream file1(fileToReadWeights);
		char delim = ' ';
		char delim1 = ',';
		size_t k = 0;
		if (file1.is_open())
		{
			string line, feature, name = "";

			while (getline(file1, line))
			{
				if (line == "") { break; }

				namesCells.push_back({});
				vector<double> weightsRow;
				stringstream sline(line);

				while (getline(sline, feature, ' '))
				{

					if (feature[0] == ',')
					{
						namesCells[k].push_back(name);

						string afterComma;
						string toBeSkipped;
						getline(sline, afterComma, delim);
						toBeSkipped.insert(toBeSkipped.begin(), feature.begin() + 1, feature.end());
						toBeSkipped.push_back('_');
						toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
						feature.clear();
						copy(toBeSkipped.begin(), toBeSkipped.end(), back_inserter(feature));

						name.clear();
					}

					if (check(feature[0]))
					{
						weightsRow.push_back(stod(feature));
					}
					else
					{
						if (name != "")
						{
							name += '_';
						}
						name += feature;
					}
				}

				weights->push_back(weightsRow);
				k++;
				name.clear();
			}

			size = weights->size();
			numPoints = size;
			featureSize = (*weights)[0].size();

		}
		else
		{
			cerr << "Error while opening file with weights to read!\n";
			exit(1);
		}
	}


	fill_n(back_inserter(distanceSquared), numPoints, 0.0);
}

void KohonenMap::readData(string fromWhere, bool _countHonkong)
{
	Normalizer A(fromWhere);
	if (_countHonkong)
	{
		A.convertHonkongToChina();

	}
	A.normalize(data);
	namesData = { A.getNames() };
	random_device rd;
	mt19937 g(rd());
	size_t dataSize = data->size();
	vector<size_t> index(dataSize);
	iota(index.begin(), index.end(), 0); //создаём вектор индексов, начиная с 0
	shuffle(index.begin(), index.end(), g); //делаем его рандомным

	vector<vector<double>>* dataSafe = new vector<vector<double>>(dataSize, vector<double>(featureSize)); //рандомное перемешивание по одному и тому же алгоритму
	vector<string> namesSafe = vector<string>(dataSize);
	for_each(index.begin(), index.end(), [&](auto& _n)
		{
			size_t i = distance(index.begin(), find(index.begin(), index.end(), _n));

			(*dataSafe)[i] = (*data)[_n]; //вот здесь, собственно, орандомливание по одному закону
			namesSafe[i] = namesData[_n];
		});

	copy(dataSafe->begin(), dataSafe->end(), data->begin()); //меняем порядок даты и имён
	copy(namesSafe.begin(), namesSafe.end(), namesData.begin());

	howMuchInOneCell = dataSize / size; //то, сколько семплов лежит в каждой ячейке
	return;
}
void KohonenMap::clusterize(const size_t whichOrder, const double percentOfChange, bool isRanJustAfterTrain)
{
	try
	{
		if (!isTeached) { throw "You forgot to train the network!\n"; }
	}
	catch (string e)
	{
		cerr << "Error: " << e;
		exit(1);
	}





	if (!isRanJustAfterTrain) //если эта функция запущена сразу после обучения и поля данных доступны прямо из класса, следует их преобразовать
	{

		/*try
		{
			if (size != numPoints)
			{
				throw 1.0; //нет смысла классифицировать, если карта неполна
			}
			data->clear();
			copy(weights->begin(), weights->end(), data->end()); //пытаемся скопировать веса в дату, чтобы не переписывать функции класса
		}
		catch (double er1)
		{
			cerr << "Can't transform data into weights for classification: try check dimension";
			exit(1);
		}
		catch (const bad_alloc& e)
		{
			cerr << "Can't transform data into weights for classification: " << e.what();
			exit(1);

		}
		catch (const exception& e)
		{
			cerr << "Can't transform data into weights for classification:  " << e.what();
			exit(1);
		}

		weights->clear(); //а вместо весов теперь будет тестовый набор
		*/
	}
	copy(namesData.begin() + (whichOrder - 1) * 100, namesData.begin() + (whichOrder) * 100, notChangedSamplesNames->begin());
	for_each(notChangedSamplesNames->begin(), notChangedSamplesNames->end(), [&](auto& _n)
		{
			replace(_n.begin(), _n.end(), ' ', '_');
		});



	vector<vector<double>> testSet = { littleChange(whichOrder, percentOfChange) }; //создаём тестовый набор из whichOrder'ной сотни немного изменённых буржуев 
	//и в обоих случаях заполняем веса нашей сотней тестовых примеров

	try
	{
		data->clear();
		namesData.clear();
		size_t i = 0;
		for_each(testSet.begin(), testSet.end(), [&](auto& _n)
			{
				data->push_back({});
				i++;
				(*data)[i - 1].insert((*data)[i - 1].end(), _n.begin(), _n.end());
			});
		testSet.clear();
	}
	catch (const exception& e)
	{
		cerr << "Can't transform weights into test set for classification:  " << e.what();
		exit(1);
	}

	//если всё получилось
	size_t i = 0;
	for_each(data->begin(), data->end(), [&](auto const& _n)
		{
			i++;

			size_t cellBest = (*this).findWinner(i - 1)[0]; //нашли лучшие соответстующие узлы карты
			//vecBest.erase(vecBest.begin() + howMuchInOneCell, vecBest.end()); //тут должно быть столько имён, на сколько прошла кластеризация

			resultOfWork[i - 1] = { namesCells[cellBest] }; //связываем его с именами буржуев
			weights->erase(weights->begin() + cellBest); //удаляем найденное со сдвигом
			size--;
			numPoints--;
			namesCells.erase(namesCells.begin() + cellBest);
			distanceSquared.erase(distanceSquared.begin() + cellBest);

		});

	string nameFile1 = to_string(whichOrder * 100);
	string nameFile2 = to_string((whichOrder - 1) * 100);
	string name = "KohonenClassifier" + nameFile2 + "_" + nameFile1 + ".txt";
	ofstream out2(name);
	if (out2.is_open())
	{
		for_each(resultOfWork.begin(), resultOfWork.end(), [&](auto const& _n)
			{
				out2 << _n.first << " ";
				for_each(_n.second.begin(), _n.second.end(), [&](auto const& _m)
					{
						out2 << _m << " ";
					});
				out2 << endl;
			});
		out2 << precisionRate();
	}
	else
	{
		cerr << "Error while writing test set output!\n";
	}
	out2.close();
	notChangedSamplesNames->clear();
	delete notChangedSamplesNames;
	return;
}
bool KohonenMap::check(char symb) const
{
	if (((static_cast<int>(symb) > 47) && (static_cast<int>(symb) < 58))
		|| (symb == ' ') || (symb == '\0'))
	{
		return true;
	}
	return false;
}
double KohonenMap::precisionRate()
{
	double rate = 0;
	vector<vector<string>> res(100, vector<string>(howMuchInOneCell));
	auto x = resultOfWork.begin();
	for (size_t i = 0; i < res.size(); i++)
	{
		for (size_t k = 0; k < howMuchInOneCell; k++)
		{
			res[i][k] = x->second[k];
		}

		x = next(resultOfWork.begin(), i + 1);
	}

	size_t i = 0;
	for_each(notChangedSamplesNames->begin(), notChangedSamplesNames->end(), [&](auto& _n)
		{
			i++;//ищем среди предсказанных имён реальные (классификдолжен работать с точностью до ячейки, в ней по howMuchInOneCell имён

			rate += count(res[i - 1].begin(), res[i - 1].end(), _n);

		});

	rate /= 100;
	return rate;

}

/*
__global__ void AddIntsCUDA(int* a, int* b)
{
	a[0] += b[0];
}
*/
/*
int main() {
	int a = 5, b = 9;
	int* d_a, *d_b;

	cudaMalloc(&d_a, sizeof(int));
	cudaMalloc(&d_b, sizeof(int));

	cudaMemcpy(d_a, &a, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_b, &b, sizeof(int), cudaMemcpyHostToDevice);

	AddIntsCUDA<<<1, 1>>>(d_a, d_b);

	cudaMemcpy(&a, d_a, sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(&b, d_b, sizeof(int), cudaMemcpyDeviceToHost);

	cout << a << endl;
	cudaFree(d_a);
	cudaFree(d_b);
	return 0;
}
*/
