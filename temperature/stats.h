#ifndef STATS_H
#define STATS_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct 
    {
	unsigned long hist[8];
    } stats_t;
    
    void increment_stats(stats_t* const, const int);
#ifdef __cplusplus
}
#endif

#endif
