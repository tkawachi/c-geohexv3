#ifndef GEOHEXV3_H__
#define GEOHEXV3_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct geohexv3_zone {
	int level;
	char *code;
	long x;
	long y;
	double lat;
	double lon;
};

struct geohexv3_zone* geohexv3_zone_alloc(void);
void geohexv3_zone_free(struct geohexv3_zone *zone);
int geohexv3_get_zone_by_location(double lat, double lon, int level, struct geohexv3_zone *zone);
int geohexv3_get_zone_by_code(const char *code, struct geohexv3_zone *zone);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GEOHEXV3_H__ */
