
#include <iostream> 
#include "KohonenMap.h"

using namespace std;
int main()
{
	
	KohonenMap A(10, true);

	A.training(0, 1e-7, 200, 0.5);
	A.printClusterization();
	A.writeToFile();
	
	
	KohonenMap B("KohonenMap.txt", 10);
	B.readData("billionaires.csv", true);
	B.clusterize(1, 0, false);
	
	
	return 0;

}