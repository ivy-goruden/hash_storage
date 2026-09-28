#ifndef HASHER
#define HASHER

class Hasher{
    public:
        static unsigned long getHash(const char* str){
            unsigned long hash = 5381;
            int c;
            while ((c = (unsigned char)*str++)) {
                hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
            }
            return hash;
        }
};

#endif