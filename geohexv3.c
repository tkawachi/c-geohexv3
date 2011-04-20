#include "geohexv3.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static const char * const H_KEY = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz";
static const double H_BASE = 20037508.34;
static const double H_DEG = M_PI * (30.0 / 180.0);
#define H_K (tan(H_DEG))
static const int MAX_LEVEL = 15;
#define MAX_CODE_LEN (MAX_LEVEL + 2)

struct xy {
	double x, y;
};

struct loc {
	double lat, lon;
};

static double
calc_hex_size(int level) {
	return H_BASE / pow(3.0, level + 1);
}

static struct xy
loc2xy(double lon, double lat) {
	struct xy result;
	result.x = lon * H_BASE / 180.0;
	result.y = log(tan((90.0 + lat) * M_PI / 360.0)) / (M_PI / 180.0);
	result.y *= H_BASE / 180.0;
	return result;
}

static struct loc
xy2loc(double x, double y) {
	struct loc result;
	result.lon = (x / H_BASE) * 180.0;
	result.lat = (y / H_BASE) * 180.0;
	result.lat = 180 / M_PI *
		(2.0 * atan(exp(result.lat * M_PI / 180.0)) - M_PI / 2.0);
	return result;
}

struct geohexv3_zone*
geohexv3_zone_alloc(void)
{
	struct geohexv3_zone *v;
	v = malloc(sizeof(*v));
	if (!v) {
		return NULL;
	}
	memset(v, 0, sizeof(*v));
	v->code = malloc(MAX_CODE_LEN + 1);
	if (!v->code) {
		free(v);
		return NULL;
	}
	return v;
}

void
geohexv3_zone_free(struct geohexv3_zone *zone)
{
	if (zone->code)
		free(zone->code);
	free(zone);
}

int
geohexv3_get_zone_by_location(double lat, double lon, int level, struct geohexv3_zone *zone)
{
	if (lat < -90 || lat > 90 || lon < -180 || lon > 180 ||
		level < 0 || level > MAX_LEVEL) {
		return -1;
	}
	zone->level = level;
	level += 2;
	double h_size = calc_hex_size(level);
	struct xy z_zy = loc2xy(lon, lat);
	double lon_grid = z_zy.x;
	double lat_grid = z_zy.y;
	double unit_x = 6 * h_size;
	double unit_y = 6 * h_size * H_K;
	double h_pos_x = (lon_grid + lat_grid / H_K) / unit_x;
	double h_pos_y = (lat_grid - H_K * lon_grid) / unit_y;
	long h_x_0 = (long)floor(h_pos_x);
	long h_y_0 = (long)floor(h_pos_y);
	double h_x_q = h_pos_x - h_x_0;
	double h_y_q = h_pos_y - h_y_0;
	long h_x = lround(h_pos_x);
	long h_y = lround(h_pos_y);

	if (h_y_q > -h_x_q + 1) {
		if ((h_y_q < 2 * h_x_q) && (h_y_q > 0.5 * h_x_q)) {
			h_x = h_x_0 + 1;
			h_y = h_y_0 + 1;
		}
	} else if (h_y_q < -h_x_q + 1) {
		if ((h_y_q > (2 * h_x_q) - 1) && (h_y_q < (0.5 * h_x_q) + 0.5)){
			h_x = h_x_0;
			h_y = h_y_0;
		}
	}

	double h_lat = (H_K * h_x * unit_x + h_y * unit_y) / 2;
	double h_lon = (h_lat - h_y * unit_y) / H_K;

	struct loc z_loc = xy2loc(h_lon, h_lat);
	double z_loc_x = z_loc.lon;
	double z_loc_y = z_loc.lat;
	if(H_BASE - h_lon < h_size){
		z_loc_x = 180;
		long h_xy = h_x;
		h_x = h_y;
		h_y = h_xy;
	}

	int code3_x[level + 1];
	int code3_y[level + 1];
	double mod_x = h_x;
	double mod_y = h_y;

	int i;
	for (i = 0; i <= level; ++i) {
		double h_pow = pow(3, level - i);
		if (mod_x >= ceil(h_pow / 2)) {
			code3_x[i] = 2;
			mod_x -= h_pow;
		} else if (mod_x <= -ceil(h_pow / 2)) {
			code3_x[i] = 0;
			mod_x += h_pow;
		} else {
			code3_x[i] = 1;
		}

		if (mod_y >= ceil(h_pow / 2)) {
			code3_y[i] = 2;
			mod_y -= h_pow;
		} else if (mod_y <= -ceil(h_pow / 2)) {
			code3_y[i] = 0;
			mod_y += h_pow;
		} else {
			code3_y[i] = 1;
		}
	}

	memset(zone->code, 0, MAX_CODE_LEN + 1);
	int h_1 = 0;
	for (i = 0; i < 3; ++i) {
		int d = code3_x[i] * 3 + code3_y[i];
		h_1 = h_1 * 10 + d;
	}
	int h_a1 = h_1 / 30;
	int h_a2 = h_1 % 30;
	zone->code[0] = H_KEY[h_a1];
	zone->code[1] = H_KEY[h_a2];

	for (i = 3; i <= level; ++i) {
		int d = code3_x[i] * 3 + code3_y[i];
		zone->code[i - 1] = '0' + d;
	}
	zone->lat = z_loc_y;
	zone->lon = z_loc_x;
	zone->x = h_x;
	zone->y = h_y;
	return 0;
}

int
geohexv3_get_zone_by_code(const char *code, struct geohexv3_zone *zone)
{
	int level = strlen(code);
	if (level - 2 < 0 || level - 2 > MAX_LEVEL) {
		return -1;
	}
	double h_size = calc_hex_size(level);
	double unit_x = 6 * h_size;
	double unit_y = 6 * h_size * H_K;

	int h_a = 0;
	char *cp;
	cp = strchr(H_KEY, code[0]);
	if (!cp) {
		return -1;
	}
	h_a += (cp - H_KEY) * 30;
	cp = strchr(H_KEY, code[1]);
	if (!cp) {
		return -1;
	}
	h_a += (cp - H_KEY);
	/* TODO incomplete impl */

	int h_decx[level + 1];
	int h_decy[level + 1];

	h_decx[0] = (h_a / 100) / 3;
	h_decy[0] = (h_a / 100) % 3;
	h_a %= 100;
	h_decx[1] = (h_a / 10) / 3;
	h_decy[1] = (h_a / 10) % 3;
	h_a %= 10;
	h_decx[2] = (h_a) / 3;
	h_decy[2] = (h_a) % 3;

	int i;
	for (i = 3; i <= level; ++i) {
		int n = code[i - 1] - '0';
		if (n < 0 || n > 8) {
			return -1;
		}
		h_decx[i] = n / 3;
		h_decy[i] = n % 3;
	}

	long h_x = 0;
	long h_y = 0;
	for (i = 0; i <= level; ++i) {
		double h_pow = pow(3, level - i);
		if (h_decx[i] == 0) {
			h_x -= h_pow;
		} else if (h_decx[i] == 2) {
			h_x += h_pow;
		}
		if (h_decy[i] == 0) {
			h_y -= h_pow;
		} else if (h_decy[i] == 2) {
			h_y += h_pow;
		}
	}

	double h_lat_y = (H_K * h_x * unit_x + h_y * unit_y) / 2;
	double h_lon_x = (h_lat_y - h_y * unit_y) / H_K;
	struct loc h_loc = xy2loc(h_lon_x, h_lat_y);
	if (h_loc.lon > 180) {
		h_loc.lon -= 360;
	} else if (h_loc.lon < -180) {
		h_loc.lon += 360;
	}

	if (zone->code != code) {
		strcpy(zone->code, code);
	}
	zone->lat = h_loc.lat;
	zone->lon = h_loc.lon;
	zone->x = h_x;
	zone->y = h_y;
	return 0;
}
