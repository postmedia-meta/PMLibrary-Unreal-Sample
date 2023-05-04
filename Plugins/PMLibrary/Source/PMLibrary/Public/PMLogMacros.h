#pragma once

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG, DEVELOPMENT
// ⚪⭕❌⛔✔➖🔹🔸

#define PM_LINE(...)								UE_LOG(LogTemp, Log,		TEXT("%hs(%d) ➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖"), __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define PM_LOG(text, ...)							UE_LOG(LogTemp, Log,		TEXT("%hs(%d) ⚪ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PM_CHECK(text, ...)							UE_LOG(LogTemp, Log,		TEXT("%hs(%d) ✔ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PM_CROSS(text, ...)							UE_LOG(LogTemp, Log,		TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PM_WARN(text, ...)							UE_LOG(LogTemp, Warning,	TEXT("%hs(%d) ⭕ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PM_ERROR(text, ...)							UE_LOG(LogTemp, Error,		TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define PM_SEQ_LOG(text, ...)						{ PM_LOG(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ⚪ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__);  }
#define PM_SEQ_CHECK(text, ...)						{ PM_CHECK(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ✔ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__);  }
#define PM_SEQ_CROSS(text, ...)						{ PM_CROSS(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__);  }
#define PM_SEQ_WARN(text, ...)						{ PM_WARN(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ⭕ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Warning, __FUNCTION__, __LINE__);  }
#define PM_SEQ_ERROR(text, ...)						{ PM_ERROR(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Error,  __FUNCTION__, __LINE__);  }

#define PM_SEQ_LOG_PARAMS(text, params, ...)		{ PM_LOG(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ⚪ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__, params);  }
#define PM_SEQ_CHECK_PARAMS(text, params, ...)		{ PM_CHECK(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ✔ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__, params);  }
#define PM_SEQ_CROSS_PARAMS(text, params, ...)		{ PM_CROSS(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__, params);  }
#define PM_SEQ_WARN_PARAMS(text, params, ...)		{ PM_WARN(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ⭕ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Warning, __FUNCTION__, __LINE__, params);  }
#define PM_SEQ_ERROR_PARAMS(text, params, ...)		{ PM_ERROR(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Error, __FUNCTION__, __LINE__, params);  }

#define PM_SCREEN_LOG(text, ...)					{ PM_LOG(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::Printf(TEXT("%hs(%d) ⚪ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}
#define PM_SCREEN_CHECK(text, ...)					{ PM_CHECK(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("%hs(%d) ✔ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}
#define PM_SCREEN_CROSS(text, ...)					{ PM_CROSS(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}
#define PM_SCREEN_WARN(text, ...)					{ PM_WARN(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("%hs(%d) ⭕ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}
#define PM_SCREEN_ERROR(text, ...)					{ PM_ERROR(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}

#define PM_SCREEN_LOG_T(time, text, ...)			{ PM_LOG(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, time, FColor::White, FString::Printf(TEXT("%hs(%d) ⚪ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}
#define PM_SCREEN_CHECK_T(time, text, ...)			{ PM_CHECK(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, time, FColor::Green, FString::Printf(TEXT("%hs(%d) ✔ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}
#define PM_SCREEN_CROSS_T(time, text, ...)			{ PM_CROSS(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, time, FColor::Red, FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}
#define PM_SCREEN_WARN_T(time, text, ...)			{ PM_WARN(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, time, FColor::Yellow, FString::Printf(TEXT("%hs(%d) ⭕ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}
#define PM_SCREEN_ERROR_T(time, text, ...)			{ PM_ERROR(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, time, FColor::Red, FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}

#define PM_SCREEN_MSG(color, time, text, ...)		{ PM_LOG(text, ##__VA_ARGS__); if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, time, color, FString::Printf(TEXT("%hs(%d) ⚪ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__)); }}

#define PM_SCREEN_SEQ_LOG(text, ...)				{ PM_SCREEN_LOG(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ⚪ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__);  }
#define PM_SCREEN_SEQ_CHECK(text, ...)				{ PM_SCREEN_CHECK(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ✔ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__);  }
#define PM_SCREEN_SEQ_CROSS(text, ...)				{ PM_SCREEN_CROSS(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Information, __FUNCTION__, __LINE__);  }
#define PM_SCREEN_SEQ_WARN(text, ...)				{ PM_SCREEN_WARN(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ⭕ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Warning, __FUNCTION__, __LINE__);  }
#define PM_SCREEN_SEQ_ERROR(text, ...)				{ PM_SCREEN_ERROR(text, ##__VA_ARGS__); UPMSeqManager::Get()->Log(FString::Printf(TEXT("%hs(%d) ❌ " text), __FUNCTION__, __LINE__, ##__VA_ARGS__), EPMSeqLogLevel::Error, __FUNCTION__, __LINE__);  }


#else

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHIPPING

#define PM_LINE(...)

#define PM_LOG(...)
#define PM_CHECK(...)
#define PM_CROSS(...)
#define PM_WARN(...)
#define PM_ERROR(...)

#define PM_SEQ_LOG(...)
#define PM_SEQ_CHECK(...)
#define PM_SEQ_CROSS(...)
#define PM_SEQ_WARN(...)
#define PM_SEQ_ERROR(...)

#define PM_SEQ_LOG_PARAMS(...)
#define PM_SEQ_CHECK_PARAMS(...)
#define PM_SEQ_CROSS_PARAMS(...)
#define PM_SEQ_WARN_PARAMS(...)
#define PM_SEQ_ERROR_PARAMS(...)

#define PM_SCREEN_LOG(...)
#define PM_SCREEN_CHECK(...)
#define PM_SCREEN_CROSS(...)
#define PM_SCREEN_WARN(...)
#define PM_SCREEN_ERROR(...)

#define PM_SCREEN_LOG_T(...)
#define PM_SCREEN_CHECK_T(...)
#define PM_SCREEN_CROSS_T(...)
#define PM_SCREEN_WARN_T(...)
#define PM_SCREEN_ERROR_T(...)

#define PM_SCREEN_MSG(...)

#define PM_SCREEN_SEQ_LOG(text, ...)
#define PM_SCREEN_SEQ_CHECK(text, ...)
#define PM_SCREEN_SEQ_CROSS(text, ...)
#define PM_SCREEN_SEQ_WARN(text, ...)
#define PM_SCREEN_SEQ_ERROR(text, ...)

#endif