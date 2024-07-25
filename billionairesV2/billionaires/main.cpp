
#include <iostream> 
#include "KohonenMap.h"
using namespace std;

int main()
{

	KohonenMap A(20);

	A.training(0, 0.6);

	A.writeToFile();
	//A.normalize();

	/*vector<string>* featuresExamples = new vector<string>;
	for (int i = 2; i < 35; i++)
	{
		checkUniqueFeatures(*puki, featuresExamples, i);
		for (int k = 0; k < featuresExamples->size(); k++)
		{
			cout << (*featuresExamples)[k] << endl;
		}
		cout << "_____________________" << endl;
		cout << endl;
		featuresExamples->clear();
	}
	vector<double>* uk = new vector<double>;
	for (int i = 0; i < 10; i++)
	{
		uk->push_back(pow(10, i));
	}
	boxCoxTransform(uk, 0.01);
	for (int i = 0; i < 10; i++)
	{
		cout << (*uk)[i] << endl;
	}
	*/
	return 0;

}