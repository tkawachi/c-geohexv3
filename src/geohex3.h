#ifndef GEOHEXV3_H__
#define GEOHEXV3_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct geohex3_zone {
	int level;
	char *code;
	long x;
	long y;
	double lat;
	double lon;
};

struct geohex3_zone* geohex3_zone_alloc(void);
void geohex3_zone_free(struct geohex3_zone *zone);
int geohex3_get_zone_by_location(double lat, double lon, int level, struct geohex3_zone *zone);
int geohex3_get_zone_by_code(const char *code, struct geohex3_zone *zone);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GEOHEXV3_H__ */
