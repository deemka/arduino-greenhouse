typedef struct 
{
    unsigned long hist[8];
} stats_t;

void increment_stats(stats_t* const hist, const int mode)
{
    hist->hist[mode] += 1;
}
