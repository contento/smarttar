#include <iostream.h>
#include <math.h>

double g_Ceil(double value, double ceilVal)
{
	if (ceilVal == 0.0)
		return value;

	if (ceilVal == 1.0)
		return ceil(value);

	if (ceilVal == 0.5)
	{
		double remain = value-floor(value);
		if (remain == 0.0)
			return value;

		if (remain <= 0.5)
			return floor(value) + 0.5;

		return ceil(value);
	}

	return 0.0; // still here, something is wrong
}

void main()
{
	double ceilVal = 0.5;
	double value;
	double ceiled;

	do
	{
		cout << "Value (0 to quit): ";
		cin >> value;

		ceiled = g_Ceil(value, ceilVal);

		cout
			<< "\tCeil " << value
			<< " with " << ceilVal << " = " << ceiled << endl;
	}
	while (value != 0);
}
