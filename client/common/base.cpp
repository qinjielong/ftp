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

void replace_all(std::string &str, const std::string &old_value, const std::string &new_value)     
{     
        std::string::size_type pos(0);     
        while(true) {     
            	if((pos = str.find(old_value)) != std::string::npos) {      
                	str.replace(pos, old_value.length(), new_value);
	   	}else{
			break; 
		}    
        }     
}
     
std::string get_file_name(const char *path) {
	std::string temp = path;
	replace_all(temp, "\\", "/");

	std::string::size_type pos = temp.rfind('/');
        if (std::string::npos != pos){
                temp = temp.substr(pos + 1);
        }
	return temp;
}

bool is_dir(const char *path) {
	struct stat s_buf;
	stat(path, &s_buf);
 
	if(S_ISDIR(s_buf.st_mode)) {
		return true;
	}
 
	if(S_ISREG(s_buf.st_mode)) {
	}
	return false;	
}
