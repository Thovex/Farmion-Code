#pragma once

#define I_LOG_M(log, msg, i) UE_LOG(log, Warning, TEXT( FString(msg).Append(": %d") ), i);
#define I_LOG(log, i) UE_LOG(log, Warning, TEXT("%d"), i);

#define F_LOG_M(log, msg, f) UE_LOG(log, Warning, TEXT( FString(msg).Append(": %f") ), f);
#define F_LOG(log, f) UE_LOG(log, Warning, TEXT("%f"), f);

#define S_LOG_M(log, msg, s) UE_LOG(log, Warning, TEXT( FString(msg).Append(": %s") ), *s);
#define S_LOG(log, s) UE_LOG(log, Warning, TEXT("%s"), *s);

#define V_LOG_M(log, msg, v) UE_LOG(log, Warning, TEXT( FString(msg).Append(": %s") ), *v.ToString());
#define V_LOG(log, v) UE_LOG(log, Warning, TEXT("%s"), *v.ToString());
