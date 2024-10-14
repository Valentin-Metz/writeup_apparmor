cat > preload.cpp << EOF
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/apparmor.h>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


void exit(int status) {
        unsetenv("LD_PRELOAD");
        //setgid(0);
        //setuid(0);
        
        std::string line;
        std::vector<std::string> lines;

        
        aa_change_hat("untrusted", 0x961101); 
        system("echo Hello from untrusted area!"); // does not print
        
        std::ifstream input("/opt/tpch/sf1/customer1.tbl");

        while (!input.eof()){
            std::getline(input,line);
            lines.push_back(line);
        }
        
        aa_change_hat(0, 0x961101);
        system("echo Hello from trusted area!");
        system("cat /opt/tpch/sf1/customer1.tbl"); // no permissions to open
        
        std::cerr << "Writing to stderr works!" << std::endl;
        

        for(size_t i = 0; i < lines.size(); ++i){
            std::cerr << lines[i] << std::endl;
        }
        
        fprintf(stderr, "Done\n");
}

EOF

g++ -fPIC -shared -o preload.so preload.cpp
LD_PRELOAD=/builds/Feuermagier/fde-22-bonusproject-1/build/release/preload.so /data/delivery/measure ./main
