#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "cpu_map.h"

struct cpu_map *cpu_map__read_all(void) 
{
	struct cpu_map *cpus = NULL;
	FILE *fp;
	
	fp = fopen("/sys/devices/system/cpu/online","r");
	if(!fp) 
		return cpu_map__def_new();
	
	cpus = cpu_map__read(fp);
	fclose(fp);
	return cpus;
}

static struct cpu_map *cpu_map__trim_new(int nr_cpus,int *tmp_cpus)
{
	size_t payload_size = nr_cpus*sizeof(int);
	struct cpu_map *cpus = malloc(sizeof(*cpus) + payload_size);
	
	if(cpus != NULL) {
		cpus->nr = nr_cpus;
		memcpy(cpus->map, tmp_cpus, payload_size);
	}
	return cpus;
}

static struct cpu_map *cpu_map__read(FILE *fp) 
{
	struct cpu_map *cpus = NULL;
	int nr_cpus = 0;
	int *tmp_cpus = NULL, *tmp;
	int max_entries = 0;
	int n, cpu, prev;
	char sep;
	
	sep = 0;
	prev = -1;
	for(;;) {
		n = fscanf(fp,"%u%c",&cpu,&sep);
		if(n <= 0)
			break;
		if(prev >= 0) {
			int new_max = nr_cpus + cpu - prev - 1;
			
			if(new_max >= max_entries) {
				max_entries = new_max + MAX_NR_CPUS/2;
				tmp = realloc(tmp_cpus, max_entries*sizeof(int));
				if(tmp == NULL)
					goto out_free_map;
				tmp_cpus = tmp;
			}
			while(++prev < cpu)
				tmp_cpus[nr_cpus++] = prev;
		}
		if(nr_cpus == max_entries) {
			max_entries += MAX_NR_CPUS;
			tmp = realloc(tmp_cpus, max_entries*sizeof(int));
			if(tmp == NULL)
				goto out_free_map;
			tmp_cpus = tmp;
		}
		
		tmp_cpus[nr_cpus++] = cpu;
		if(n==2 && sep=='-')
			prev = cpu;
		else
			prev = -1;
		if(n==1 || sep=='\n')
			break;
	}
	
	if(nr_cpus > 0)
		cpus = cpu_map__trim_new(nr_cpus, tmp_cpus);
	else 
		cpus = cpu_map__def_new();

out_free_map:
	free(tmp_cpus);
	return cpus;
}

static struct cpu_map *cpu_map__def_new(void)
{
	struct cpu_map *cpus = NULL;
	int nr_cpus = sysconf(_SC_NPROCESSORS_ONLN);
	
	if(nr_cpus < 0) 
		return NULL;

	cpus = malloc(sizeof(*cpus) + nr_cpus*sizeof(int));
	if(cpus != NULL) {
		int i;
		for(i=0; i<nr_cpus; i++) 
			cpus->map[i] = i;
		cpus->nr = nr_cpus;
	}
	return cpus;
}
