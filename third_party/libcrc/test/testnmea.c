/*
 * Library: libcrc
 * File:    test/testnmea.c
 * Author:  Lammert Bies
 *
 * This file is licensed under the MIT License as stated below
 *
 * Copyright (c) 2008-2016 Lammert Bies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Description
 * -----------
 * The source file test/testnmea.c contains routines to test the functionality
 * of the NMEA routines in the libcrc library after compilation on a specific
 * platform.
 */

#include <stdio.h>
#include <string.h>
#include "testall.h"

#include "../include/checksum.h"


						/************************************************/
struct chk_tp {					/*						*/
	const char *	input;			/* NMEA Input string				*/
	const char *	output;			/* Expected checksum output			*/
};						/*						*/
						/************************************************/

static struct chk_tp checks[] = {
	{ "$GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A*74",		"74" },
	{ "$GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A*",		"74" },
	{ "$GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A*  ",		"74" },
	{ "$GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A",		"74" },
	{  "GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A*74",		"74" },
	{  "GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A*",		"74" },
	{  "GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A*  ",		"74" },
	{  "GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A",		"74" },
	{ "$GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,*5D",	"5D" },
	{ "$GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,*",		"5D" },
	{ "$GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,*  ",	"5D" },
	{ "$GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,",		"5D" },
	{  "GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,*5D",	"5D" },
	{  "GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,*",		"5D" },
	{  "GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,*  ",	"5D" },
	{  "GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,",		"5D" },
	{ "$PSRF100,0,9600,8,1,0*",								"0C" },
	{ "$PSRF102,9600,8,1,0*",								"12" },
	{ "$GPRMC,183729,A,3907.356,N,12102.482,W,000.0,360.0,080301,015.5,E*",			"6F" },
	{ "$GPRMB,A,,,,,,,,,,,,V*",								"71" },
	{ "$GPGGA,183730,3907.356,N,12102.482,W,1,05,1.6,646.4,M,-24.1,M,,*",			"75" },
	{ "$GPGSA,A,3,02,,,07,,09,24,26,,,,,1.6,1.6,1.0*",					"3D" },
	{ "$GPGSV,2,1,08,02,43,088,38,04,42,145,00,05,11,291,00,07,60,043,35*",			"71" },
	{ "$GPGSV,2,2,08,08,02,145,00,09,46,303,47,24,16,178,32,26,18,231,43*",			"77" },
	{ "$PGRME,22.0,M,52.9,M,51.0,M*",							"14" },
	{ "$GPGLL,3907.360,N,12102.481,W,183730,A*",						"33" },
	{ "$PGRMZ,2062,f,3*",									"2D" },
	{ "$PGRMM,WGS 84*",									"06" },
	{ "$GPBOD,,T,,M,,*",									"47" },
	{ "$GPRTE,1,1,c,0*",									"07" },
	{ "$GPRMC,183731,A,3907.482,N,12102.436,W,000.0,360.0,080301,015.5,E*",			"67" },
	{ "$GPRMB,A,,,,,,,,,,,,V*",								"71" },
	{ "$GPRMC,002454,A,3553.5295,N,13938.6570,E,0.0,43.1,180700,7.1,W,A*",			"3F" },
	{ "$GPRMB,A,,,,,,,,,,,,A,A*",								"0B" },
	{ "$GPGGA,002454,3553.5295,N,13938.6570,E,1,05,2.2,18.3,M,39.0,M,,*",			"7F" },
	{ "$GPGSA,A,3,01,04,07,16,20,,,,,,,,3.6,2.2,2.7*",					"35" },
	{ "$GPGSV,3,1,09,01,38,103,37,02,23,215,00,04,38,297,37,05,00,328,00*",			"70" },
	{ "$GPGSV,3,2,09,07,77,299,47,11,07,087,00,16,74,041,47,20,38,044,43*",			"73" },
	{ "$GPGSV,3,3,09,24,12,282,00*",							"4D" },
	{ "$GPGLL,3553.5295,N,13938.6570,E,002454,A,A*",					"4F" },
	{ "$GPBOD,,T,,M,,*",									"47" },
	{ "$PGRME,8.6,M,9.6,M,12.9,M*",								"15" },
	{ "$PGRMZ,51,f*",									"30" },
	{ "$HCHDG,101.1,,,7.1,W*",								"3C" },
	{ "$GPRTE,1,1,c,*",									"37" },
	{ "$GPRMC,002456,A,3553.5295,N,13938.6570,E,0.0,43.1,180700,7.1,W,A*",			"3D" },
	{ "$GPRMC,023042,A,3907.3837,N,12102.4684,W,0.0,156.1,131102,15.3,E,A*",		"36" },
	{ "$GPRMB,A,,,,,,,,,,,,A,A*",								"0B" },
	{ "$GPGGA,023042,3907.3837,N,12102.4684,W,1,04,2.3,507.3,M,-24.1,M,,*",			"75" },
	{ "$GPGSA,A,3,04,05,,,09,,,24,,,,,2.8,2.3,1.0*",					"36" },
	{ "$GPGSV,3,2,11,09,47,229,42,10,04,157,00,14,00,305,00,24,70,154,33*",			"79" },
	{ "$GPGLL,3907.3837,N,12102.4684,W,023042,A,A*",					"5E" },
	{ "$GPBOD,,T,,M,,*",									"47" },
	{ "$GPVTG,156.1,T,140.9,M,0.0,N,0.0,K*",						"41" },
	{ "$PGRME,8.4,M,23.8,M,25.7,M*",							"2B" },
	{ "$PGRMZ,1735,f*",									"34" },
	{ "$PGRMM,WGS 84*",									"06" },
	{ "$HCHDG,,,,15.3,E*",									"30" },
	{ "$GPRTE,1,1,c,*",									"37" },
	{ "$GPRMC,023044,A,3907.3840,N,12102.4692,W,0.0,156.1,131102,15.3,E,A*",		"37" },
	{ "$GPRMC,152926,V,6027.8259,N,02225.6713,E,10.8,0.0,190803,5.9,E,S*",			"22" },
	{ "$GPRMB,V,,,,,,,,,,,,A,S*",								"0E" },
	{ "$GPGGA,152926,6027.8259,N,02225.6713,E,8,09,2.0,44.7,M,20.6,M,,*",			"79" },
	{ "$GPGSA,A,3,07,08,09,11,18,23,26,28,29,,,,6.6,2.0,3.0*",				"38" },
	{ "$GPGSV,3,1,09,07,29,138,44,08,22,099,42,09,30,273,44,11,07,057,35*",			"75" },
	{ "$GPGSV,3,2,09,18,28,305,43,23,14,340,39,26,64,222,49,28,60,084,49*",			"7E" },
	{ "$GPGSV,3,3,09,29,52,187,48*",							"4E" },
	{ "$GPGLL,6027.8259,N,02225.6713,E,152926,V,S*",					"48" },
	{ "$GPBOD,,T,,M,,*",									"47" },
	{ "$PGRME,15.0,M,22.5,M,15.0,M*",							"1B" },
	{ "$PGRMZ,147,f,3*",									"19" },
	{ "$GPRTE,1,1,c,*",									"37" },
	{ "$GPRMC,152928,V,6027.8319,N,02225.6713,E,10.8,0.0,190803,5.9,E,S*",			"29" },
	{ "$GPGGA,184050.84,3907.3839,N,12102.4772,W,1,05,1.8,00543,M,,,,*",			"33" },
	{ "$GPRMC,184050.84,A,3907.3839,N,12102.4772,W,00.0,000.0,080301,15,E*",		"54" },
	{ "$GPGSA,A,3,24,07,09,26,05,,,,,,,,03.6,01.8,03.1*",					"05" },
	{ "$PMGNST,02.12,3,T,534,05.0,+03327,00*",						"40" },
	{ "$GPGLL,3907.3839,N,12102.4771,W,184051.812,A*",					"2D" },
	{ "$GPGGA,184051.81,3907.3839,N,12102.4771,W,1,05,1.8,00543,M,,,,*",			"34" },
	{ "$GPRMC,184051.81,A,3907.3839,N,12102.4771,W,00.0,000.0,080301,15,E*",		"53" },
	{ "$GPGSA,A,3,24,07,09,26,05,,,,,,,,03.6,01.8,03.1*",					"05" },
	{ "$GPGSV,3,1,08,07,57,045,43,09,48,303,48,04,44,144,,02,39,092,*",			"7F" },
	{ "$GPGSV,3,2,08,24,18,178,44,26,17,230,41,05,13,292,43,08,01,147,*",			"75" },
	{ "$GPGSV,3,3,08,,,,,,,,,,,,,,,,*",							"71" },
	{ "$GPGLL,3907.3840,N,12102.4770,W,184052.812,A*",					"21" },
	{ "$GPAPB,A,A,0.0,L,N,,,1.1,M,SIM002,1.1,M,,,*",					"21" },
	{ "$GPGSA,A,3,01,02,03,04,,,,,,,,,2.0,2.0,2.0*",					"34" },
	{ "$GPGSV,3,1,11,01,77,103,,13,53,215,,04,47,300,,20,47,090,*",				"76" },
	{ "$GPGSV,3,2,11,19,24,158,,07,21,237,,25,16,039,,24,11,315,*",				"73" },
	{ "$GPGSV,3,3,11,11,08,149,,27,00,179,,30,00,354,,,,,*",				"46" },
	{ "$GPGLL,5100.2111,N,00500.0006,E,104715.203,A*",					"37" },
	{ "$GPGGA,104715.20,5100.2111,N,00500.0006,E,1,04,2.0,-0047,M,,,,*",			"39" },
	{ "$GPRMB,A,0.00,L,SIM001,SIM002,5102.6069,N,00500.0000,E,002.4,000.,021.7,V*",		"0D" },
	{ "$GPRMC,104715.20,A,5100.2111,N,00500.0006,E,21.7,003.0,140801,01.,W*",		"70" },
	{ "$GPAPB,A,A,0.0,L,N,,,1.1,M,SIM002,1.1,M,,,*",					"21" },
	{ "$GPGSA,A,3,01,02,03,04,,,,,,,,,2.0,2.0,2.0*",					"34" },
	{ "$GPGGA,171537,3350.975,N,11823.991,W,2,07,1.1,-25.8,M,,M,1.8,,D*",			"17" },
	{ "$GPGLL,3350.975,N,11823.991,W,171537,A,D*",						"50" },
	{ "$GPRMC,171537,A,3350.975,N,11823.991,W,0.0,096.5,060401,013.0,E,D*",			"07" },
	{ "$GPVTG,096.5,T,083.5,M,0.0,N,0.0,K,D*",						"22" },
	{ "$GPGSA,A,2,04,09,07,24,02,05,26,,,,,,,1.1,*",					"3C" },
	{ "$GPGSV,2,1,07,04,62,120,47,09,52,292,53,07,42,044,41,24,38,179,45*",			"7B" },
	{ "$GPGSV,2,2,07,02,34,101,43,05,18,304,40,26,09,223,36,,,,*",				"48" },
	{ "$PRAYA,6,1,122,0,0,2,36,1,1,,,,,*",							"5A" },
	{ "$GPDTM,W84,,0.000000,N,0.000000,E,0.0,W84*",						"6F" },
	{ "$GPGGA,171538,3350.974,N,11823.991,W,2,07,1.1,-25.8,M,,M,1.8,,D*",			"19" },
	{ "$GPGGA,171537,3350.975,N,11823.991,W,2,07,1.1,-25.8,M,,M,1.8,,D*",			"17" },
	{ "$GPGLL,3350.975,N,11823.991,W,171537,A,D*",						"50" },
	{ "$GPRMC,171537,A,3350.975,N,11823.991,W,0.0,096.5,060401,013.0,E,D*",			"07" },
	{ "$GPVTG,096.5,T,083.5,M,0.0,N,0.0,K,D*",						"22" },
	{ "$GPGSA,A,2,04,09,07,24,02,05,26,,,,,,,1.1,*",					"3C" },
	{ "$GPGSV,2,1,07,04,62,120,47,09,52,292,53,07,42,044,41,24,38,179,45*",			"7B" },
	{ "$GPGSV,2,2,07,02,34,101,43,05,18,304,40,26,09,223,36,,,,*",				"48" },
	{ "$PRAYA,6,1,122,0,0,2,36,1,1,,,,,*",							"5A" },
	{ "$GPDTM,W84,,0.000000,N,0.000000,E,0.0,W84*",						"6F" },
	{ "$GPGGA,171538,3350.974,N,11823.991,W,2,07,1.1,-25.8,M,,M,1.8,,D*",			"19" },
	{ "$GPGGA,120557.916,5058.7456,N,00647.0515,E,2,06,1.7,108.5,M,47.6,M,1.5,0000*",	"7A" },
	{ "$GPGSA,A,3,20,11,25,01,14,31,,,,,,,2.6,1.7,1.9*",					"3B" },
	{ "$GPGSV,2,1,08,11,74,137,45,20,58,248,43,07,27,309,00,14,23,044,36*",			"7A" },
	{ "$GPGSV,2,2,08,01,14,187,41,25,13,099,39,31,11,172,37,28,09,265,*",			"71" },
	{ "$GPRMC,120557.916,A,5058.7456,N,00647.0515,E,0.00,82.33,220503,,*",			"39" },
	{ "$GPGGA,120558.916,5058.7457,N,00647.0514,E,2,06,1.7,109.0,M,47.6,M,1.5,0000*",	"71" },
	{ "$GPGGA,001430.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.6,M,-26.0,M,,*",		"5F" },
	{ "$GPGSA,A,3,15,18,14,,,31,,,23,,,,04.5,02.1,04.0*",					"0F" },
	{ "$GPGSV,3,1,10,15,48,123,35,18,36,064,36,14,77,186,39,03,36,239,29*",			"7A" },
	{ "$GPGSV,3,2,10,09,08,059,,31,35,276,35,17,10,125,,11,08,306,*",			"79" },
	{ "$GPGSV,3,3,10,23,41,059,37,25,06,173,*",						"70" },
	{ "$GPRMC,001430.003,A,3907.3885,N,12102.4767,W,000.0,175.3,220403,015.4,E*",		"71" },
	{ "$GPGGA,001431.003,3907.3885,N,12102.4767,W,1,05,02.1,00545.5,M,-26.0,M,,*",		"5D" },
	{ "$GPVTG,139.7,T,,M,010.3,N,019.1,K*",							"67" },
	{ "$GPGGA,050306,4259.8839,N,07130.3922,W,0,00,99.9,0010,M,,M,000,0000*",		"66" },
	{ "$GPGLL,4259.8839,N,07130.3922,W,050306,V*",						"20" },
	{ "$GPRMC,050306,V,4259.8839,N,07130.3922,W,010.3,139.7,291003,,*",			"10" },
	{ "$GPZDA,050306,29,10,2003,,*",							"43" },
	{ "$GPGSA,A,1,,,,,,,,,,,,,99.9,99.9,99.9*",						"09" },
	{ "$PSNY,0,00,05,500,06,06,06,06*",							"14" },
	{ "$GPRMC,162254.00,A,3723.02837,N,12159.39853,W,0.820,188.36,110706,,,A*",		"74" },
	{ "$GPVTG,188.36,T,,M,0.820,N,1.519,K,A*",						"3F" },
	{ "$GPGGA,162254.00,3723.02837,N,12159.39853,W,1,03,2.36,525.6,M,-25.6,M,,*",		"65" },
	{ "$GPGSA,A,2,25,01,22,,,,,,,,,,2.56,2.36,1.00*",					"02" },
	{ "$GPGSV,4,1,14,25,15,175,30,14,80,041,,19,38,259,14,01,52,223,18*",			"76" },
	{ "$GPGSV,4,2,14,18,16,079,,11,19,312,,14,80,041,,21,04,135,25*",			"7D" },
	{ "$GPGSV,4,3,14,15,27,134,18,03,25,222,,22,51,057,16,09,07,036,*",			"79" },
	{ "$GPGSV,4,4,14,07,01,181,,15,25,135,*",						"76" },
	{ "$GPGLL,3723.02837,N,12159.39853,W,162254.00,A,A*",					"7C" },
	{ "$GPZDA,162254.00,11,07,2006,00,00*",							"63" },
	{ NULL, NULL }
};

/*
 * int test_checksum_NMEA( bool verbose );
 *
 * The test_checksum_NMEA() routines performs a number of NMEA message checksum
 * tests to see if the output of the routine in the library matches expected
 * outputs. The number of mismatches is returned. Progress is printed to stdout.
 */

int test_checksum_NMEA( bool verbose ) {

	int a, errors;
	unsigned char buffer[4];

	errors = 0;

	printf( "Testing NMEA checksum: " );

	a = 0;
	while ( checks[a].input != NULL  &&  checks[a].output != NULL ) {

		if ( strcmp( (char *) checksum_NMEA( (const unsigned char *) checks[a].input, buffer ), checks[a].output ) ) {

			if ( verbose ) printf( "\n    FAIL: \"%s\" returns \"%s\" instead of \"%s\"", checks[a].input, buffer, checks[a].output );
			errors++;
		}
		a++;
	}

	if ( errors == 0 ) printf( "OK\n" );
	else {

		if ( verbose ) printf( "\n    " );
		printf( "FAILED %d checks\n", errors );
	}

	return errors;

}  /* test_checksum_NMEA */