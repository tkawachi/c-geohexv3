#include <stdio.h>
#include <stdlib.h>

#include "geohexv3.h"

int
main(int argc, char *argv[])
{
	struct geohexv3_zone *zone = geohexv3_zone_alloc();
	if (zone == NULL) {
		fprintf(stderr, "alloc failed\n");
		exit(1);
	}
	int res = geohexv3_get_zone_by_location(
		33.35137950146622,135.6104480957031,0, zone);
	if (res < 0) {
		fprintf(stderr, "geohexv3_get_zone_by_location() failed\n");
		exit(1);
	}
	printf("33.35137950146622,135.6104480957031,0 -> %s\n", zone->code);

	res = geohexv3_get_zone_by_location(
		85.05112507763846,89.37952995300293,15, zone);
	if (res < 0) {
		fprintf(stderr, "geohexv3_get_zone_by_location() failed\n");
		exit(1);
	}
	printf("85.05112507763846,89.37952995300293,15 -> %s\n", zone->code);
	geohexv3_zone_free(zone);
}
