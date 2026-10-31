#ifndef STD_TYPES_H
#define STD_TYPES_H

#include "Platform_Types.h"
#include "Compiler.h"

typedef uint8 Std_ReturnType;

#ifndef E_OK
#define E_OK     ((Std_ReturnType)0U)
#endif
#ifndef E_NOT_OK
#define E_NOT_OK ((Std_ReturnType)1U)
#endif

#define STD_LOW     ((uint8)0U)
#define STD_HIGH    ((uint8)1U)
#define STD_IDLE    ((uint8)0U)
#define STD_ACTIVE  ((uint8)1U)
#define STD_OFF     ((uint8)0U)
#define STD_ON      ((uint8)1U)

#define STD_VENDOR_ID          0U
#define STD_MODULE_ID          197U
#define STD_AR_RELEASE_MAJOR_VERSION 4U
#define STD_AR_RELEASE_MINOR_VERSION 4U
#define STD_AR_RELEASE_REVISION_VERSION 0U

#endif /* STD_TYPES_H */
