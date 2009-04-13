/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_PMAP_H
#define JOS_KERN_PMAP_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/memlayout.h>
#include <inc/assert.h>
struct Env;


/* This macro takes a kernel virtual address -- an address that points above
 * KERNBASE, where the machine's maximum 256MB of physical memory is mapped --
 * and returns the corresponding physical address.  It panics if you pass it a
 * non-kernel virtual address.
 */
#define PADDR(kva)						\
({								\
	physaddr_t __m_kva = (physaddr_t) (kva);		\
	if (__m_kva < KERNBASE)					\
		panic("PADDR called with invalid kva %08lx", __m_kva);\
	__m_kva - KERNBASE;					\
})

/* This macro takes a physical address and returns the corresponding kernel
 * virtual address.  It warns if you pass an invalid physical address. */
#define KADDR(pa)						\
({								\
	physaddr_t __m_pa = (pa);				\
	uint32_t __m_ppn = PPN(__m_pa);				\
	if (__m_ppn >= npage)					\
		warn("KADDR called with invalid pa %08lx", __m_pa);\
	(void*) (__m_pa + KERNBASE);				\
})



extern char bootstacktop[], bootstack[];

extern struct Page *pages;
extern size_t npage;

extern physaddr_t boot_cr3;
extern pde_t *boot_pgdir;

extern struct Segdesc (COUNT(SEG_COUNT) gdt)[];
extern struct Pseudodesc gdt_pd;

void	i386_detect_memory(void);
bool	enable_pse(void);
void	i386_vm_init(void);

void	page_init(void);
void	page_check(void);
int	page_alloc(struct Page **pp_store);
void	page_free(struct Page *pp);
int	page_insert(pde_t *pgdir, struct Page *pp, void *va, int perm);
void	page_remove(pde_t *pgdir, void *va);
struct Page *page_lookup(pde_t *pgdir, void *va, pte_t **pte_store);
void	page_decref(struct Page *pp);

void	tlb_invalidate(pde_t *pgdir, void *va);
void tlb_flush_global(void);

void *COUNT(len)
user_mem_check(struct Env *env, const void *DANGEROUS va, size_t len, int perm);

void *COUNT(len)
user_mem_assert(struct Env *env, const void *DANGEROUS va, size_t len, int perm);

static inline void cache_flush(void)
{
	wbinvd();
}

static inline void cacheline_flush(void* addr)
{
	clflush((uintptr_t*)addr);
}

static inline ppn_t
page2ppn(struct Page *pp)
{
	return pp - pages;
}

static inline physaddr_t
page2pa(struct Page *pp)
{
	return page2ppn(pp) << PGSHIFT;
}

static inline struct Page*
pa2page(physaddr_t pa)
{
	if (PPN(pa) >= npage)
		warn("pa2page called with pa larger than npage");
	return &pages[PPN(pa)];
}

static inline void*
page2kva(struct Page *pp)
{
	return KADDR(page2pa(pp));
}

pte_t *pgdir_walk(pde_t *pgdir, const void *va, int create);

#endif /* !JOS_KERN_PMAP_H */
