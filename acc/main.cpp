// accelerometer.cpp
// Scan data from a given device at a given frequency across multiple channels
// Bryan Orr and Alex Dunn at Argonne National Lab Photon Source (APS)

/* Include files */
#include <stdio.h>
#include <time.h>
// MCC DAQ library
#include "cbw.h"
#include <iostream>

using namespace std;

// Declarations
int run (int BoardNum, short LowChan, short HighChan, long Rate, long Count, int Range);

int main (int argc, char *argv[])
    {
		// Check that the right number of arguments is given.
		//	Otherwise, print help and exit.
		if (argc != 8) {
			cout << "acc.exe [boardnum] [low-chan] [high-chan] "
				 << "[scan-rate] [minutes] [num-runs] [range]\n"
				  << endl;
			cout << "Note that the log file will be saved in the current directory.\n"
				 << "This is not necessarily the same as the location of acc.exe."
				 << endl;
			cout << "1. [boardnum] is the board's number assigned by InstaCal in the file\n"
				 << "C:\\ProgramData\\Measurement Computing\\CB.CFG.\n"
				 << "This number may be 0-99."
				 << endl;
			cout << "2. [low-chan] is the number of the lowest channel to be scanned.\n"
				 << "Channel numbering starts at 0."
				 << endl;
			cout << "3. [high-chan] is the number of the highest channel to be scanned.\n"
				 << "Only one channel will be scanned if LOW-CHAN == HIGH-CHAN."
				 << endl;
			cout << "4. [scan-rate] is samples per second per channel.  Max is 100000."
				 << endl;
			cout << "5. [minutes] is the total number of minutes you want to collect data.\n"
				 << "As a reference, 99 minutes is the maximum time for 3 channels.\n"
				 << "This must be an integer."
				 << endl;
			cout << "6. [num-runs] is the number of times to repeat the data collection.\n"
				 << "Zero repeats data collection indefinitely. Negative values are not accepted."
				 << endl;
			cout << "7. [range] is the voltage range that is scanned for data.\n"
				 << "The device scans across +/-[range] Volts. Choose 1, 2, 5, or 10."
				 << endl;
			return 0;
		}

    /* Variable Declarations */
    int BoardNum = atoi(argv[1]);
    int ULStat = 0;
    short LowChan = atoi(argv[2]);
    short HighChan = atoi(argv[3]);
    long Rate = atoi(argv[4]);
    float RevLevel = (float)CURRENTREVNUM;
	int minutes = atoi(argv[5]);
	int runs = atoi(argv[6]);
	int Gain;

	// Set the range
	switch (atoi(argv[7])) {
		case 1:
			Gain = BIP1VOLTS;
			break;
		case 2:
			Gain = BIP2VOLTS;
			break;
		case 5:
			Gain = BIP5VOLTS;
			break;
		case 10:
			Gain = BIP10VOLTS;
			break;
		default:
			printf("Invalid range parameter.\n");
			return 0;
	}

	// Check that the requested number of minutes is valid
	if (Rate*(HighChan - LowChan + 1)*60*minutes > 785862000 || minutes < 1) {
			cout << "Your desired collection time is too great or less than 1.\n"
				 << "Lower your minutes argument and make sure it's an integer."
				 << endl;
			return 0;
		}

	// calcualte total number of data points to collect
	long NumPoints = minutes*Rate*(HighChan - LowChan + 1)*60;

  /* Declare UL Revision Level */
	// This function must be called first for MCC DAQ's Universal Library
   ULStat = cbDeclareRevision(&RevLevel);

    /* Initiate error handling
    Parameters:
        PRINTALL :all warnings and errors encountered will be printed
        DONTSTOP :program will continue even if error occurs.
                     Note that STOPALL and STOPFATAL are only effective in 
                     Windows applications, not Console applications. 
   */
    cbErrHandling (PRINTALL, DONTSTOP);

	// Iterate through the runs
	int x = 0;
	if (runs == 0) {
		while(x == 0) run(BoardNum, LowChan, HighChan, Rate, NumPoints, Gain);
	}
	else if (runs > 0) {
		for (int i = 1; i <= runs; i++) run(BoardNum, LowChan, HighChan, Rate, NumPoints, Gain);
	}
	else {
		printf("Invalid number of runs.\n");
	}

	// Call function to read the binary file into a CSV
	// convertArray(FileName);

	// call function to convert the binary to CSV format
	//convertCSV(TotalCount);

	return 0;
}

// Call the functions to record data
int run (int BoardNum, short LowChan, short HighChan, long Rate, long Count, int Range) {
	int ULStat = 0;
	char FileName[80];
	int Options;
	long PreTrigCount;
	long TotalCount;

	// binary log filename
	time_t now = time(NULL);
	strftime(FileName,79,"LOG_%d%b%Y_%Hh%Mm%Ss.DAT", localtime(&now));
	Options = TIMED + NODTCONNECT;

	printf ("Data will now be taken with the following parameters:\n");
    printf ("Rate     = %ld Hz\n", Rate);
    printf ("LowChan  = %u\n", LowChan);
    printf ("HighChan = %u\n", HighChan);
    printf ("Options  = %u\n", Options);
    printf ("Gain     = %u\n", Range);
    printf ("FileName = %s\n", FileName);
    printf ("Count    = %ld\n\n", Count);

    /* Collect the values with cbFileAInScan()
      Parameters:
        BoardNum   :the number used by CB.CFG to describe this board
          LowChan    :first A/D channel of the scan
          HighChan   :last A/D channel of the scan
          Count      :the total number of A/D samples to collect
          Rate       :sample rate in samples per second
          Gain       :the gain for the board
          FileName   :the filename for the collected data values
          Options    :data collection options  */
    if (ULStat == 0) {
		ULStat = cbFileAInScan (BoardNum, LowChan, HighChan, Count, &Rate, Range, FileName, Options);
	}

	// Get info from the log file on disk
    if (ULStat == 0) {
		ULStat = cbFileGetInfo (FileName, &LowChan, &HighChan, &PreTrigCount, &TotalCount, &Rate, &Range);
	}

	if (ULStat == 0) {
		printf ("The binary log file %s now contains the following: \n", FileName);
		printf ("Rate     = %ld Hz\n", Rate);
		printf ("LowChan  = %u\n", LowChan);
		printf ("HighChan = %u\n", HighChan);
		printf ("Options  = %u\n", Options);
		printf ("Gain     = %u\n", Range);
		printf ("FileName = %s\n", FileName);
		printf ("Count    = %ld\n", TotalCount);
		printf("Successfully completed.\n\n");
	}
	else cout << "Error Status $d.  See MCC DAQ's Universal Library "
			  << "documentation for error code information\n"
			  << endl;

	return 0;
}