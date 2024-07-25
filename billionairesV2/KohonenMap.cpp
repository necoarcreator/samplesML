#include "KohonenMap.h"
#include "Normalizer.h"

#include <cfloat>
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
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

KohonenMap::KohonenMap(int _gridSize, bool _countHonkong) : gridSize(_gridSize), isTeached(false)
{
	vector<size_t>* uniqueCategoryFeaturesSize = new vector<size_t>;
	data = new vector<vector<double>>;
	weights = new vector<vector<vector<double>>>(gridSize);
	Normalizer A("billionaires.csv", uniqueCategoryFeaturesSize);

	if (_countHonkong)
	{
		A.convertHonkongToChina();

	}
	A.normalize(data);
	size = data->size();
	featureSize = (*data)[0].size();
	howMuchInOneCell = size / gridSize / gridSize; //сколько семплов кластеризуется в одной ячейке
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



	random_device rd2;
	mt19937 gen(rd2());

	vector<vector<double>> weightsMtx(gridSize);
	vector<double> weightsRow(featureSize);


	for (size_t i = 0; i < gridSize; i++) //сори, это переписывать тупо лень
	{

		(*weights)[i] = weightsMtx;
		for (size_t j = 0; j < gridSize; j++)
		{
			size_t p = 0;
			size_t pLocal = 0;

			clusterization[i * gridSize + j + 1] = 0;
			(*weights)[i][j] = weightsRow;
			for (size_t k = 0; k < featureSize; k++)
			{
				double maxFeat = findMaxOrMin(k, true);
				double minFeat = findMaxOrMin(k, false);
				if (maxFeat > 1e-7)
				{
					//(*weights)[i][j] = (double)(rand() % ((int)round(maxFeat))) + minFeat;
					if (((minFeat == 0.0) and (maxFeat == 1.0)) and k < 128) // <30, т.к. там минимум 0, а максимум 1, но это не категориальные признаки
					{

						uniform_int_distribution<> dis(0, (*uniqueCategoryFeaturesSize)[p]); //здесь захардкожено - нужно вручную прописывать вероятности

						if ((int)dis(gen) == 1) { //с вероятностью 1/(*uniqueCategoryFeaturesSize)[p] это 1, в остальных случаях - 0
							(*weights)[i][j][k] = maxFeat;
						}
						else
						{
							(*weights)[i][j][k] = minFeat;
						}
						pLocal++;
						if (pLocal == (*uniqueCategoryFeaturesSize)[p]) { p++; pLocal = 0; }// контролируем, чтобы распределение было равномерным с вероятно
					}
					else
					{
						uniform_real_distribution<> dis(minFeat, maxFeat);
						(*weights)[i][j][k] = dis(gen);
					}
				}
				else
				{

					(*weights)[i][j][k] = 0.0;
				}
				if (maxFeat != 0.0 and maxFeat != 1.0)
				{
					(*weights)[i][j][k] /= maxFeat;
				}



			}

		}

	}
	
		
	//distanceSquared.push_back(0.0);
	fill_n(back_inserter(distanceSquared), gridSize * gridSize, 0.0);
	delete uniqueCategoryFeaturesSize;
	/*for (size_t i = 0; i < numPoints; i++)
	{
		distanceSquared.push_back(0);
	}*/
	return;

}

vector<vector<double>> KohonenMap::littleChange(size_t whatSamplesToChange, double percent)
{
	size_t i = (whatSamplesToChange - 1) * 100;
	vector<vector<double>> result(100, vector<double> (featureSize));
	for_each(data->begin() + (whatSamplesToChange - 1) * 100, data->begin() + (whatSamplesToChange) * 100, [&](auto& _n)
		{
			size_t j = 0;

			for_each(_n.begin(), _n.end(), [&](auto& _m)
				{
					result[i][j] = _m + pow(-1, rand() % 2) * percent * _m / 100;//немного изменяем известные семплы в одну или другую сторону
					j++;
					return;
				});
			i++;
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
		

		double maxValue = arrayFeature[max_element(arrayFeature.begin(), arrayFeature.end())- arrayFeature.begin()];

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

vector<pair<size_t, size_t>> KohonenMap::findWinner(size_t k)
{
	size_t i = 0;
	
	for_each(weights->begin(), weights->end(), [&](auto& _n)
		{
			size_t j = 0;
			for_each(_n.begin(), _n.end(), [&](auto& _m)
				{

					size_t q = 0;

					double temporaryDist = accumulate(_m.begin(), _m.end(), 0.0, [&](double acc, auto& _h) //сигнатура другая: первым аргументом идёт сумма, посчитанная ранее
						{
							double _d = (*data)[k][q];
							q++;
							return acc + pow(_h - _d, 2);
						});
					distanceSquared[gridSize*i + j] = temporaryDist;
					j++;
				});
			
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
	vector<size_t> result (gridSize*gridSize);
	
	for (size_t i = 0; i < gridSize; i++)
	{
		for (size_t j = 0; j < gridSize; j++)
		{
			double winnerValue = check[i*gridSize + j];

			vector<double>::iterator winnerIndex = find(distanceSquared.begin(), distanceSquared.end(), winnerValue);

			const auto pos = distance(distanceSquared.begin(), winnerIndex);
			
			result[i * gridSize + j] = (pos);

		}
	}
	
	clusterization[result[0]] += 1;
	//pair<size_t, size_t> resultClosest = make_pair(result[0] / gridSize, result[0] % gridSize);
	vector< pair<size_t, size_t>> resultClosest(9);

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			size_t gridY = result[0] / gridSize + i;
			size_t gridX = result[0] % gridSize + j;

			if (gridY < 0) {
				gridY = 0;
			}
			if (gridY >= gridSize)
			{
				gridY = gridSize - 1;
			}
			if (gridX < 0) {
				gridX = 0;
			}
			if (gridX >= gridSize)
			{
				gridX = gridSize - 1;
			}
			resultClosest[3 * (i + 1) + (j + 1)] = make_pair(gridY, gridX);
			
		}
	}
	swap(resultClosest[0], resultClosest[4]);
	return  resultClosest; //индекс ближайшего в сетке узла
}

void KohonenMap::countErr(pair<size_t, size_t> winnerIndex, double* err, int algorithm)
{
	
	if (algorithm == 0) //метод МНК
	{
		double summErr = accumulate(data->begin(), data->end(), 0.0, [&](double accc, auto const& _n)
			{
				size_t j = 0;
				double temporaryErr = accumulate(_n.begin(), _n.end(), 0.0, [&](double acc, auto const& _m) //сигнатура другая: первым аргументом идёт сумма, посчитанная ранее
					{
						j++;
						return acc + pow((*weights)[winnerIndex.first][winnerIndex.second][j-1] - _n[j-1], 2);
						
					});
				return accc + temporaryErr;
			});

		/*for (size_t i = 0; i < size; i++)
		{
			double temporaryError = 0.0;
			for (size_t j = 0; j < featureSize; j++) 
			{
				temporaryError += pow((*weights)[winnerIndex.first][winnerIndex.second][j] - (*data)[i][j], 2);
			}
			summErr += temporaryError;
				//pow(temporaryError, 0.5); //должно быть так, но если измерять квадрат ошибки, программа станет работать заметно быстрее, лол
		}
		*/
		*err += summErr/size;
		return;
	}

}
void KohonenMap::training(int E_ALGO, double precision, int howManyEpochsMax, double learning_rate)
{
#ifdef DEBUG
	precision = 20.0;
#endif
	double* err = new double(0);
	double prevErr = DBL_MAX;
	int epochAnalogue = 1; //дабы не переписывать updateWeights лишний раз
	
	auto timeStart = chrono::high_resolution_clock::now();
	ofstream out("log.txt");
	while ((abs(prevErr - *err) > precision) && (epochAnalogue <  howManyEpochsMax))
	{
		//clusterization.clear(); //чтобы подсчёт был только по последней эпохе
		prevErr = *err;
		*err = 0;
		
		out << "%%%%%%%%%%%%%%%%%%%%%" << endl;
		out << "on epo = " << epochAnalogue << endl;
		for_each(weights->begin(), weights->end(), [&](auto const& _n)
			{
				for_each(_n.begin(), _n.end(), [&](auto const& _m)
					{
						out << _m[0] << endl;
					});
			});

			/*for (auto x : (*weights))
			{
				out << x[0] << endl;

			}*/
		out << "%%%%%%%%%%%%%%%%%%%%%" << endl;
		size_t k = 0;
		for_each(data->begin(), data->end(), [&](auto& _d) //проходим по каждому семплу
			{
				pair<size_t, size_t> winnerIndex = (*this).findWinner(k)[0]; //смотрим, какой из узлов ближе всего к i-му семплу
				
				size_t i = 0;

				for_each(weights->begin(), weights->end(), [&](auto& _n)
					{
						size_t j = 0;

						for_each(_n.begin(), _n.end(), [&](auto& _m)
							{ 

								double dist = pow(winnerIndex.first - i, 2) + pow(winnerIndex.second - j, 2);
								double influence = exp(-dist / (2 * pow(epochAnalogue / 2.0, 2))); //функция влияния на соседей

								size_t p = 0;
								for_each(_d.begin(), _d.end(), [&](auto& _f)
									{
										_m[p] += influence * learning_rate * (_d[p] - _m[p]);
										p++;
									});
								//(*this).updateWeights(i, j, winnerIndex, influence);
								j++;
							});
						i++;
					});

				(*this).countErr(winnerIndex, err, 0); //вычисляем ошибку по близости *только* BMU к данным: код 0 - по МНК
				k++;
		});
		
		/*for (size_t i = 0; i < size; i++) //проходим по каждому семплу
		{
			vector<size_t> winnerIndex = { findWinner(i) };  //смотрим, какой вектор весов ближе всего к i-му семплу
			
			(*this).updateWeights(i, winnerIndex, epochAnalogue);
			(*this).countErr(winnerIndex, err, 0); //вычисляем ошибку: код 0 - по МНК
		}*/
		learning_rate *= 0.95;
		epochAnalogue++;
		if (not (epochAnalogue % 50))
		{
			auto timeReq = chrono::high_resolution_clock::now();
			cout << "It took " << chrono::duration_cast<std::chrono::seconds>(chrono::duration<double>(timeReq - timeStart)).count() << " seconds to train our network for " << epochAnalogue << " epochs" << endl;
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
			for_each(_n.begin(), _n.end(), [&](auto& _m)
				{
					auto g = bind(less<double>(), _1, 1e-7); //сравниваем с 1e-7
					replace_if(_m.begin(), _m.end(), g, 0.0); //если сравнивается, заменяем на 0
					return;
				});
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
/*
void KohonenMap::updateWeights(size_t i, vector<size_t> winnerIndex, int epoch)
{
	double coeff = 1.0;
	if (epoch > 1) { coeff = 6.0; }

	for (size_t j = 0; j < featureSize; j++) //тоже проще записать через for
	{
		for (size_t k = 0; k < winnerIndex.size(); k++)
		{
			(*weights)[winnerIndex[k]][j] += (1 / pow(2, k)) *  neighbourFunc(winnerIndex[k], j, epoch, 2) * ((*data)[i][j] - (*weights)[winnerIndex[k]][j]);
			if ((*weights)[winnerIndex[k]][j] > 1.0)
			{
				(*weights)[winnerIndex[k]][j] = 1.0;
			}
		}
	}
	return;
}
*/
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
			for_each(_n.begin(), _n.end(), [&](auto const& _m)
				{
					cout << _m[featureIndex] << endl;
					return;
				});

		});
	/*
	for (size_t i = 0; i < numPoints; i++)
	{
		cout <<  (*weights)[i][featureIndex] << endl;
	}
	*/
	return;
}
vector<string> KohonenMap::findClosestDataName(pair<size_t, size_t> coord)
{
	vector<double> distanceSquaredData(size);
	size_t i = 0;
	for_each(data->begin(), data->end(), [&](auto& _n)
		{
			size_t j = 0;
			double temporaryDist = accumulate(_n.begin(), _n.end(), 0.0, [&](double res, auto& _m)
				{
					double _h = (*weights)[coord.first][coord.second][j];
					j++;
					return res + pow(_h - _m, 2);
					
				});

			distanceSquaredData[i] = temporaryDist;
					//pow(temporaryDist, 0.5); //должно быть так, но относительные величины не особо поменяются, так что и квадрат расстояний сойдёт
			i++;
		});
			

			vector<double> check = { distanceSquaredData };
			std::sort(check.begin(), check.end());
			vector<string> result(howMuchInOneCell);

			
			
			bool isUnique = false;
			size_t epo = 0;
			size_t k = 0;
			size_t uniqueIt = 0;
			while (!isUnique) //крутимся, пока не добудем уникальные ближайшие семплы
			{
				cout << "this is " << epo << "iteration" << endl;

				unordered_set<string> numSet;
				
				if (k == 0)
				{
					for_each(result.begin(), result.end(), [&](auto& _n)
						{
							double winnerValue = check[k];

							auto winnerIndex = find(distanceSquaredData.begin(), distanceSquaredData.end(), winnerValue);

							const auto pos = distance(distanceSquaredData.begin(), winnerIndex);

							_n = (namesData[pos]);
							k++;
							
						});
				}
				else  //для повторных проходов
				{
					for_each(result.begin() + k, result.end(), [&](auto& _n)
						{
							double winnerValue = check[uniqueIt]; //uniqueIt будет указывать на массив расстояний

							auto winnerIndex = find(distanceSquaredData.begin(), distanceSquaredData.end(), winnerValue);

							const auto pos = distance(distanceSquaredData.begin(), winnerIndex);

							_n = (namesData[pos]);
							uniqueIt++;
						});
				}

				
				auto iter = stable_partition(result.begin(), result.end(), [&](auto &_n) //нужно чтобы убрать неуникальные значения, сохранив порядок
					{ bool ret = !numSet.count(_n); numSet.insert(_n); return ret; 
					}); // возвращает истину если элемент не был замечен ранее в массиве
				result.erase(iter, result.end());
				if (result.size() == howMuchInOneCell) { isUnique = true; } //закончим итерироваться, все значения уже уникальные
				else
				{
					k = distance(result.begin(), result.end()); //указывает на первое незаполненное значение
					while (result.size() < howMuchInOneCell) { result.push_back(""); } //добиваем до нужного размера обратно
				}
				epo++;
			}
			

			return  result; //индекс ближайшего в сетке узла
}
void KohonenMap::writeToFile()
{
	
	ofstream out("KohonenMap.txt");
	if (out.is_open())
	{
				size_t i = 0;

				for_each(weights->begin(), weights->end(), [&](auto const& _n)
					{
						size_t j = 0;
						for_each(_n.begin(), _n.end(), [&](auto const& _m)
							{
								for_each(_m.begin(), _m.end(), [&](auto const& _p)
									{
										out << _p << " "; //выписываем все фичи i-го узла
									});
								 


								pair<size_t, size_t> winnerIndex = make_pair(i, j);  //смотрим, какой семпл ближе всего к k-му узлу (т.е. обратная задача)
								vector<string> namesInCell = { (*this).findClosestDataName(winnerIndex) };
								for_each(namesInCell.begin(), namesInCell.end(), [&](auto const& _c)
									{
										out << _c << " ,"; //и после этого в эту же строчку - имена с ближайшей даты
										cout << _c << " ,";
										size_t t = distance(namesData.begin(), find(namesData.begin(), namesData.end(), _c)); //должно попадать единожды
										
										cout << t << endl;
										
										if (t < namesData.size())
										{
											namesData.erase(find(namesData.begin(), namesData.end(), namesData[t])); //дабы не повторяться 
											data->erase(find(data->begin(), data->end(), (*data)[t]));
											size--;
										}
										else
										{
											cerr << "Error while synchronizing data and names!" << endl;
											exit(1);
										}

									});
								cout << "____________________" << endl;
								out << "\n";
								j++;
							});
						i++;
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

KohonenMap::KohonenMap(string fileToReadWeights, size_t _gridSize) : isTeached(true), gridSize(_gridSize)
{

	data = new vector<vector<double>>;
	weights = new vector<vector<vector<double>>>(gridSize);
	namesCells = new  vector<vector<vector<string>>>(gridSize);
	vector<vector<double>> weightsMtx(gridSize);
	vector<vector<string>> namesCellsMtx(gridSize);
	resultOfWork = vector<map<size_t, vector<string>>>(9);
	notChangedSamplesNames = new vector<string>(100, "");
	if (fileToReadWeights != "")
	{
		ifstream file1(fileToReadWeights);
		char delim = ' ';
		char delim1 = ',';
		size_t i = 0;
		size_t k = 0;
		if (file1.is_open())
		{
			string line, feature, name = "";

			vector<double> weightsRow;

			while (getline(file1, line))
			{
				if (line == "") { break; }

				if(! (k % gridSize) )
				{
					if (k != 0) { k = 0; i++; } //на новый ряд
					(*weights)[i] = weightsMtx;
					(*namesCells)[i] = namesCellsMtx;
					
				}
				

				stringstream sline(line);

				while (getline(sline, feature, ' '))
				{

					if (feature[0] == ',')
					{
						(*namesCells)[i][k].push_back(name);

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
				(*weights)[i][k] = weightsRow;
				k++;
				name.clear();
				weightsRow.clear();
			}

			size = gridSize;
			featureSize = (*weights)[0][0].size();
			
		}
		else
		{
			cerr << "Error while opening file with weights to read!\n";
			exit(1);
		}
	}
	

	fill_n(back_inserter(distanceSquared), gridSize * gridSize, 0.0);
}

void KohonenMap::readData(string fromWhere, bool _countHonkong)
{
	vector <size_t>* nullV = new vector<size_t>;
	Normalizer A(fromWhere, nullV);
	delete nullV;
	if ( _countHonkong)
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

	vector<vector<double>>* dataSafe = new vector<vector<double>> (dataSize, vector<double>(featureSize)); //рандомное перемешивание по одному и тому же алгоритму
	vector<string> namesSafe = vector<string>(dataSize);
	for_each(index.begin(), index.end(), [&](auto& _n)
		{
			size_t i = distance(index.begin(), find(index.begin(), index.end(), _n));
			
			(*dataSafe)[i] = (*data)[_n]; //вот здесь, собственно, орандомливание по одному закону
			namesSafe[i] = namesData[_n];
		});

	copy(dataSafe->begin(), dataSafe->end(), data->begin()); //меняем порядок даты и имён
	copy(namesSafe.begin(), namesSafe.end(), namesData.begin());

	howMuchInOneCell = dataSize / gridSize / gridSize; //то, сколько семплов лежит в каждой ячейке
	return;
}
void KohonenMap::clusterize(const size_t whichOrder, const double percentOfChange, bool isRanJustAfterTrain)
{
	try
	{
		if (not isTeached) { throw "You forgot to train the network!\n"; }
	}
	catch (string e)
	{
		cerr << "Error: " << e;
		exit(1);
	}





	if (isRanJustAfterTrain) //если эта функция запущена сразу после обучения и поля данных доступны прямо из класса, следует их преобразовать
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
					
					(*data)[i].insert((*data)[i].end(), _n.begin(), _n.end());

					i++;
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
			

			vector<pair<size_t, size_t>> cellBest = (*this).findWinner(i); //нашли лучшие соответстующие узлы карты
			//vecBest.erase(vecBest.begin() + howMuchInOneCell, vecBest.end()); //тут должно быть столько имён, на сколько прошла кластеризация
			for (size_t k = 0; k < cellBest.size(); k++)
			{
				resultOfWork[k][i] = { (*namesCells)[cellBest[k].first][cellBest[k].second]}; //связываем его с именами буржуев
			}
			

			i++;
			/*
			weights->erase(weights->begin() + cellBest); //удаляем найденное со сдвигом
			size--;
			numPoints--;
			namesCells.erase(namesCells.begin() + cellBest);
			distanceSquared.erase(distanceSquared.begin() + cellBest);
			*/

		});

	string nameFile1 = to_string(whichOrder * 100);
	string nameFile2 = to_string((whichOrder - 1) * 100);
	string name = "KohonenClassifier" + nameFile2 + "_" + nameFile1 + ".txt";
	ofstream out2(name);
	if (out2.is_open())
	{
		for_each(resultOfWork.begin(), resultOfWork.end(), [&](auto const& _n)
			{
				for_each(_n.begin(), _n.end(), [&](auto const& _m)
					{
						out2 << _m.first << " ";
					
				
				for_each(_m.second.begin(), _m.second.end(), [&](auto const& _p)
					{
						out2 << _p << " ";
					});
				out2 << endl;
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
	if (((static_cast<int>(symb) > 47) and (static_cast<int>(symb) < 58))
		|| (symb == ' ') || (symb == '\0'))
	{
		return true;
	}
	return false;
}
double KohonenMap::precisionRate()
{
	double rate = 0;
	vector<vector<vector<string>>> res(resultOfWork.size(), vector <vector<string>>(100, vector<string>(howMuchInOneCell)));
	for (size_t l = 0; l < resultOfWork.size(); l++)
	{
		auto x = resultOfWork[l].begin();
		for (size_t i = 0; i < res[l].size(); i++)
		{
			for (size_t k = 0; k < x->second.size(); k++)
			{
				res[l][i][k] = x->second[k];
				
			}

			x = next(resultOfWork[l].begin(), i + 1);
		}
	}
	
	
	size_t i = 0;
	size_t sum = 0;
	
	for_each(notChangedSamplesNames->begin(), notChangedSamplesNames->end(), [&](auto& _n)
		{

			//ищем среди предсказанных имён реальные (классификдолжен работать с точностью до ячейки, в ней по howMuchInOneCell имён
			
				for (size_t k = 0; k < 1; k++)
				{
					rate += count(res[k][i].begin(), res[k][i].end(), _n);
				}

			i++;

		});
	rate /= 100;
	return rate;

}
/*
vector<size_t> KohonenMap::findWinnerMPI(size_t i)
{
	vector<vector<double>> weightsPart(volume);

	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int volume = numPoints / size;
	if (rank == 0)
	{
		vector<vector<double>> weightsPart(volume);
		vector<vector<double>> weightsPartLast(volume + numPoints % size);

		for (int i = 1; i < size - 1; i++) MPI_Send(copy(weights->begin() + i* volume, weights->begin() + i * (volume + 1)), volume, MPI_DOUBLE, i, 5, MPI_COMM_WORLD);
		MPI_Send(copy(weights->begin() + (size-1) * volume, weights->end()), volume + numPoints % size, MPI_DOUBLE, size, 5, MPI_COMM_WORLD);


	}
	else
	{
		MPI_Recv(&interval, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}


	int MPI_Get_Processor_name(char* name, int* len);
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
	/*vector<double> check = {distanceSquared};
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
	int MPI_Finalize(void);
	return result; //индекс матрицы весов, а не начальных данных
}
*/