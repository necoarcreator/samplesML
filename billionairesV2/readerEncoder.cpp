#include "readerEncoder.h"
#include <stdio.h>
#include <string> 
#include <sstream>
#include <iostream> //файловый ввод
#include <fstream> //файловый ввод
#include <array> //для динамического массива чисел 
#include<algorithm> //для сортировки
#include <stdlib.h> //для exit
#include <vector>
#include <functional>
#include <math.h>

using namespace std;

readerEncoder::readerEncoder(string filename)
{
	result = new vector<vector<string>>;
	size = 0;
	featureSize = 0;
	fileName = filename;
}

readerEncoder::~readerEncoder()
{
	result->clear();
	delete result;
	size = 0;
	featureSize = 0;
}

void readerEncoder::writeFile() const
{
	ofstream out("tp.txt");
	if (out.is_open())
	{
		for_each(result->begin(), result->end(), [&](const auto& _n) //мудрый принт
			{
				for_each(_n.begin(), _n.end(), [&](const auto& m)
					{
						out << m;
					});
				out << "\n";
			});
		cout << "writing ended successefully!\n";
	}
	else
	{
		cerr << "Error while reading file!\n";
	}
	out.close();
	return;
	/*for (size_t n = 0; n < size; n++)
		{
			for (size_t i = 0; i < featureSize; i++) {
				out << (*result)[i][n];
			}
			out << "\n";

		}*/
}
bool readerEncoder::checkIfNotVoid(string input) const
{
	if (input[0])
	{
		return true;
	}
	return false;
}
bool readerEncoder::check(char symb) const
{
	if (((static_cast<int>(symb) > 47) and (static_cast<int>(symb) < 58))
		|| (symb == ' ') || (symb == '\0'))
	{
		return true;
	}
	return false;
}
void readerEncoder::checkUniqueFeatures(vector<string>* uniqueSphere, int featureNum) const
{
	uniqueSphere->push_back((*result)[0][featureNum]);
	for_each(result->begin(), result->end(), [=](auto const& _n)
		{
			auto it = find_if(uniqueSphere->begin(), uniqueSphere->end(), [=](auto const& _m) //находим уникальные ключи
				{
					return _m == _n[featureNum];
				});

			if (not (it != uniqueSphere->end()))
			{
				uniqueSphere->push_back(_n[featureNum]);
			}
		});

	/*for (size_t i = 0; i < size; i++)
	{
		
		for (size_t j = 0; j < uniqueSphere->size(); j++)
		{
			if ((*uniqueSphere)[j] == (*result)[i][featureNum])
			{
				isUnique = false;
				//cout << j << endl;
				break;
			}
		}
		if (isUnique)
		{
			uniqueSphere->push_back((*result)[i][featureNum]);
		}
	}*/

	return;
}
void readerEncoder::readParce(int numFeatures)
{
	//vector<string> res(numFeatures, "0");
	vector<string> res(numFeatures, "0");
	ifstream file(fileName);
	char delim = ',';
	char delimBefore1 = '"';
	char delimBefore2 = '$';
	char delimAfter = '"';
	vector<string> uniqueSphere;
	if (file.is_open())
	{
		string line;
		getline(file, line); //headline
		size_t i = 0;
		while (getline(file, line))
		{
			stringstream sline(line);
			string rank, finalWorth, category, personName, age, country, city,
				source, industries, countryOfCitizenship, organization, selfMade,
				status, gender, birthDate, lastName, firstName, title, date,
				state, residenceStateRegion, birthYear, birthMonth, birthDay,
				cpi_country, cpi_change_country, gross_tertiary_education_enrollment,
				gross_primary_education_enrollment_country, life_expectancy_country,
				tax_revenue_country_country, total_tax_rate_country, population_country,
				latitude_country, longitude_country;
			string toBeSkipped;
			string* gdp_country = new string;

			getline(sline, rank, delim);
			getline(sline, finalWorth, delim);
			getline(sline, category, delim);
			getline(sline, personName, delim); //сюда надо проверку на скобки
			if (personName[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				toBeSkipped.insert(toBeSkipped.begin(), personName.begin() + 1, personName.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				personName.clear();
				copy(toBeSkipped.begin(), toBeSkipped.end(), back_inserter(personName));
			}
			getline(sline, age, delim);
			getline(sline, country, delim);
			if (country[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				toBeSkipped.insert(toBeSkipped.begin(), country.begin() + 1, country.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				country.clear();
				copy(toBeSkipped.begin(), toBeSkipped.end(), back_inserter(personName));
			}
			getline(sline, city, delim);
			if (city[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				toBeSkipped.insert(toBeSkipped.begin(), city.begin() + 1, city.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				city.clear();
				copy(toBeSkipped.begin(), toBeSkipped.end(), back_inserter(city));
			}
			getline(sline, source, delim);
			if (source[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				toBeSkipped.insert(toBeSkipped.begin(), source.begin() + 1, source.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				source.clear();
				copy(toBeSkipped.begin(), toBeSkipped.end(), back_inserter(source));
			}
			getline(sline, industries, delim);
			getline(sline, countryOfCitizenship, delim);
			getline(sline, organization, delim);
			if (organization[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				toBeSkipped.insert(toBeSkipped.begin(), organization.begin() + 1, organization.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				organization.clear();
				copy(toBeSkipped.begin(), toBeSkipped.end(), back_inserter(organization));
			}
			getline(sline, selfMade, delim);
			getline(sline, status, delim);
			getline(sline, gender, delim);
			getline(sline, birthDate, delim);
			getline(sline, lastName, delim);
			getline(sline, firstName, delim);
			getline(sline, title, delim);
			if (title[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				toBeSkipped.insert(toBeSkipped.begin(), title.begin() + 1, title.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				title.clear();
				copy(toBeSkipped.begin(), toBeSkipped.end(), back_inserter(title));
			}
			getline(sline, date, delim);
			getline(sline, state, delim);
			getline(sline, residenceStateRegion, delim);
			getline(sline, birthYear, delim);
			getline(sline, birthMonth, delim);
			getline(sline, birthDay, delim);
			getline(sline, cpi_country, delim);
			getline(sline, cpi_change_country, delim);

			getline(sline, toBeSkipped, delimBefore1); //тут проблема: знаки запятых внутри. оно выделено кавычками и знаком доллара, так что нужно поправить
			getline(sline, toBeSkipped, delimBefore2);
			getline(sline, *gdp_country, delimAfter);
			getline(sline, toBeSkipped, delim);
			deleteCommas(gdp_country);

			getline(sline, gross_tertiary_education_enrollment, delim);
			getline(sline, gross_primary_education_enrollment_country, delim);
			getline(sline, life_expectancy_country, delim);
			getline(sline, tax_revenue_country_country, delim);
			getline(sline, total_tax_rate_country, delim);
			getline(sline, population_country, delim);
			getline(sline, latitude_country, delim);
			getline(sline, longitude_country, delim);


			result->push_back(res);

			(*result)[i][0] = rank;
			(*result)[i][1] = finalWorth;
			(*result)[i][2] = category;
			(*result)[i][3] = personName; //под снос
			(*result)[i][4] = age;
			(*result)[i][5] = country;
			(*result)[i][6] = city; //под снос (пока, ибо мегаполисы могут порешать, но их слишком много)
			(*result)[i][7] = source; //под снос
			(*result)[i][8] = industries; //под снос - то же, что и category
			(*result)[i][9] = countryOfCitizenship;
			(*result)[i][10] = organization; //под снос
			(*result)[i][11] = selfMade;
			(*result)[i][12] = status;
			(*result)[i][13] = gender;
			(*result)[i][14] = birthDate; //под снос
			(*result)[i][15] = lastName; //под снос
			(*result)[i][16] = firstName; //под снос
			(*result)[i][17] = title; //тоже под снос теперь!!бесполезно почти
			(*result)[i][18] = date; //под снос
			(*result)[i][19] = state; //под снос 
			(*result)[i][20] = residenceStateRegion; //под снос
			(*result)[i][21] = birthYear; //под снос
			(*result)[i][22] = birthMonth; //под снос
			(*result)[i][23] = birthDay; //под снос
			(*result)[i][24] = cpi_country;
			(*result)[i][25] = cpi_change_country;
			(*result)[i][26] = *gdp_country;
			(*result)[i][27] = gross_tertiary_education_enrollment;
			(*result)[i][28] = gross_primary_education_enrollment_country;
			(*result)[i][29] = life_expectancy_country;
			(*result)[i][30] = tax_revenue_country_country;
			(*result)[i][31] = total_tax_rate_country;
			(*result)[i][32] = population_country;
			(*result)[i][33] = latitude_country; //под снос
			(*result)[i][34] = longitude_country; //под снос
			i++;
			gdp_country->clear();
			delete gdp_country;
		}
	}
	file.close();

	featureSize = (*result)[0].size();
	size = result->size();

	return;
}

void readerEncoder::deleteCommas(string* input)
{
	while (true)
	{
		auto it = find(input->begin(), input->end(), ',');
		int index = it - input->begin();
		if (index <  input->size())
		{
			auto iter = input->begin();
			advance(iter, index);
			input->replace(iter, iter + 1, "");
		}
		else
		{
			break;
		}
	}
	return;
}
void readerEncoder::deleteRow(long int num)
{
	for_each(result->begin(), result->end(), [=](auto& _n)
		{
			_n.erase(_n.begin() + num);
		});

	/*for (size_t i = 0; i < size; i++)
	{
		(*result)[i].erase((*result)[i].begin() + num);
	}*/
	return;
}
void readerEncoder::deleteLine(long int num)
{
	result->erase(result->begin() + num + 1);
	return;
}
void readerEncoder::deleteAllUseless()
{
	int numDeletes = 0;

	deleteRow(34);
	numDeletes++;

	deleteRow(33);
	numDeletes++;

	deleteRow(23);
	numDeletes++;

	deleteRow(22);
	numDeletes++;

	deleteRow(21);
	numDeletes++;

	deleteRow(20);
	numDeletes++;

	deleteRow(19);
	numDeletes++;

	deleteRow(18);
	numDeletes++;

	deleteRow(17);
	numDeletes++;

	deleteRow(16);
	numDeletes++;

	deleteRow(15);
	numDeletes++;

	deleteRow(14);
	numDeletes++;

	deleteRow(10);
	numDeletes++;

	deleteRow(9);
	numDeletes++;

	deleteRow(8);
	numDeletes++;

	deleteRow(7); 
	numDeletes++;

	//deleteRow(6);
	//numDeletes++;

	//deleteRow(5);
	//numDeletes++;

	deleteRow(3);
	numDeletes++;

	deleteRow(0); // надо ли? это ж тупо топ
	numDeletes++;

	featureSize -= numDeletes;

	return;
}
void readerEncoder::deleteUselessSamples(double threshold)
{
	size_t lineSize = (*result)[0].size();
	long int numDeletes = 0;
	for_each(result->begin(), result->end(), [&](auto const& _n) //мудрое удаление
		{
			size_t errCounter = 0; //если семпл окажется слишком пустым, удалим его
			auto it = find(_n.begin(), _n.end(), "");
			if (it != _n.end())
			{
				errCounter++;
			}
			if (errCounter >= lineSize * threshold)
			{
				(*this).deleteLine((long int)(distance(result->begin(), find(result->begin(),result->end(), _n)) - numDeletes)); //1
				numDeletes++;
				size--;
			}
		});
	return;

	/*for (size_t i = 0; i < result->size(); i++)
	{
		size_t errCounter = 0; //если семпл окажется слишком пустым, удалим его

		for (size_t j = 0; j < (*result)[i].size(); j++)
		{
			if ((*result)[i][j] == "")
			{
				errCounter++;
			}
			if (errCounter >= lineSize * threshold) //вообще это надо делать перед прогоном в матрицу даблов
			{
				deleteLine(i - numDeletes);
				numDeletes++;
				size--;
				break;
			}
		}
	}
	*/
	return;
}
void readerEncoder::constructFeatureOneHot(vector<string> uniqueKeys, int featureNum) //для малого раброса параметров
{
	int sizeUnique = uniqueKeys.size();
	//нужно будет переделать это в числовую матрицу функцией
	for_each(result->begin(), result->end(), [&](auto& _n)
		{
			auto it = find(uniqueKeys.begin(), uniqueKeys.end(), _n[featureNum]);
			int index = it - uniqueKeys.begin();
			if (index < sizeUnique)
			{
				_n.insert(next(_n.begin(), featureNum + 1), next(uniqueKeys.begin(), 1), uniqueKeys.end()); //столбец под номером featureNum остаётся на своём месте, всё после него сдвигается
				return;
			}
		});
	/*
	for (size_t i = 0; i < size; i++)
	{
		auto row = next(result->begin(), i);

		for (auto x : uniqueKeys)
		{
			if ((*result)[i][featureNum] == x) //создаём столбцы для единиц/нулей
			{
				row->insert(next(row->begin(), featureNum + 1), next(uniqueKeys.begin(), 1), uniqueKeys.end()); //столбец под номером featureNum остаётся на своём месте, всё после него сдвигается
				break;
			}
		}
	}*/
	int sum = 0;
	for_each(result->begin(), result->end(), [&](auto & _n)
		{
			
			auto it = find(_n.begin() + featureNum + 1, _n.begin() + featureNum + sizeUnique, _n[featureNum]);
			const int iter = distance(_n.begin() + featureNum + 1, it);
			fill(_n.begin() + featureNum + 1, _n.begin() + featureNum + sizeUnique, "0");
			if (iter < sizeUnique)
			{
				_n[featureNum] = "0";
				_n[iter + featureNum] = "1";

			}
			else
			{
				_n[featureNum] = "1";
				sum++;
			}
			return;
		});
	
		
/*
	for (size_t i = 0; i < size; i++)
	{
		auto row = next(result->begin(), i);
		int countZero = 0;
		for (size_t k = featureNum + 1; k < featureNum + sizeUnique; k++) //поначалу не трогаем опорный столбец, заполняем 1 и 0 всё, кроме него
		{
			if ((*result)[i][featureNum] == (*result)[i][k])
			{
				(*result)[i][k] = "1";
			}
			else
			{
				(*result)[i][k] = "0";
				countZero++; //если везде будут нули, значит, в опорном столбце должна быть единица
			}
		}
		if (countZero == sizeUnique - 1) //чистим опорный столбец
		{
			(*result)[i][featureNum] = "1";
		}
		else
		{
			(*result)[i][featureNum] = "0";
		}

	}
	*/
	return;
}
void readerEncoder::constructFeatureSimple(vector<string> uniqueKeys, int featureNum)
{
	for_each(result->begin(), result->end(), [&](auto& _n)
		{
			if (_n[featureNum] == uniqueKeys[1])
			{
				_n[featureNum] = "1.0";
			}
			else
			{
				_n[featureNum] = "0.0";
			}
			return;
		});

	/*for (size_t i = 0; i < size; i++)
	{
		auto row = next(result->begin(), i);

			if ((*result)[i][featureNum] == uniqueKeys[1]) //создаём столбцы для единиц/нулей
			{
				(*result)[i][featureNum] = "1.0";
			}
			else
			{
				(*result)[i][featureNum] = "0.0";
			}
		}
		*/
	return;
}

void readerEncoder::constructFeatureTarget(vector<string> uniqueKeys, int numFeature)
{
	vector<double> summs(uniqueKeys.size(), 0.0); //вектор суммарных капиталов для буржуев, обладающих i-м видом признака
	double summAll = 0.0;

	for_each(result->begin(), result->end(), [&](auto const& _n)
		{
			summAll += stod(_n[0]);  //нужно будет для нормировки на эту сумму - сумму всех капиталов
			for_each(uniqueKeys.begin(), uniqueKeys.end(), [&](auto const& _m)
				{
					if (_n[numFeature] == _m)
					{
						summs[find(uniqueKeys.begin(), uniqueKeys.end(), _m) - uniqueKeys.begin()] += stod(_n[0]);
					}
				});

		});

	/*for (size_t i = 0; i < result->size(); i++)
	{
		summAll += stod((*result)[i][0]); //нужно будет для нормировки на эту сумму - сумму всех капиталов
		for (size_t k = 0; k < uniqueKeys.size(); k++)
		{
			if ((*result)[i][numFeature] == uniqueKeys[k])
			{
				summs[k] += stod((*result)[i][0]); //добавляем в сумму капитал буржуя обладающего k-м видом признака
			}

		}
	}
	*/

	transform(summs.begin(), summs.end(), summs.begin(), bind2nd(divides<double>(), summAll)); //делим всё на норму

	/*for (size_t i = 0; i < uniqueKeys.size(); i++)
	{
		summs[i] /= summAll; //нормировка
	}*/
	
	for_each(result->begin(), result->end(), [&](auto& _n)
		{
			auto it = find(uniqueKeys.begin(), uniqueKeys.end(), _n[numFeature]);
			if (it != uniqueKeys.end())
			{
				int iter = it - uniqueKeys.begin();
				_n[numFeature] = to_string(summs[iter]);
				return;
			}
		});
	/*
	for (size_t i = 0; i < size; i++)
	{
		for (size_t k = 0; k < uniqueKeys.size(); k++)
		{
			if ((*result)[i][numFeature] == uniqueKeys[k])
			{
				(*result)[i][numFeature] = to_string(summs[k]); //заменяем вид признака на его частоту встречи среди других видов признака и надеемся, что таких же частот не будет
			} //а даже если и будет, то это всё хороший способ "отрезать" лишние данные, т.к. предполагается использовать этот метод для countries/cities, а у них повторы частот ожидаются только для малых городов/стран - т.е. для "портрета" буржуя будет достаточно сказать, что он из "малого" города/страны с кодом w_i
		}
	}*/
	return;
}
void readerEncoder::turnIntoNumbers(vector<vector<double>>* output)
{
	//следующий этап работы
	deleteAllUseless();
	deleteUselessSamples();
	vector<vector<string>> featuresExamples;
	vector<string> featuresExamplesRow;

	copy(result->begin(), result->end(), back_inserter(featuresExamples)); //копируем матрицу, чтоб по ней сверяться

	/*for (size_t i = 0; i < result->size(); i++)
	{
		featuresExamplesRow.clear();
		featuresExamplesRow.insert(featuresExamplesRow.begin(), (*result)[i].begin(), (*result)[i].end()); //копируем сюда строку, чтобы по ней сверяться и вместе с этим менять ширину изначальной матрицы
		featuresExamples.push_back(featuresExamplesRow);

	}*/

	size_t uniqueIterator = 0;
	for (size_t i = 0; i < featureSize; i++) //ну, тут мне уже совсем лень пользоваться алгоритмами
	{
		if ((i == 3) || (i == 4)) //для стран и городов применим Target кодирование
		{
			vector<string>* uniqueFeatures = new vector<string>;
			checkUniqueFeatures(uniqueFeatures, uniqueIterator);
			constructFeatureTarget(*uniqueFeatures, uniqueIterator);
			
			uniqueIterator++;
			delete uniqueFeatures;
		}
		else if (not check(featuresExamples[0][i][0])) //если i-й признак изначально не числовой - применяем OneHot кодирование
		{
			vector<string>* uniqueFeatures = new vector<string>;
			checkUniqueFeatures(uniqueFeatures, uniqueIterator); //для должностей надо что-то ещё
			uniqueLabelSizes.push_back(uniqueFeatures->size());
			int uniqueSize = uniqueFeatures->size();
			if (uniqueSize == 2)
			{
				constructFeatureSimple(*uniqueFeatures, uniqueIterator);
				uniqueIterator += 1;
			}
			else
			{  
				constructFeatureOneHot(*uniqueFeatures, uniqueIterator);

				uniqueIterator += uniqueSize;
			}
			
			delete uniqueFeatures;
		}
		else
		{
			uniqueIterator++;
		}
	}

	featureSize = (*result)[0].size();

	for_each(result->begin(), result->end(), [&](auto const& _n)
		{
			vector<double> outputRow;
			transform(_n.begin(), _n.end(), back_inserter(outputRow), [&](string const _m)
				{if (_m != "") { return stod(_m); }
				else return 0.0; });
			output->push_back(outputRow);
		});

	/*for (size_t i = 0; i < size; i++)
	{
		vector<double> outputRow;
		for (size_t j = 0; j < featureSize; j++)
		{
			if ((*result)[i][j] == "")
			{
				outputRow.push_back(0);
			}
			else
			{
				outputRow.push_back(stod((*result)[i][j]));
			}
		}
		output->push_back(outputRow);
	}
	*/
	return;
}

vector<string> readerEncoder::saveNames()
{
	vector<string> res(size, "");
	for_each(res.begin(), res.end(), [&](auto & _n)
		{
			size_t i = distance(res.begin(), find(res.begin(), res.end(), _n));
			_n = (*result)[i][3];
		
		});
	return res;
}

vector<size_t> readerEncoder::getCategoryFeturesSize()
{
	return uniqueLabelSizes;
}