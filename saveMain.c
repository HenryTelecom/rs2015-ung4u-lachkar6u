#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int isInit;

static inline size_t word_align(size_t size) {
    return size + (sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
}

struct block {
    struct block *next, *prev;
    size_t        size;
    int           free;
    void         *data;
};


typedef struct block *Block;

Block first_h;

static void *Mem_Init() {
    static Block b = NULL;
    if (!b) {
        b = sbrk(word_align(sizeof(struct block)));
        if (b == (void*) -1) {
            exit(0);
        }
        b->next = NULL;
        b->prev = NULL;
        b->size = 0;
        b->free = 0;
        b->data = NULL;

    }
    isInit = 1;
    first_h = b;
    return b;
}


Block Mem_Alloc_Block_find(size_t s, Block *heap) {
    Block c = Mem_Init();
    for (; c && (!c->free || c->size < s); *heap = c, c = c->next);
    return c;
}

void Mem_Alloc_merge_next(Block c) {
    c->size = c->size + c->next->size + sizeof(struct block);
    c->next = c->next->next;
    if (c->next) {
        c->next->prev = c;
    }
}

void Mem_Alloc_split_next(Block c, size_t size) {
    Block newblock = (Block)((char*) c + size);
    newblock->prev = c;
    newblock->next = c->next;
    newblock->size = c->size - size;
    newblock->free = 1;
    newblock->data = newblock + 1;
    if (c->next) {
        c->next->prev = newblock;
    }
    c->next = newblock;
    c->size = size - sizeof(struct block);
}

void *Mem_Alloc(size_t size) {
    if (!size) return NULL;
    size_t length = word_align(size + sizeof(struct block));
    Block prev = NULL;
    Block c = Mem_Alloc_Block_find(size, &prev);
    if (!c) {
        Block newblock = sbrk(length);
        if (newblock == (void*) -1) {
            return NULL;
        }
        newblock->next = NULL;
        newblock->prev = prev;
        newblock->size = length - sizeof(struct block);
        newblock->data = newblock + 1;
        prev->next = newblock;
        c = newblock;
    } else if (length + sizeof(size_t) < c->size) {
        Mem_Alloc_split_next(c, length);
    }
    c->free = 0;
    return c->data;
}

void Mem_Free(void *ptr) {
    if (!ptr || ptr < Mem_Init() || ptr > sbrk(0)) return;
    Block c = (Block) ptr - 1;
    if (c->data != ptr) return;
    c->free = 1;

    if (c->next && c->next->free) {
        Mem_Alloc_merge_next(c);
    }
    if (c->prev->free) {
        Mem_Alloc_merge_next(c = c->prev);
    }
    if (!c->next) {
        c->prev->next = NULL;
        sbrk(- c->size - sizeof(struct block));
    }
}
/*fonction qui retourne la taille en octet du bloc alloue*/
int Mem_GetSize(void *ptr)
{
    if(isInit == 0)
        printf("veuiller initialiser la memoire\n");
    if (isInit == 1)
        {     
        /*on recupere l en tete*/
        Block c = (Block) ptr - 1;
        if (!c->free)
        {
            printf("la taille du bloc est : %d\n",c->size);
        } else
        {
            printf("bloc libre\n" );
        }
        return 0;
    }
}
/*teste si le pointeur appartient a un bloc memoire alloue*/
int Mem_isValid(void *ptr)
{
    printf("Mem_isValid\n");
    Block c;
    c = first_h;
    while(c->next != NULL)
    {
        printf("dedans\n");
        if((Block) ptr >= c+2 && (Block) ptr <= (c+2 + c->size))
            printf("le bloc est alloue\n");
        return 1;
        c = c->next;
    }
    printf("il ne faut pas supprimer le first_h\n");
    return -1;
}

int main(int argc, char const *argv[])
{
    int *tableauentier;
    tableauentier = Mem_Alloc(8* sizeof(int));
    
    printf("%p\n", &tableauentier);

    Mem_GetSize(tableauentier);
    printf("\n");
    int i;
    for (i = 0; i < 50; ++i)
    {
        tableauentier[i] = i;
        printf("%d\n", tableauentier[i]);
    }
    
    Mem_GetSize(tableauentier);
    
    printf("\n");
    printf("%d\n", tableauentier[3]);
    Mem_isValid(tableauentier);
    printf("\n");
    Mem_Free(tableauentier);

    return 0;
}