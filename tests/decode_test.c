#include <math.h>
#include <stdio.h>

#include "geohex3.h"

#define EPSILON 1e-10
int
eqdouble(double a, double b)
{
	return fabs(a - b) < EPSILON;
}

int
eqlon(double a, double b)
{
	double c = fabs(a - b);
	if (c > 360.0) {
		c -= 360.0;
	}
	return c < EPSILON;
}

int
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
		return -1;
	}
	if (!zone || !zone2) {
		fprintf(stderr, "alloc failed\n");
		return -1;
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
			!eqlon(lon, zone->lon) ||
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
	if (fail == 0) {
		return 0;
	} else {
		return -1;
	}
}

int main() {
	return test_decode("decode.txt");
}
