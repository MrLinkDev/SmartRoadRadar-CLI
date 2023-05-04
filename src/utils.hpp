#ifndef SMART_ROAD_UTILS_HPP
#define SMART_ROAD_UTILS_HPP

typedef unsigned long long int size_t;

typedef char byte_t;
typedef unsigned char u_byte_t;

typedef unsigned short u_short_t;

void append(char *dest, const size_t *length, char symbol) {
    length += 1;
    dest[*length] = symbol;
}

void append(unsigned char *dest, const size_t *length, unsigned char symbol) {
    length += 1;
    dest[*length] = symbol;
}


#endif //SMART_ROAD_UTILS_HPP
