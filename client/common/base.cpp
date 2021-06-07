#include "base.h"

#include <iostream>

int create_directory(const std::string &path)
{
        int len = path.length();
        if (len <= 0 || len > FILE_NAME_MAX) {
                std::cout << "path length error!" << std::endl;
                return -1;
        }

        char temp_path[FILE_NAME_MAX] = {0};
        for(int i = 0; i < len; i++) {
                temp_path[i] = path[i];
                if (temp_path[i] == '\\' || temp_path[i] == '/') {
                        if (file_exists(temp_path)) {
                                continue;
                        }

                        int result = mkdir(temp_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                        if (0 != result) {
                                std::cout << "mkdir err " << temp_path << std::endl;
                                return -1;
                        }
                }
        }

        return 0;
}

