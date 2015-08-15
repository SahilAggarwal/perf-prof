#ifndef _H_CPUMAP
#define _H_CPUMAP

#define MAX_NR_CPUS 256

struct cpu_map {
	int nr;
	int map[];
};

struct cpu_map *cpu_map__read_all(void);

static struct cpu_map *cpu_map__trim_new(int, int *);

static struct cpu_map *cpu_map__read(FILE *);

static struct cpu_map *cpu_map__def_new(void);

#endif




