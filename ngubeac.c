#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <glib.h>
#include <gmodule.h>

#define NGUIX	20
#define NGUIY	17

static gint beacons = 2;
static gint beamwidth = 50;
static gint generations = 70;
static gint btype = 1;
static gchar *filename = NULL;
static gboolean randmap = FALSE;
static gint mapn = 0;
static gboolean single = FALSE;

static GOptionEntry entries[] = {
    { "beacons", 'b', 0, G_OPTION_ARG_INT, &beacons, "Number of cell types to use: 2=box 3=knight 7=arrows 9=lines 10=donut", "N" },
    { "width", 'w', 0, G_OPTION_ARG_INT, &beamwidth, "Beam width", "W" },
    { "generations", 'g', 0, G_OPTION_ARG_INT, &generations, "Generations", "G" },
    { "btype", 't', 0, G_OPTION_ARG_INT, &btype, "Beacon type: 0=speed 1=prod 2=efficiency. Default 1", "T" },
    { "map", 'm', 0, G_OPTION_ARG_INT, &mapn, "Map: 0=tutorial 1=flesh 2=tronne", "M" },
    { "file", 'f', 0, G_OPTION_ARG_FILENAME, &filename, "Filename", NULL },
    { "random", 'r', 0, G_OPTION_ARG_NONE, &randmap, "Randomize layout", NULL },
    { "single", 's', 0, G_OPTION_ARG_NONE, &single, "Optimize a single square rather than the whole map", NULL },
    { NULL }   
};

char maps[][NGUIX*NGUIY] = { { 
	0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,0,
	0,0,0,0,0,0,0,1,0,1,1,0,0,0,1,1,1,1,1,0,
	0,0,0,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,0,
	0,0,0,1,1,1,1,0,1,1,0,0,0,1,0,1,0,1,1,0,
	0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,0,
	0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,
	0,0,1,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,0,
	0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 
	0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,1,1,
	1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,1,1,1,0,0,
	1,1,1,1,1,1,1,0,0,1,1,0,0,1,1,1,1,1,1,0,
	1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,
	1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,
	1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,
	0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,
	1,1,1,0,1,0,1,0,0,0,0,0,0,1,1,1,1,1,0,0,
	1,1,1,0,1,0,1,0,0,0,0,0,0,1,1,1,1,0,0,0,
	1,1,1,0,1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,
	1,1,1,0,1,0,1,1,0,0,0,0,1,1,1,1,1,1,0,0,
	1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,0,0,0,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0 },
    {
	1,0,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,1,1,0,
	1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,
	1,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,0,1,0,1,1,0,1,1,1,1,0,1,1,
	1,1,1,0,1,1,0,0,0,1,1,1,1,1,1,1,0,1,1,1,
	0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,
	1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,0,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,
	1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,0,0,1,1,
	0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,0,0,0,0,1,
	1,1,0,1,1,0,0,1,0,0,0,1,1,0,0,0,0,0,1,1,
	1,1,0,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,
	0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,
	1,1,0,1,1,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,
	1,1,0,1,1,0,0,0,1,1,1,0,1,1,1,1,0,1,1,1 } } ;

struct beam {
    GSequence *g;
    GHashTable *seen;
    int left;
};

struct map {
    int x, y;
    char *map;
    unsigned int score;
    uint64_t hash;
};

void free_map(struct map *m)
{
    if (m) {
	free(m->map);
	free(m);
    }
}

struct map *copy_map(struct map *m)
{
    struct map *rv = malloc(sizeof(struct map));

    rv->x = m->x;
    rv->y = m->y;
    rv->map = malloc(m->x * m->y);
    memcpy(rv->map, m->map, m->x * m->y);
    rv->score = m->score;
        
    return rv;
}

/*            box knight arrow line donut */
int bv[][5] = { { 40, 35, 26, 27, 23 },
                { 30, 35, 22, 27, 26 },
                { -15, -13, -7, -9, -8 } };

char translate[] = " .*k<^>v|-o";
#define B(a,b,c) if ((a)>=0 && (b)>=0 && (a)<m->x && (b)<m->y && score[(a)+((b)*m->x)]) score[(a)+((b)*m->x)] += (c)
/* Apply the bonus (if any) from the beacon at x,y to the score map */
void apply_bonus(struct map *m, unsigned int *score, int x, int y)
{ 
    switch (m->map[x + (y * m->x)]) {
        case 2: // *
            B(x-1,y-1,bv[btype][0]);
            B(x-1,y+0,bv[btype][0]);
            B(x-1,y+1,bv[btype][0]);
            B(x+0,y-1,bv[btype][0]);
            B(x+0,y+1,bv[btype][0]);
            B(x+1,y-1,bv[btype][0]);
            B(x+1,y+0,bv[btype][0]);
            B(x+1,y+1,bv[btype][0]);
            break;
        case 3: // k
            B(x-1,y-2,bv[btype][1]);
            B(x+1,y-2,bv[btype][1]);
            B(x-2,y-1,bv[btype][1]);
            B(x-2,y+1,bv[btype][1]);
            B(x-1,y+2,bv[btype][1]);
            B(x+1,y+2,bv[btype][1]);
            B(x+2,y-1,bv[btype][1]);
            B(x+2,y+1,bv[btype][1]);
            break;
        case 4: // <
            B(x-5,y  , bv[btype][2]);
            B(x-4,y-1, bv[btype][2]);
            B(x-4,y  , bv[btype][2]);
            B(x-4,y+1, bv[btype][2]);
            B(x-3,y-2, bv[btype][2]);
            B(x-3,y-1, bv[btype][2]);
            B(x-3,y  , bv[btype][2]);
            B(x-3,y+1, bv[btype][2]);
            B(x-3,y+2, bv[btype][2]);
            B(x-2,y  , bv[btype][2]);
            B(x-1,y  , bv[btype][2]);
            break;
        case 5: // ^
            B(x  ,y-5, bv[btype][2]);
            B(x-1,y-4, bv[btype][2]);
            B(x  ,y-4, bv[btype][2]);
            B(x+1,y-4, bv[btype][2]);
            B(x-2,y-3, bv[btype][2]);
            B(x-1,y-3, bv[btype][2]);
            B(x  ,y-3, bv[btype][2]);
            B(x+1,y-3, bv[btype][2]);
            B(x+2,y-3, bv[btype][2]);
            B(x  ,y-2, bv[btype][2]);
            B(x  ,y-1, bv[btype][2]);
            break;
        case 6: // >
            B(x+5,y  , bv[btype][2]);
            B(x+4,y-1, bv[btype][2]);
            B(x+4,y  , bv[btype][2]);
            B(x+4,y+1, bv[btype][2]);
            B(x+3,y-2, bv[btype][2]);
            B(x+3,y-1, bv[btype][2]);
            B(x+3,y  , bv[btype][2]);
            B(x+3,y+1, bv[btype][2]);
            B(x+3,y+2, bv[btype][2]);
            B(x+2,y  , bv[btype][2]);
            B(x+1,y  , bv[btype][2]);
            break;
        case 7: // v
            B(x  ,y+5, bv[btype][2]);
            B(x-1,y+4, bv[btype][2]);
            B(x  ,y+4, bv[btype][2]);
            B(x+1,y+4, bv[btype][2]);
            B(x-2,y+3, bv[btype][2]);
            B(x-1,y+3, bv[btype][2]);
            B(x  ,y+3, bv[btype][2]);
            B(x+1,y+3, bv[btype][2]);
            B(x+2,y+3, bv[btype][2]);
            B(x  ,y+2, bv[btype][2]);
            B(x  ,y+1, bv[btype][2]);
            break;
        case 8: // |
            B(x  ,y-6, bv[btype][3]);
            B(x  ,y-5, bv[btype][3]);
            B(x  ,y-4, bv[btype][3]);
            B(x  ,y-3, bv[btype][3]);
            B(x  ,y-2, bv[btype][3]);
            B(x  ,y-1, bv[btype][3]);
            B(x  ,y+1, bv[btype][3]);
            B(x  ,y+2, bv[btype][3]);
            B(x  ,y+3, bv[btype][3]);
            B(x  ,y+4, bv[btype][3]);
            B(x  ,y+5, bv[btype][3]);
            B(x  ,y+6, bv[btype][3]);
            break;
        case 9: // -
            B(x-6, y, bv[btype][3]);
            B(x-5, y, bv[btype][3]);
            B(x-4, y, bv[btype][3]);
            B(x-3, y, bv[btype][3]);
            B(x-2, y, bv[btype][3]);
            B(x-1, y, bv[btype][3]);
            B(x+1, y, bv[btype][3]);
            B(x+2, y, bv[btype][3]);
            B(x+3, y, bv[btype][3]);
            B(x+4, y, bv[btype][3]);
            B(x+5, y, bv[btype][3]);
            B(x+6, y, bv[btype][3]);
            break;
	case 10: // o
	    B(x-2, y-2, bv[btype][4]);
	    B(x-2, y-1, bv[btype][4]);
	    B(x-2, y  , bv[btype][4]);
	    B(x-2, y+1, bv[btype][4]);
	    B(x-2, y+2, bv[btype][4]);
	    B(x-1, y-2, bv[btype][4]);
	    B(x-1, y+2, bv[btype][4]);
	    B(x  , y-2, bv[btype][4]);
	    B(x  , y+2, bv[btype][4]);
	    B(x+1, y-2, bv[btype][4]);
	    B(x+1, y+2, bv[btype][4]);
	    B(x+2, y-2, bv[btype][4]);
	    B(x+2, y-1, bv[btype][4]);
	    B(x+2, y  , bv[btype][4]);
	    B(x+2, y+1, bv[btype][4]);
	    B(x+2, y+2, bv[btype][4]);
	    break;
    }
}

/* Calculate the score of the given map */
unsigned int score(struct map *m)
{
    int i, j, rv = 0;
    unsigned int score[NGUIX*NGUIY];
    uint64_t hash = 0;
    
    /* Initialize score map */
    for (i=0; i<(m->x*m->y); i++) {
        if (m->map[i] == 1) {
            score[i] = 100;
        } else {
            score[i] = 0;
        }
    }

    /* Apply beacon bonuses to map */
    for (i=0; i<m->x; i++) {
        for (j=0; j<m->y; j++) {
            apply_bonus(m, score, i, j);
        }
    }

    /* Calculate score */
    for (i=0; i<(m->x*m->y); i++) {
	if (single) {
	    if (score[i] > rv)
		rv = score[i];
	} else {
            rv += score[i];
	}
	hash = (hash * 33) + m->map[i];
    }

    m->score = rv;
    m->hash = hash;

    return rv;
}

/* Return the n-th neighbor of the given map, or NULL if there is none */
struct map *neighbor(struct map *m, int nc, int n)
{
    int x, c;
    struct map *rv = NULL;

    c = (n % (nc-1)) + 1;
    n = n / (nc-1);

    for (x = 0; x < (m->x*m->y); x++) {
        if (m->map[x]) {
            if (n-- == 0) {
                rv = copy_map(m);
		rv->score = 0;
		rv->hash = 0;
                rv->map[x] += c;
                if (rv->map[x] > nc) {
                    rv->map[x] -= nc;
                }
                return rv;
            }
        }
    }

    return NULL;
}

void print_map(struct map *m)
{
    int x, y;

    for (y = 0; y < m->y; y++) {
        for (x = 0; x < m->x; x++) {
            putchar(translate[(int)m->map[x + (y * m->x)]]);
        }
        putchar('\n');
    }
    putchar('\n');
}

gint map_cmp(struct map *a, struct map *b, gpointer c)
{
    if (a->score == 0)
        score(a);
    if (b->score == 0)
        score(b);
    return b->score - a->score;
}

void
iter_maps(struct map *m, struct beam *beam)
{
    if (!g_hash_table_add(beam->seen, (void *)m->hash)) {
	return;
    }

    beam->left--;

    int i=0;
    struct map *mn;
    while ((mn = neighbor(m, beacons, i++)))  {
	g_sequence_insert_sorted(beam->g, mn, (GCompareDataFunc)map_cmp, NULL);
    }
}

struct map *
read_map(FILE *f)
{
    struct map *m = malloc(sizeof(struct map));
    m->x = NGUIX;
    m->y = NGUIY;
    m->map = malloc(m->x*m->y);

    int n=0;
    while (n < (NGUIX*NGUIY) && !feof(f)) {
	switch(fgetc(f)) {
		case ' ':
			m->map[n++] = 0;
			break;
		case '.':
			m->map[n++] = 1;
			break;
		case '*':
			m->map[n++] = 2;
			break;
		case 'k':
			m->map[n++] = 3;
			break;
		case '<':
			m->map[n++] = 4;
			break;
		case '^':
			m->map[n++] = 5;
			break;
		case '>':
			m->map[n++] = 6;
			break;
		case 'v':
			m->map[n++] = 7;
			break;
		case '|':
			m->map[n++] = 8;
			break;
		case '-':
			m->map[n++] = 9;
			break;
		case 'o':
			m->map[n++] = 10;
			break;
	}
    }

    return m;
}

/* Randomize the beacons on the map. 50% of spaces will be left blank and 50%
 * set to a random beacon. */
void
randomize_map(struct map *m)
{
    int i;
    for (i=0; i<m->x*m->y; i++) {
	if (m->map[i]) {
	    m->map[i] = g_random_boolean() ? 1 : g_random_int_range(1,beacons)+1;
	}
    }
}

int main(int argc, char *argv[])
{
    GError *error = NULL;
    GOptionContext *context;

    context = g_option_context_new ("- optimize beacons ");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
	    g_print ("option parsing failed: %s\n", error->message);
	    exit (1);
    }

    struct map *orig ;
    int i=0;
    struct map *n;
    
    if (filename) {
	orig = read_map(fopen(filename, "r"));
    } else {
	orig = malloc(sizeof(struct map));
	orig->x = NGUIX;
	orig->y = NGUIY;
	orig->map = malloc(NGUIX*NGUIY);
	memcpy(orig->map, maps[mapn], NGUIX*NGUIY);
    }

    if (randmap) {
	randomize_map(orig);
    }

    GHashTable *seen = g_hash_table_new(NULL, NULL);
    GSequence *g = g_sequence_new((GDestroyNotify)free_map);
    while ((n = neighbor(orig, beacons, i++)))  {
	g_sequence_insert_sorted(g, n, (GCompareDataFunc)map_cmp, NULL);
    }

    struct map *champion = NULL;
    int bw = beamwidth;
    for (i=0; i<generations; i++) {
	struct beam b;
	GSequenceIter *p1, *p2;
	b.g = g_sequence_new((GDestroyNotify)free_map);
	b.seen = seen;
	b.left = bw;

	p1 = g_sequence_get_begin_iter(g);
	while (b.left > 0 && !g_sequence_iter_is_end(p1)) {
		p2 = g_sequence_iter_move(p1, b.left+1);
		g_sequence_foreach_range(p1, p2, (GFunc)iter_maps, &b);
		p1 = g_sequence_iter_move(p2, 1);
	}
	g_sequence_free(g);
	g = b.g;
	
	struct map *best = g_sequence_get(g_sequence_get_begin_iter(g));
	if (!champion || best->score > champion->score) {
	    free_map(champion);
	    champion = copy_map(best);
	    putchar('\n');
	    print_map(best);
	    printf("%d: %d\n", i, best->score);

	    bw = beamwidth;
	} else {
	    bw = (bw * 105) / 100;
	}
	putchar('.');
	fflush(stdout);
    }
    putchar('\n');
    fflush(stdout);

    g_hash_table_destroy(seen);

    return 0;
}
