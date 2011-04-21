#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "geohex3.h"

#define EPSILON 1e-10
int
eqdouble(double a, double b)
{
	return fabs(a - b) < EPSILON;
}

void
test_encode(const char *fname)
{
	FILE *f = fopen(fname, "r");
	char buf[BUFSIZ];
	int succ = 0, fail = 0;
	int res;
	struct geohex3_zone *zone = geohex3_zone_alloc();

	if (!f) {
		perror("fopen");
		return;
	}
	if (!zone) {
		fprintf(stderr, "alloc failed\n");
		return;
	}

	while (fgets(buf, sizeof(buf), f) != NULL) {
		double lat, lon;
		int level;
		char code[BUFSIZ];
		int r = sscanf(buf,"%lf,%lf,%d,%s", &lat, &lon, &level, code);
		if (r != 4) {
			continue;
		}
		res = geohex3_get_zone_by_location(lat, lon, level, zone);
		if (res == 0 && strcmp(code, zone->code) == 0) {
			succ ++;
		} else {
			fail ++;
		}
	}
	fclose(f);
	geohex3_zone_free(zone);

	printf("Encode: succ: %d, fail: %d\n", succ, fail);
}

void
test_decode(const char *fname)
{
	FILE *f = fopen(fname, "r");
	char buf[BUFSIZ];
	int succ = 0, fail = 0;
	int res;
	struct geohex3_zone *zone = geohex3_zone_alloc();
	struct geohex3_zone *zone2 = geohex3_zone_alloc();

	if (!f) {
		perror("fopen");
		return;
	}
	if (!zone || !zone2) {
		fprintf(stderr, "alloc failed\n");
		return;
	}

	while (fgets(buf, sizeof(buf), f) != NULL) {
		double lat, lon;
		int level;
		char code[BUFSIZ];
		int r = sscanf(buf,"%lf,%lf,%d,%s", &lat, &lon, &level, code);
		if (r != 4) {
			continue;
		}
		res = geohex3_get_zone_by_code(code, zone);
		if (res < 0 ||
			!eqdouble(lat, zone->lat) ||
			!eqdouble(lon, zone->lon) ||
			level != zone->level) {
			fprintf(stderr, "%d, %lf %lf, %lf %lf, %d %d\n",
				res, lat, zone->lat, lon, zone->lon,
				level, zone->level);
			fail ++;
			continue;
		}
		res = geohex3_get_zone_by_location(zone->lat, zone->lon, zone->level, zone2);
		if (res == 0 && strcmp(code, zone2->code) == 0) {
			succ ++;
		} else {
			fprintf(stderr, "%d %s %s\n", res, code, zone2->code);
			fail ++;
		}
	}
	fclose(f);
	geohex3_zone_free(zone);
	geohex3_zone_free(zone2);

	printf("Decode: succ: %d, fail: %d\n", succ, fail);
}

int
main(int argc, char *argv[])
{
	struct geohex3_zone *zone = geohex3_zone_alloc();
	if (zone == NULL) {
		fprintf(stderr, "alloc failed\n");
		exit(1);
	}
	int res = geohex3_get_zone_by_location(
		33.35137950146622,135.6104480957031,0, zone);
	if (res < 0) {
		fprintf(stderr, "geohex3_get_zone_by_location() failed\n");
		exit(1);
	}
	printf("33.35137950146622,135.6104480957031,0 -> %s\n", zone->code);

	res = geohex3_get_zone_by_location(
		85.05112507763846,89.37952995300293,15, zone);
	if (res < 0) {
		fprintf(stderr, "geohex3_get_zone_by_location() failed\n");
		exit(1);
	}
	printf("85.05112507763846,89.37952995300293,15 -> %s\n", zone->code);

	res = geohex3_get_zone_by_code("bb337184418811744", zone);
	if (res < 0) {
		fprintf(stderr, "geohex3_get_zone_by_code() failed\n");
		exit(1);
	}
	printf("bb337184418811744 -> %.15f,%.15f,%d\n", zone->lat, zone->lon, zone->level);

	geohex3_zone_free(zone);

	test_encode("encode.txt");
	test_decode("decode.txt");
	return 0;
}
