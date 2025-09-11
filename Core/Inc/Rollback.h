#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <stdbool.h>

// Hàm rollback: copy firmware từ Temp App (PARTITION_APP_BACKUP) sang Main App (PARTITION_APP_MAIN)
bool Rollback_ToMainApp(void);

#endif /* ROLLBACK_H */
