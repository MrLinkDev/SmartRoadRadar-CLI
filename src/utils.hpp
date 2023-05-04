#ifndef SMART_ROAD_UTILS_HPP
#define SMART_ROAD_UTILS_HPP

#define DELIMITER   " "

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

std::string get_first_item(std::string *line) {
    std::string item{};
    std::string delimiter = DELIMITER;

    if (line->find(delimiter) == std::string::npos) {
        item = *line;
        line->erase(0, line->length());
    } else {
        item = line->substr(0, line->find(delimiter));
        line->erase(0, line->find(delimiter) + delimiter.length());
    }

    return item;
}

std::string get_first_item(std::string *line, std::string *delimiter) {
    std::string item{};

    if (line->find(*delimiter) == std::string::npos) {
        item = *line;
        line->erase(0, line->length());
    } else {
        item = line->substr(0, line->find(*delimiter));
        line->erase(0, line->find(*delimiter) + delimiter->length());
    }

    return item;
}


#endif //SMART_ROAD_UTILS_HPP
