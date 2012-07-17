#include <stdio.h>
// For arrayToCSV:
#include <iostream>
#include <fstream>

void convertCSV(int count);
void convertArray(char* FileName);

void convertArray(char* FileName) {
	/* Show the data using cbFileRead()
	Parameters:
      FileName      :the filename containing the data
      NumPoints     :the number of data values to read from the file
      FirstPoint    :index of the first data value to read
      DataBuffer[]   :array to read data into  */

	WORD DataBuffer[10];
	int ULStat;
	int i, j;

	long NumPoints = 10;     /* number of points to buffer at a given time */
	long FirstPoint = 0;     /* start at the first point in the log file*/

	i = 0;
	// FIXME: When we want to go through all values, i should increment up to TotalCount - 1
	// FIXME: This is broken
	for (i; i <= 10; i++) {
		// Increment by 10 and load new values into the buffer
		ULStat = cbFileRead (FileName, FirstPoint, &NumPoints, DataBuffer);
		FirstPoint = FirstPoint + 10;
		for (j = 0; j < (int)NumPoints ; j++)
			printf ("     %2u) %4u  \n", i - 10 + j, DataBuffer[j]);
			// TODO: Write the array data to CSV rather than simply printing it
			// Call arrayToCSV() here
	}

	// Free the memory buffer
	ULStat = cbWinBufFree(DataBuffer);
}

// FIXME: This is template code that I have not edited
// We might actually want this to take an int instead of 
//		an array of ints
void arrayToCSV (int* data) {

	int arr1[] = {1,2,3};
	int arr2[] = {3,5,6};

	ofstream myfile;
	myfile.open ("example.txt");

	for (int i=0; i<3; i++)
	{
	myfile << arr1[i] << "," << arr2[i] << endl;
	}
	myfile.close();
}