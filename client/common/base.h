#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>

#define FILE_NAME_MAX 128

inline bool file_exists(const char *name) {
    return ( access(name, F_OK ) != -1 );
}

inline int pwd()
{
	char temp[FILE_NAME_MAX] = {0};
	getcwd(temp, sizeof(temp)-1);
	return 0;
}

inline int change_dir(const char *path)
{
	pwd();
	if(chdir(path)<0){
		return -1;
	}
	pwd();
	return 0;
}

int create_directory(const std::string &path);

void replace_all(std::string &str, const std::string &old_value, const std::string &new_value);

std::string get_file_name(const char *path);

bool is_dir(const char *path);

size_t get_file_size(const char *path);
