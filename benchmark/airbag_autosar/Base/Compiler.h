#ifndef COMPILER_H
#define COMPILER_H

#define AUTOMATIC
#define TYPEDEF
#define NULL_PTR ((void *)0)
#define FUNC(rettype, memclass) rettype
#define VAR(type, memclass) type
#define P2VAR(ptrtype, memclass, ptrclass) ptrtype *
#define P2CONST(ptrtype, memclass, ptrclass) const ptrtype *
#define CONST(type, memclass) const type

#endif 
