#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "geohex3.h"

static char progname[BUFSIZ];

typedef enum {
	DECODE,
	ENCODE
} exec_mode_t;

static void
usage(void)
{
	printf("Usage:\n"
		"    %s [-d] code [code...]\n"
		"    %s -e lat lon level\n",
		progname, progname);
}

static void
print_zone(struct geohex3_zone *zone)
{
	printf("%.15lf %.15lf %d %ld %ld %s\n",
		zone->lat, zone->lon, zone->level, zone->x, zone->y,
		zone->code);
}

static void
decode_one(const char *code, struct geohex3_zone *zone)
{
	int ret;

	ret = geohex3_get_zone_by_code(code, zone);
	if (ret < 0) {
		fprintf(stderr, "failed to decode %s\n", code);
		return;
	}
	print_zone(zone);
}

static void
encode_one(const double lat, const double lon, const int level,
	struct geohex3_zone *zone)
{
	int ret;

	ret = geohex3_get_zone_by_location(lat, lon, level, zone);
	if (ret < 0) {
		fprintf(stderr,
			"failed to encode lat:%.15lf lon:%.15lf level:%d\n",
			lat, lon, level);
		return;
	}
	print_zone(zone);
}

static int
decode(int argc, char *argv[])
{
	int i, ret;
	struct geohex3_zone *zone;

	zone = geohex3_zone_alloc();
	if (!zone) {
		return -1;
	}
	if (argc > 0) {
		for (i = 0; i < argc; ++i) {
			decode_one(argv[i], zone);
		}
	} else {
		char buf[BUFSIZ];
		char code[BUFSIZ];

		while (fgets(buf, sizeof(buf), stdin) != NULL) {
			ret = sscanf(buf, "%s", code);
			if (ret != 1) {
				continue;
			}
			decode_one(code, zone);
		}
		if (ferror(stdin)) {
			perror("fgets");
		}
	}
	geohex3_zone_free(zone);
	return 0;
}

static int
encode(int argc, char *argv[])
{
	int level, ret;
	double lat, lon;
	struct geohex3_zone *zone;

	zone = geohex3_zone_alloc();
	if (!zone) {
		return -1;
	}
	if (argc > 0) {
		if (argc != 3) {
			fprintf(stderr, "encoding should take 3 args\n");
			goto finish;
		}
		lat = atof(argv[0]);
		lon = atof(argv[1]);
		level = atoi(argv[2]);
		encode_one(lat, lon, level, zone);
	} else {
		char buf[BUFSIZ];

		while (fgets(buf, sizeof(buf), stdin) != NULL) {
			ret = sscanf(buf, "%lf %lf %d", &lat, &lon, &level);
			if (ret != 3) {
				continue;
			}
			encode_one(lat, lon, level, zone);
		}
		if (ferror(stdin)) {
			perror("fgets");
		}
	}
finish:
	geohex3_zone_free(zone);
	return 0;
}

int
main(int argc, char *argv[])
{
	int ch;
	int ret = 0;
	exec_mode_t mode = DECODE; /* default DECODE */

	strcpy(progname, basename(argv[0]));

	while ((ch = getopt(argc, argv, "deh")) != -1) {
		switch (ch) {
		case 'd':
			mode = DECODE;
			break;
		case 'e':
			mode = ENCODE;
			break;
		case 'h':
		default:
			usage();
			return EXIT_FAILURE;
		}
	}
	argc -= optind;
	argv += optind;

	if (mode == DECODE) {
		ret = decode(argc, argv);
	} else {
		ret = encode(argc, argv);
	}
	if (ret < 0) {
		return EXIT_FAILURE;
	} else {
		return 0;
	}
}
