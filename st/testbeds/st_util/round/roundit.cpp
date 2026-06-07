#include <iostream.h>
#include <math.h>

double round(double value, double roundVal)
{
  return floor((value+roundVal/2)/roundVal)*roundVal;
}

void main()
{
	double tariff   = 0.01;
	double roundVal = 0.05;

	double tax      = 0.15;

	double minutes = 0;

	double value;
	double rounded;

	do
	{
		cout << "Minutes (0 to quit): ";
		cin >> minutes;

		value = (minutes * tariff)*(1.0+tax);
		rounded = round(value, roundVal);

		cout
			<< "\tRound " << value
			<< " [" << minutes << "*" << tariff << "*(1+" <<  tax << ")" << "]"
			<< " with " << roundVal << " = " << rounded << endl;
	}
	while (minutes != 0);
}
