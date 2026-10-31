#ifndef MEMMAP_H
#define MEMMAP_H

#if defined(CAN_START_SEC_CODE)
#undef CAN_START_SEC_CODE
#elif defined(CAN_STOP_SEC_CODE)
#undef CAN_STOP_SEC_CODE
#elif defined(CANIF_START_SEC_CODE)
#undef CANIF_START_SEC_CODE
#elif defined(CANIF_STOP_SEC_CODE)
#undef CANIF_STOP_SEC_CODE
#elif defined(PDUR_START_SEC_CODE)
#undef PDUR_START_SEC_CODE
#elif defined(PDUR_STOP_SEC_CODE)
#undef PDUR_STOP_SEC_CODE
#elif defined(ECUM_START_SEC_CODE)
#undef ECUM_START_SEC_CODE
#elif defined(ECUM_STOP_SEC_CODE)
#undef ECUM_STOP_SEC_CODE
#elif defined(BSWM_START_SEC_CODE)
#undef BSWM_START_SEC_CODE
#elif defined(BSWM_STOP_SEC_CODE)
#undef BSWM_STOP_SEC_CODE
#elif defined(SCHM_START_SEC_CODE)
#undef SCHM_START_SEC_CODE
#elif defined(SCHM_STOP_SEC_CODE)
#undef SCHM_STOP_SEC_CODE
#else
/* No section macro selected. Kept permissive for educational build portability. */
#endif

#endif /* MEMMAP_H */
