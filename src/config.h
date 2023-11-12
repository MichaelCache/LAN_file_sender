#pragma once

#if defined(Q_OS_WIN)
#define OS_NAME "Windows"
#elif defined(Q_OS_OSX)
#define OS_NAME "Mac OSX"
#elif defined(Q_OS_LINUX)
#define OS_NAME "Linux"
#else
#define OS_NAME "Unknown"
#endif

#include <QString>

static const QString ORG_NAME("MichaelCache");
static const QString APP_NAME("LAN_file_sender");

#define DefaultBroadcastInterval 5000 // 5s
#define DefaultBroadcastPort 56780
#define DefaultFileInfoPort 17115
#define DefaultTransferPort 17116
#define DefaultFileBufferSize 96 * 1024  // 96 KB
#define MaxFileBufferSize 1024 * 1024    // 1 MB
#define DefaultMaxSendThread 5
#define DefaultMaxReceiveThread 5

static const QString BROAD_MSG_MAGIC{"7463921"};

static const QString SETTINGS_FILE{"LANsenderconfig"};

static const QString HOST_NAME{"HostName"};
static const QString FILE_BUFFER{"FileBufferSize"};
static const QString SEND_THREAD{"SendMaxThread"};
static const QString RECE_THREAD{"ReceiveMaxThread"};
static const QString DOWN_DIR{"DownloadDir"};
static const QString SEND_PORT{"SendPort"};
static const QString REPLACE{"ReplaceExistingFile"};
static const QString BROAD_INTER{"BroadcastInterval"};

