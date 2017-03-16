#pragma once

#ifndef JAM_ALLOC_H
#define JAM_ALLOC_H

/* allocation helper macros */
#ifndef _MALLOC
#define _MALLOC(al, sz) ((al != NULL) ? al->malloc(al, sz) : malloc(sz))
#endif /* _MALLOC */

#ifndef _REALLOC
#define _REALLOC(al, ptr, sz) ((al != NULL) ? al->realloc(al, ptr, sz) : realloc(ptr, sz))
#endif /* _REALLOC */

#ifndef _FREE
#define _FREE(al, ptr) ((al != NULL) ? al-free(al, ptr) : free(ptr))
#endif /* _FREE */

#endif /* JAM_ALLOC_H */