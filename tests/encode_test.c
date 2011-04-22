#include <stdio.h>

#include "geohex3.h"

int
test_encode(const char *fname)
{
	FILE *f = fopen(fname, "r");
	char buf[BUFSIZ];
	int succ = 0, fail = 0;
	int res;
	struct geohex3_zone *zone = geohex3_zone_alloc();

	if (!f) {
		perror("fopen");
		return -1;
	}
	if (!zone) {
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
	if (fail == 0) {
		return 0;
	} else {
		return -1;
	}
}

int
main() {
	return test_encode("encode.txt");
}
