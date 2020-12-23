
#ifndef LAB4_CONFIG_H
#define LAB4_CONFIG_H

#define MAX_STR 128

#define BUF_COUNT (/*8*/ + 3 + 0 + 7 + 1 + 6) // 17

#define EXE_FILENAME "./main.exe"
#define MAPPING_FILE "./map_file.txt"
#define MAPPING_NAME "mapping"
#define SEMAPHORE_READER_NAME "reader_sem"
#define SEMAPHORE_WRITER_NAME "writer_sem"
#define LOG_READER_FILENAME "./lab4_reader.log"
#define LOG_WRITER_FILENAME "./lab4_writer.log"
#define LOG_READER_MUTEX_NAME "log_reader_mut"
#define LOG_WRITER_MUTEX_NAME "log_writer_mut"

#endif // !LAB4_CONFIG_H
