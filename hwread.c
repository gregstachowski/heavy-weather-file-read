#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* hwread.c -- Read history.dat binary files from Heavy Weather weatherstation.
 *
 * Copyright (C) G.S.Stachowski, 2003.08.29
 *
 * Data file format:
 *
 * Records are 36 bytes long. Data is written in the same units specified in
 * Setup within Heavy Weather, in a mixture of 4-byte floats and integers.
 * Dewpoint and windchill are not stored in the binary file, and are calculated
 * on the fly for the text file.
 *
 * Bytes:
 *  0 -  3 : These bytes probably specify the units used in the file.
 *  4 -  7 : Date, unsigned 32-bit long integer, in seconds,
 *           (probably) from 1.1.190[01]. 
 *  8 - 11 : Pressure, 32-bit float.
 * 12 - 15 : Wind speed, 32-bit float.
 * 16      : Wind direction, 8-bit integer (unsigned char), compass points 
 *           clockwise from N (0=N, 1=NNE, 2=NE, 3=ENE, ... 14=NW, 15=NNW).
 * 17 - 19 : Blank (all zeros).
 * 20 - 23 : Total rainfall, 32-bit float.
 * 24 - 27 : Indoor temperature, 32-bit float.
 * 28 - 31 : Outdoor temperature, 32-bit float.
 * 32      : Indoor humidity, 8-bit unsigned integer.
 * 33      : Blank (reserved?).
 * 34      : Outdoor humidity, 8-bit unsigned integer.
 * 35      : Blank (reserved?).
 *
 *"No data" markers:
 * Note: these markers are empirically deduced from the data file and may
 * not be completely correct.
 *
 * Wind speed (also affects direction): 00 00 4C 42
 * Outdoor temperature: 52 38 A2 42 (= 81.1 deg. C)
 * Outdoor humidity: 6E (> 100%)
 * 
 *  -  -  -  -  -  -  - -
 * TBDW:
 * --"no data" markers for all sensors
 * -- first four bytes - automatic unit interpretation?
 * -- bugs in temperature, pressure and time code? :
 * Record 413: Indoor temp i s 23.5, read as 0.0.
 * Record   3: Date in txt file is sensible (= rec. 2), read as 244.
 */

#define RECORD_LENGTH_BYTES 36

void printf_wind(short wind_dir) {
	switch (wind_dir) {
		case  0: printf("N "); break;
		case  1: printf("NNE"); break;
		case  2: printf("NE"); break;
		case  3: printf("ENE"); break;
		case  4: printf("E "); break;
		case  5: printf("ESE"); break;
		case  6: printf("SE"); break;
		case  7: printf("SSE"); break;
		case  8: printf("S "); break;
		case  9: printf("SSW"); break;
		case 10: printf("SW"); break;
		case 11: printf("WSW"); break;
		case 12: printf("W "); break;
		case 13: printf("WNW"); break;
		case 14: printf("NW"); break;
		case 15: printf("NNW"); break;
	}
	return;
}

void sprintf_wind(char *outstr, short wind_dir) {
	switch (wind_dir) {
		case  0: sprintf(outstr,"%s\tN ", outstr); break;
		case  1: sprintf(outstr,"%s\tNNE", outstr); break;
		case  2: sprintf(outstr,"%s\tNE", outstr); break;
		case  3: sprintf(outstr,"%s\tENE", outstr); break;
		case  4: sprintf(outstr,"%s\tE ", outstr); break;
		case  5: sprintf(outstr,"%s\tESE", outstr); break;
		case  6: sprintf(outstr,"%s\tSE", outstr); break;
		case  7: sprintf(outstr,"%s\tSSE", outstr); break;
		case  8: sprintf(outstr,"%s\tS ", outstr); break;
		case  9: sprintf(outstr,"%s\tSSW", outstr); break;
		case 10: sprintf(outstr,"%s\tSW", outstr); break;
		case 11: sprintf(outstr,"%s\tWSW", outstr); break;
		case 12: sprintf(outstr,"%s\tW ", outstr); break;
		case 13: sprintf(outstr,"%s\tWNW", outstr); break;
		case 14: sprintf(outstr,"%s\tNW", outstr); break;
		case 15: sprintf(outstr,"%s\tNNW", outstr); break;
	}
        return;
}


int main (int argc, char *argv[]) {
	long   n; /* record number */
	long   i;
	int    c;

	short  wind_dir;
	float  pressure, in_temp, out_temp, dewpoint, windchill;
	float  wind_speed;
	float  rain_total;

        unsigned long   date;
	unsigned short  in_humidity, out_humidity;

	struct tm *time;

	unsigned char *record; /* must be unsigned char to allow values > 128 */
	FILE  *history=NULL;

	char outstr[128]="";

	if (argc != 2) { 
		printf("Error, must give number of record to read (>= 1).\n");
		fflush(stdout);
		exit(1);
	} else {
		sscanf(argv[1],"%ld", &n);
	}

	history = fopen("history.dat","r");
	if (history == NULL) {
		printf("Error, history.dat not found.\n");
		fflush(stdout);
		exit(1);
	}

	record = (unsigned char *)calloc(1, RECORD_LENGTH_BYTES);

	/* printf("Record %ld:\n",n); */

	for (i=0; i<n; i++) {  
		if (! feof(history) ) {
			c = fread(record, RECORD_LENGTH_BYTES, 1, history);
			/*
			if (c < RECORD_LENGTH_BYTES) {
				printf("Error reading record %ld.\n",i);
				fflush(stdout);
			}
			*/
		} else {
			printf("Error, premature end of file.\n");
			fflush(stdout);
			fclose(history);
			exit(1);
		}
	}

	fclose(history);

	strncpy((char *)&date,         (char *)record+ 4, 4);
	strncpy((char *)&pressure,     (char *)record+ 8, 4);
	strncpy((char *)&wind_speed,   (char *)record+12, 4);
	/*
	strncpy((unsigned char *)&wind_dir,     (unsigned char *)record+16, 1);
	*/ /* Note below */
	wind_dir = *(record+16);
        strncpy((char *)&rain_total,   (char *)record+20, 4);
	strncpy((char *)&in_temp,      (char *)record+24, 4);
	strncpy((char *)&out_temp,     (char *)record+28, 4);
        /*
	strncpy((unsigned char *)&in_humidity,  (unsigned char *)record+32, 1);
	*/
	in_humidity  = *(record+32);
	/*
       	strncpy((unsigned char *)&out_humidity, (unsigned char *)record+34, 1);
	*/
	out_humidity = *(record+34);

	/* Note: for the one byte values (wind direction, humidity) we could
	 * just use"wind_dir = *(record+16)" etc., but use strncpy for 
	 * consistency with the other values.
	 * ... er, this breaks - the other byte of the int is undefined maybe?
	 */

	/* UNIX/Linux have the date in seconds since 1970-01-01 00:00:00; 
 	 * deal with this by plugging the first date in the text history file
	 * into mktime(), this give the number od seconds since 1970-01-01 for
 	 * that date. Subtracting this number from the date in the binary file
	 * (which is the number of seconds since [probably] 1900-01-01 gives
 	 * us a magic number which is the time shift between the two date 
	 * representations. Thus all we have to do to get the date from the 
	 * binary file into a form we can use with normal localtime() is 
	 * subtract this shift from it first.
	 */

	
	/*date = 3271313220ul; testing */
#ifndef LONG_DATE
	date = date - 2240611800ul; /* subtract magic number time shift */
#endif
	time = localtime((time_t *)&date);
        /*printf("%02d:%02d:%02d  %4d-%02d-%02d\n",time->tm_hour,time->tm_min,time->tm_sec,time->tm_year+1900,time->tm_mon+1,time->tm_mday);*/

	/* mktime() of first date in history.txt */
	/*
	time->tm_hour=0;
	time->tm_min=0;
	time->tm_sec=0;
	time->tm_mday=1;
	time->tm_mon=0;
	time->tm_year=100;
	printf("%u\n", (unsigned long)mktime(time));
	*/
	
#ifdef LONG_FORMAT
	
	printf("Date:                 ");

	printf("%s",asctime(localtime((time_t *)&date)));

	if (pressure < 700) {
		printf("Pressure:              no data\n");
	} else {
		printf("Pressure:            %6.1f  hPa\n",pressure);
	}

	printf("Indoor  temperature:   %-4.1f °C\n",in_temp);
	printf("Indoor  humidity:      ");
	if (in_humidity > 100) {
                printf("no data\n");
        } else {
                printf("%u    %%\n", in_humidity);
	}

	if (*(record+28) == 0x52 && *(record+29) == 0x38 && \
            *(record+30) == 0xA2 && *(record+31) == 0x42) {
		printf("Outdoor temperature:   no data\n");
	} else {
		printf("Outdoor temperature:   %-4.1f °C\n", out_temp);
	}
	printf("Outdoor humidity:      ");
	if (out_humidity > 100) {
		printf("no data\n");
	} else {
		printf("%u    %%\n", out_humidity);
	}

	/* Wind */
	if (*(record+12) == 0x00 && *(record+13) == 0x00 && \
	    *(record+14) == 0x4C && *(record+15) == 0x42) {
		printf("Wind speed:            no data\n");
		printf("Wind direction:        no data\n");
	} else {
		printf("Wind speed:            % 4.1f  m/s\n",wind_speed);
		printf("Wind direction:         ");
		printf_wind(wind_dir);
		printf("\n");
	}

	/* Rain */
	printf("Total rainfall:      % 6.1f  mm\n",rain_total);

#else
	if (pressure < 700) {
		sprintf(outstr,"  -  ");
	} else {
        	sprintf(outstr,"%5.1f", pressure);
	}
        sprintf(outstr,"%s\t%-4.1f", outstr, in_temp);

        if (in_humidity > 100) {
	        sprintf(outstr,"%s\t - ", outstr);
	} else {
	        sprintf(outstr,"%s\t%u", outstr, in_humidity);
	}
        if (*(record+28) == 0x52 && *(record+29) == 0x38 && \
            *(record+30) == 0xA2 && *(record+31) == 0x42) {
                sprintf(outstr,"%s\t - ", outstr);
        } else {
                sprintf(outstr,"%s\t%-4.1f", outstr, out_temp);
        }
        if (out_humidity > 100) {
                sprintf(outstr,"%s\t - ", outstr);
        } else {
                sprintf(outstr,"%s\t%u", outstr, out_humidity);
	}
	sprintf(outstr,"%s\t - ", outstr);	/* dewpoint */
	sprintf(outstr,"%s\t - ", outstr);	/* windchill */

        if (*(record+12) == 0x00 && *(record+13) == 0x00 && \
            *(record+14) == 0x4C && *(record+15) == 0x42) {
                sprintf(outstr,"%s\t - ", outstr);
		sprintf(outstr,"%s\t - ", outstr);
        } else {
                sprintf(outstr,"%s\t%4.1f", outstr,wind_speed);
                sprintf_wind(outstr, wind_dir);
	}

	sprintf(outstr,"%s\t%3.1f", outstr,rain_total);

	/* Date */
#ifndef LONG_DATE
	sprintf(outstr,"%s\t%4d-%02d-%02d %02d:%02d:%02d",outstr,time->tm_year+1900,time->tm_mon+1,time->tm_mday,time->tm_hour,time->tm_min,time->tm_sec);
#else
	sprintf(outstr,"%s\t%lu", outstr, date);
#endif

	printf("%s\n", outstr);
#endif

	return(0);
}

