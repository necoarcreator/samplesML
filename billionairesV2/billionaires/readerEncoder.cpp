#include "readerEncoder.h"
#include <stdio.h>
#include <string> 
#include <sstream>
#include <iostream> //�������� ����
#include <fstream> //�������� ����
#include <array> //��� ������������� ������� ����� 
#include<algorithm> //��� ����������
#include <stdlib.h> //��� exit
#include <vector>
#include <math.h>

using namespace std;

readerEncoder::readerEncoder()
{
	result = new vector<vector<string>>;
	size = 0;
	featureSize = 0;
}

readerEncoder::~readerEncoder()
{
	result->clear();
	delete result;
	size = 0;
	featureSize = 0;
	result = new vector<vector<string>>;
}

void readerEncoder::writeFile()
{
	ofstream out("tp.txt");
	if (out.is_open())
	{
		for (size_t n = 0; n < size; n++)
		{
			for (size_t i = 0; i < featureSize; i++) {
				out << (*result)[i][n];
			}
			out << "\n";

		}
		cout << "writing ended successefully!\n";
	}
	else
	{
		cerr << "Error while reading file!\n";
	}
	out.close();
}
bool readerEncoder::checkIfNotVoid(string input)
{
	if (input[0])
	{
		return true;
	}
	return false;
}
bool readerEncoder::check(char symb)
{
	if (((static_cast<int>(symb) > 47) and (static_cast<int>(symb) < 58))
		|| (symb == ' ') || (symb == '\0'))
	{
		return true;
	}
	return false;
}
void readerEncoder::checkUniqueFeatures(vector<string>* uniqueSphere, int featureNum)
{
	uniqueSphere->push_back((*result)[0][featureNum]);
	for (size_t i = 0; i < size; i++)
	{
		if (i == 0) {

		}
		bool isUnique = true;
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
	}

	return;
}
void readerEncoder::readParce(int numFeatures)
{
	vector<string> res;
	for (int j = 0; j < numFeatures; j++)
	{
		res.push_back("0");
	}
	ifstream file("billionaires.csv");
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
			getline(sline, personName, delim); //���� ���� �������� �� ������
			if (personName[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				auto iter = personName.begin();
				advance(iter, 1); //�������� �������
				toBeSkipped.insert(toBeSkipped.begin(), iter, personName.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				personName.clear();
				personName.insert(personName.begin(), toBeSkipped.begin(), toBeSkipped.end());
			}
			getline(sline, age, delim);
			getline(sline, country, delim);
			if (country[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				auto iter = country.begin();
				advance(iter, 1); //�������� �������
				toBeSkipped.insert(toBeSkipped.begin(), iter, country.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				country.clear();
				country.insert(country.begin(), toBeSkipped.begin(), toBeSkipped.end());
			}
			getline(sline, city, delim);
			if (city[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				auto iter = city.begin();
				advance(iter, 1); //�������� �������
				toBeSkipped.insert(toBeSkipped.begin(), iter, city.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				city.clear();
				city.insert(city.begin(), toBeSkipped.begin(), toBeSkipped.end());
			}
			getline(sline, source, delim);
			if (source[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				auto iter = source.begin();
				advance(iter, 1); //�������� �������
				toBeSkipped.insert(toBeSkipped.begin(), iter, source.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				source.clear();
				source.insert(source.begin(), toBeSkipped.begin(), toBeSkipped.end());
			}
			getline(sline, industries, delim);
			getline(sline, countryOfCitizenship, delim);
			getline(sline, organization, delim);
			if (organization[0] == '\"')
			{
				string afterComma;
				getline(sline, afterComma, delimAfter);
				getline(sline, toBeSkipped, delim);
				auto iter = organization.begin();
				advance(iter, 1); //�������� �������
				toBeSkipped.insert(toBeSkipped.begin(), iter, organization.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				organization.clear();
				organization.insert(organization.begin(), toBeSkipped.begin(), toBeSkipped.end());
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
				auto iter = title.begin();
				advance(iter, 1); //�������� �������
				toBeSkipped.insert(toBeSkipped.begin(), iter, title.end());
				toBeSkipped.insert(toBeSkipped.end(), afterComma.begin(), afterComma.end());
				title.clear();
				title.insert(title.begin(), toBeSkipped.begin(), toBeSkipped.end());
			}
			getline(sline, date, delim);
			getline(sline, state, delim);
			getline(sline, residenceStateRegion, delim);
			getline(sline, birthYear, delim);
			getline(sline, birthMonth, delim);
			getline(sline, birthDay, delim);
			getline(sline, cpi_country, delim);
			getline(sline, cpi_change_country, delim);

			getline(sline, toBeSkipped, delimBefore1); //��� ��������: ����� ������� ������. ��� �������� ��������� � ������ �������, ��� ��� ����� ���������
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
			(*result)[i][3] = personName; //��� ����
			(*result)[i][4] = age;
			(*result)[i][5] = country;
			(*result)[i][6] = city; //��� ���� (����, ��� ���������� ����� ��������, �� �� ������� �����)
			(*result)[i][7] = source; //��� ����
			(*result)[i][8] = industries; //��� ���� - �� ��, ��� � category
			(*result)[i][9] = countryOfCitizenship;
			(*result)[i][10] = organization; //��� ����
			(*result)[i][11] = selfMade;
			(*result)[i][12] = status;
			(*result)[i][13] = gender;
			(*result)[i][14] = birthDate; //��� ����
			(*result)[i][15] = lastName; //��� ����
			(*result)[i][16] = firstName; //��� ����
			(*result)[i][17] = title;
			(*result)[i][18] = date; //��� ����
			(*result)[i][19] = state; //��� ���� 
			(*result)[i][20] = residenceStateRegion; //��� ����
			(*result)[i][21] = birthYear; //��� ����
			(*result)[i][22] = birthMonth; //��� ����
			(*result)[i][23] = birthDay; //��� ����
			(*result)[i][24] = cpi_country;
			(*result)[i][25] = cpi_change_country;
			(*result)[i][26] = *gdp_country;
			(*result)[i][27] = gross_tertiary_education_enrollment;
			(*result)[i][28] = gross_primary_education_enrollment_country;
			(*result)[i][29] = life_expectancy_country;
			(*result)[i][30] = tax_revenue_country_country;
			(*result)[i][31] = total_tax_rate_country;
			(*result)[i][32] = population_country;
			(*result)[i][33] = latitude_country; //��� ����
			(*result)[i][34] = longitude_country; //��� ����
			i++;
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
		int index = input->find(',');
		if (index < input->size())
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
void readerEncoder::deleteRow(int num)
{
	
	for (size_t i = 0; i < size; i++)
	{
		(*result)[i].erase((*result)[i].begin() + num);
	}
	return;
}
void readerEncoder::deleteLine(int num)
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

	deleteRow(0); // ���� ��? ��� � ���� ���
	numDeletes++;

	featureSize -= numDeletes;

	return;
}
void readerEncoder::deleteUselessSamples(double threshold)
{
	size_t lineSize = (*result)[0].size();
	int numDeletes = 0;
	for (size_t i = 0; i < result->size(); i++)
	{
		size_t errCounter = 0; //���� ����� �������� ������� ������, ������ ���

		for (size_t j = 0; j < (*result)[i].size(); j++)
		{
			if ((*result)[i][j] == "")
			{
				errCounter++;
			}
			if (errCounter >= lineSize * threshold) //������ ��� ���� ������ ����� �������� � ������� ������
			{
				deleteLine(i - numDeletes);
				numDeletes++;
				size--;
				break;
			}
		}
	}
	return;
}
void readerEncoder::constructFeatureOneHot(vector<string> uniqueKeys, int featureNum) //��� ������ ������� ����������
{
	int sizeUnique = uniqueKeys.size();
	//����� ����� ���������� ��� � �������� ������� ��������
	for (size_t i = 0; i < size; i++)
	{
		auto row = next(result->begin(), i);

		for (auto x : uniqueKeys)
		{
			if ((*result)[i][featureNum] == x) //������ ������� ��� ������/�����
			{
				row->insert(next(row->begin(), featureNum + 1), next(uniqueKeys.begin(), 1), uniqueKeys.end()); //������� ��� ������� featureNum ������� �� ���� �����, �� ����� ���� ����������
				break;
			}
		}
	}

	bool firstIteration = true;
	for (size_t i = 0; i < size; i++)
	{
		auto row = next(result->begin(), i);
		int countZero = 0;
		for (size_t k = featureNum + 1; k < featureNum + sizeUnique; k++) //�������� �� ������� ������� �������, ��������� 1 � 0 ��, ����� ����
		{
			if ((*result)[i][featureNum] == (*result)[i][k])
			{
				(*result)[i][k] = "1";
			}
			else
			{
				(*result)[i][k] = "0";
				countZero++; //���� ����� ����� ����, ������, � ������� ������� ������ ���� �������
			}
		}
		if (countZero == sizeUnique - 1) //������ ������� �������
		{
			(*result)[i][featureNum] = "1";
		}
		else
		{
			(*result)[i][featureNum] = "0";
		}

	}
	return;
}
void readerEncoder::constructFeatureTarget(vector<string> uniqueKeys, int numFeature)
{
	vector<double> summs(uniqueKeys.size(), 0.0); //������ ��������� ��������� ��� �������, ���������� i-� ����� ��������
	double summAll = 0.0;
	for (size_t i = 0; i < result->size(); i++)
	{
		summAll += stod((*result)[i][1]); //����� ����� ��� ���������� �� ��� ����� - ����� ���� ���������
		for (size_t k = 0; k < uniqueKeys.size(); k++)
		{
			if ((*result)[i][numFeature] == uniqueKeys[k])
			{
				summs[k] += stod((*result)[i][1]); //��������� � ����� ������� ������ ����������� k-� ����� ��������
			}

		}
	}

	for (size_t i = 0; i < uniqueKeys.size(); i++)
	{
		summs[i] /= summAll; //����������
	}

	for (size_t i = 0; i < size; i++)
	{
		for (size_t k = 0; k < uniqueKeys.size(); k++)
		{
			if ((*result)[i][numFeature] == uniqueKeys[k])
			{
				(*result)[i][numFeature] = to_string(summs[k]); //�������� ��� �������� �� ��� ������� ������� ����� ������ ����� �������� � ��������, ��� ����� �� ������ �� �����
			} //� ���� ���� � �����, �� ��� �� ������� ������ "��������" ������ ������, �.�. �������������� ������������ ���� ����� ��� countries/cities, � � ��� ������� ������ ��������� ������ ��� ����� �������/����� - �.�. ��� "��������" ������ ����� ���������� �������, ��� �� �� "������" ������/������ � ����� w_i
		}
	}
	return;
}
void readerEncoder::turnIntoNumbers(vector<vector<double>>* output)
{
	//��������� ���� ������
	deleteAllUseless();
	deleteUselessSamples();
	vector<vector<string>> featuresExamples;
	vector<string> featuresExamplesRow;

	for (size_t i = 0; i < result->size(); i++)
	{
		featuresExamplesRow.clear();
		featuresExamplesRow.insert(featuresExamplesRow.begin(), (*result)[i].begin(), (*result)[i].end()); //�������� ���� ������, ����� �� ��� ��������� � ������ � ���� ������ ������ ����������� �������
		featuresExamples.push_back(featuresExamplesRow);

	}

	int uniqueIterator = 0;
	for (size_t i = 0; i < featureSize; i++)
	{
		if ((i == 3) || (i == 4)) //��� ����� � ������� �������� Target �����������
		{
			vector<string>* uniqueFeatures = new vector<string>;
			checkUniqueFeatures(uniqueFeatures, uniqueIterator);
			constructFeatureTarget(*uniqueFeatures, uniqueIterator);
			uniqueIterator++;
			delete uniqueFeatures;
		}
		else if (not check(featuresExamples[0][i][0])) //���� i-� ������� ���������� �� �������� - ��������� OneHot �����������
		{
			vector<string>* uniqueFeatures = new vector<string>;
			checkUniqueFeatures(uniqueFeatures, uniqueIterator); //��� ���������� ���� ���-�� ���
			int uniqueSize = uniqueFeatures->size();
			constructFeatureOneHot(*uniqueFeatures, uniqueIterator);
			uniqueIterator += uniqueSize;
			delete uniqueFeatures;
		}
		else
		{
			uniqueIterator++;
		}
	}

	featureSize = (*result)[0].size();

	for (size_t i = 0; i < size; i++)
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
	return;
}
