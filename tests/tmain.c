#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "geohex3.h"

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
	return 0;
}
