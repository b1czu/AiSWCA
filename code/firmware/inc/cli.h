#ifndef __CLI_H
#define __CLI_H

#if LOGGER_SHORT_DEBUG

#define LOG_INFO(M, ...) do{  if(INFO_LOG)printf_(INFO_LOG_MSG M "\n\r", \
														 ##__VA_ARGS__); \
													  }while(0)

#define LOG_DEBUG(M, ...) do{  if(DEBUG_LOG)printf_(DEBUG_LOG_MSG M "\n\r", \
														 ##__VA_ARGS__); \
													  }while(0)

#define LOG_ERROR(M, ...) do{  if(ERROR_LOG)printf_(ERROR_LOG_MSG M "\n\r", \
														 ##__VA_ARGS__); \
													  }while(0)

#elif LOGGER_FULL_DEBUG

#define LOG_INFO(M, ...) do{  if(INFO_LOG)printf_(INFO_LOG_MSG M "\t(%s:%d)\n\r", \
														 ##__VA_ARGS__,__FILE__,__LINE__); \
													  }while(0)

#define LOG_DEBUG(M, ...) do{  if(DEBUG_LOG)printf_(DEBUG_LOG_MSG M "\t(%s:%d)\n\r", \
														 ##__VA_ARGS__,__FILE__,__LINE__); \
													  }while(0)

#define LOG_ERROR(M, ...) do{  if(ERROR_LOG)printf_(ERROR_LOG_MSG M "\t(%s:%d)\n\r", \
														 ##__VA_ARGS__,__FILE__,__LINE__); \
													  }while(0)

#else

#define LOG_INFO(M, ...)
#define LOG_DEBUG(M, ...)
#define LOG_ERROR(M, ...)

#endif



/* CLI for AiSWCA */
void cli_init(void);
void cli_put_char(char);

void USART2_IRQHandler(void);


#endif /* __CLI_H */