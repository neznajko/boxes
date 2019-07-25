#include <libgen.h> // basename
#include <unistd.h> // getopt
#include <stdio.h>  // printf
#include <string.h> // strlen
#include <stdlib.h> // malloc

int boxez; // number of boxes

void usage(char *prognom)
{
    printf("Usage: %s [-hp] [\"box string\"]\n", prognom);
    printf("  -h       print this help and exit\n");
    printf("  -p       playing mode\n");
}
struct poz {    // position
    char *box;  // AA  BABB
    int   dest; // index of first empty box
    int  *orig; // list of possible moves
};
int getdest(char *box)
{
    return strchr(box, ' ') - box;
}
int *getorig(char *box)
// get orig list
{
    int *orig = (int *) malloc((boxez - 2) * sizeof (int));
    int *o_o = orig; // iterator
    int dest = getdest(box);
 
    int j;
    for (j = 0; j < dest - 1; j++)
	*o_o++ = j;

    for (j = dest + 2; j < boxez - 1; j++)
	*o_o++ = j;

    *o_o = -1; // Sentinel
    
    return orig;
}
struct poz *conspoz(char *box)
// constructor lego
{
    struct poz *p = (struct poz *) malloc(sizeof *p);
    
    p->box = box;
    p->orig = getorig(box);
    p->dest = getdest(box);
    
    return p;
}
void dumpboxes(char *box)
{
    putchar('|');
    while (*box)
	printf("%c|", *box++);
    putchar('\n');
}
void dumporig(int *orig)
{
    printf("orig: %d", *orig);

    while (*++orig != -1) {
	printf(", %d", *orig);
    }
    putchar('\n');
}
void dumpoz(struct poz *p)
{
    dumpboxes(p->box);
    printf("dest: %d\n", p->dest);
    dumporig(p->orig);
}
int ckorig(int orig, struct poz *p)
// ck if orig is in p->orig list
{
    for (int *o_o = p->orig; *o_o != -1; o_o++ ) {
	if (*o_o == orig) return 1;
    }
    return 0;
}
void svap(int orig, int dest, char *box)
{
    short Kf6;
    short *e2 = (short *) (box + orig);
    short *e4 = (short *) (box + dest);

    Kf6 = *e2;
    *e2 = *e4;
    *e4 = Kf6;
}
void move(int orig, struct poz *p)
// finally
{
    svap(orig, p->dest, p->box);
    p->orig = getorig(p->box);
    p->dest = getdest(p->box);
}
void playing_mode(char *box)
// Ys!
{
    int orig;
    struct poz *p = conspoz(box);
    
    while (1) {
   	dumpoz(p);
   	printf("Enter orig: ");
   	scanf("%d", &orig);
    	if (ckorig(orig, p)) {
    	    puts("OK");
    	    move(orig, p);
    	} else {
    	    puts("Naah!");
    	}
    }
}
struct history { // History stack, ck for duplicates
    char           *box;
    struct history *next;
} file; // history stack head

void record(char *box)
// log position into the archives
{
    struct history *a = (struct history *) malloc(sizeof *a);

    a->box = box;
    a->next = file.next;
    file.next = a;
}
void clear_history(void)
{
    struct history *i = file.next;
    struct history *j;

    while (i != NULL) {
	j = i;
	i = i->next;
	free(j);
    }
    file.next = NULL;
}
int ckmate(char *box)
// a-boxes should be before b-boxes
{
    return strchr(box, 'b') > strrchr(box, 'a');
}
int skan(char *box)
// skan history archievs for a position
{
    struct history *hsp = &file;

    while (hsp = hsp->next) {
	if (strcmp(hsp->box, box) == 0) return 1;
    }
    return 0;
}
struct node {            // tree node
    struct poz  *p;      //
    int          i;      // p->orig index counter
    int          level;  // root has 0
    struct node *parent; //
};
struct node *consnode(char *box)
{
    struct node *n = (struct node *) calloc(1, sizeof *n);

    n->p = conspoz(box);
    
    return n;
}
void dumpnode(struct node *n)
{
    dumpoz(n->p);
    printf("i: %i\n", n->i);
    printf("level: %d\n", n->level);
    printf("parent: %p\n", n->parent);
}
struct node *nextnode(struct node *n)
{
    struct poz *p = n->p;
    int orig;
    int dest = p->dest;
    char *box = strdup(p->box);

    while((orig = p->orig[n->i++]) != -1) {
	svap(orig, dest, box);
	if (skan(box) == 0) {
	    return consnode(box);
	}
	svap(dest, orig, box);
    }
    free(box);
    
    return NULL;
}
struct node *preorder; // planb solution
void *srch(struct node *root)
{
    char *box = root->p->box;;
    struct node *q;
/////////////////////////////////////////////////////////////////// boom 
    if (ckmate(box)) { // a solution found
	if (preorder == NULL || root->level < preorder->level) {
	    preorder = root;
	}
    }
    record(box); // save position to file stack
    
    while (q = nextnode(root)) {
	q->parent = root;
	q->level = root->level + 1;
	srch(q);
    }
}
void reveal(struct node *n, char *msg)
{
    clear_history();

    struct history *hsp = &file;

    do {
	record(n->p->box);
    } while (n = n->parent);

    puts(msg);
    while (hsp = hsp->next) {
    	dumpboxes(hsp->box);
    }
}
void planb(char *box)
{
    struct node *root = consnode(box);
    
    srch(root);
    
    if (preorder == NULL) {
	puts("._o"); // no solution
	return;
    }
    reveal(preorder, "Plan B (Top Secret):");
}
struct stack { // stack
    struct node  *node;
    struct stack *next;
};
void push_stack(struct node *n, struct stack *sp)
// push n into stack pointed by sp (head pointer)
{
    struct stack *a = (struct stack *) malloc(sizeof *a);
    
    a->node = n;
    a->next = sp->next;
    sp->next = a;
}
void clear_stack(struct stack *sp)
{
    struct stack *i = sp->next;
    struct stack *j;

    while (i != NULL) {
	j = i;
	i = i->next;
	free(j);
    }
    sp->next = NULL;
}
struct node *depth_srch(struct node *root)
{
    struct stack buf0 = {NULL, NULL};
    struct stack buf1 = {NULL, NULL};

    struct stack *parent   = &buf0;
    struct stack *children = &buf1;

    struct stack *iter;

    struct node *n;
    struct node *j;
    
    push_stack(root, parent);
    clear_history();
    record(root->p->box);
    
    while (1) {
	iter = parent;
	while (iter = iter->next) {
	    n = iter->node;
	    if (ckmate(n->p->box)) return n;
    	    while (j = nextnode(n)) {
		j->parent = n;
		j->level = n->level + 1;
    		push_stack(j, children);
		record(j->p->box);
   	    }
    	}
	if (children->next == NULL) {
	    return NULL;
	}
    	clear_stack(parent);
    	iter = parent;
    	parent = children;
    	children = iter;
    }
}
void planc(char *box)
{
    char msg[32];    
    struct node *root = consnode(box);
    struct node *j = depth_srch(root);

    if (j == NULL) {
	puts("._0");
    } else {
	sprintf(msg, "Mate in %d:", j->level);
	reveal(j, msg);
    }
}
int main(int argc, char *argv[])
{
    char *prognom = basename(argv[0]); // program name
    char *box; // box string
    int opt; // -1 if no more options
    int pmode = 0; // playing mode

    while ((opt = getopt(argc, argv, "hp")) != -1) {
   	switch (opt)
   	{
   	    case 'h': usage(prognom);
   		return 0;
   	    case 'p': pmode = 1;
   		break;
   	    case '?': usage(prognom);
   		return 1;
   	}
    }
    if (optind == argc) {
   	puts("Please enter box position (vidout \"):");
   	scanf("%m[ 0-9A-Za-z]", &box);
    } else {
   	box = argv[optind];
    }
    boxez = strlen(box);

    if (pmode) {
	playing_mode(box);
    } else {
	planb(box);
	planc(box);
    }
    return 0;
}
/////////////////////////////////////////////////////////////////// log:
