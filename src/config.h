#pragma once

#if defined(Q_OS_WIN)
#define OS_NAME "Windows"
// #elif defined(Q_OS_OSX)
// #define OS_NAME "Mac OSX"
#elif defined(Q_OS_LINUX)
#define OS_NAME "Linux"
#else
#define OS_NAME "Unknown"
#endif

#include <QString>

static const QString APP_NAME("LAN_file_sender");

#define DefaultBroadcastPort 56780
#define DefaultTransferPort 17116
#define DefaultFileBufferSize 96 * 1024  // 96 KB
#define MaxFileBufferSize 1024 * 1024    // 1 MB
#define DefaultMaxSendThread 5
#define DefaultMaxReceiveThread 5

static const QString BroadCastMagic{"7463921"};
