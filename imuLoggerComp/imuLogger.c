/*
 * imuLogger 
 * logs a timestamp with  data onto imu.txt every 1 seconds
 *
 * Listoe Dev
 * 18/12/09
 */
 
#include "legato.h"
#include "interfaces.h"
#include "le_tty.h"
#include "string.h"
#include "unistd.h"
#include "stdio.h"
#include "time.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "accelerometer.h"

#define IMU_SAMPLE_INTERVAL_IN_MILLISECONDS (10)



//--------------------------------------------------------------------------------------------------
/*
 * gnssTmer runs every 10 seconds when called as gnssTmerRef handler
 */
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
/**
 * Convenience function to get current time as uint64_t.
 *
 * @return
 *      Current time as a uint64_t
 */
//--------------------------------------------------------------------------------------------------
static uint64_t GetCurrentTimestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t utcMilliSec = (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
    return utcMilliSec;
}


static void imuLogTimer(le_timer_Ref_t imuLogTimerRef)
{
    //char timestamp[80] = {};
	char timestamp[80] = {0};
	//char buffer[100] = {0};
	// Atomic write example, File Descriptor case.
	//char filenamebuff[255] = {0};
	time_t     now;
    struct tm  ts;
    
    // Get current time
    time(&now);

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    
    double xAcc;
    double yAcc;
    double zAcc;
    double x;
    double y;
    double z;
    char sec[3] = {0};
    strftime(sec, sizeof(sec), "%S", &ts);
    
    uint64_t tnow = GetCurrentTimestamp();
    le_result_t accRes = mangOH_ReadAccelerometer(&xAcc, &yAcc, &zAcc);
    le_result_t gyroRes = mangOH_ReadGyro(&x, &y, &z);
	//sprintf(filenamebuff,"sdcard/gnssLog.txt", timestamp);
	//sprintf(buffer, "sdcard/imuLog_%s.txt", sec );
	//FILE* fd = fopen (buffer, "a");
	FILE* fd = fopen ("sdcard/imuLog.txt", "a");
	//GetCurrentTimestamp(timestamp);
	
	if (fd == NULL)
	{
		// Print error message and exit.
		LE_INFO("file could not be opened");
	}
	else{
		if (accRes == LE_OK && gyroRes == LE_OK)
		{
			// Write something in fd
		
		fprintf(fd, "%lld,%f,%f,%f,%f,%f,%f\n", tnow, xAcc, yAcc, zAcc, x, y, z);
		}else{
			fprintf(fd, "%s %s", timestamp, " imuLog no data\n");
		}
		

		 
		// Now write this string to fd
		if (fclose(fd) == 0)
		{
			// Print success message
			//LE_INFO("Data successfuly written");
		}
		else
		{
			LE_INFO("Error closing file");
		}
	}
}

//--------------------------------------------------------------------------------------------------
/*
 * Main program starts here
 */
//--------------------------------------------------------------------------------------------------

COMPONENT_INIT
{
	LE_INFO("imuLogTemp application has started");
	
	le_timer_Ref_t imuLogTimerRef = le_timer_Create("imuLog Timer");
    le_timer_SetMsInterval(imuLogTimerRef, IMU_SAMPLE_INTERVAL_IN_MILLISECONDS);
    le_timer_SetRepeat(imuLogTimerRef, 0);

    le_timer_SetHandler(imuLogTimerRef,imuLogTimer);
    le_timer_Start(imuLogTimerRef);
	
}
