/**
 * \file
 * \brief Заголовочный файл, содержащий определения требуемых типов данных и функции для обработки строк
 *
 * \authors Александр Горбунов
 * \date 11 марта 2023
 */

#ifndef SMART_ROAD_UTILS_HPP
#define SMART_ROAD_UTILS_HPP

/// Стандартный делитель
#define DELIMITER   " "

/// Определение типа size_t на случай, если его нет по-умолчанию
typedef unsigned long long int size_t;

/// Тип данных byte_t размером 1 байт
typedef char byte_t;
/// Тип данных u_byte_t размером 1 байт
typedef unsigned char u_byte_t;

/// Тип данных u_short_t размером 2 байта
typedef unsigned short u_short_t;

/**
 * Добавляет символ типа char (byte_t) в строку
 *
 * \param [in,out] dest Строка, в которую будет добавлен символ
 * \param [in] length Длина строки
 * \param [in] symbol Добавляемый символ
 */
void append(char *dest, const size_t *length, char symbol) {
    length += 1;
    dest[*length] = symbol;
}

/**
 * Добавляет символ типа unsigned char (u_byte_t) в строку
 *
 * \param [in,out] dest Строка, в которую будет добавлен символ
 * \param [in] length Длина строки
 * \param [in] symbol Добавляемый символ
 */
void append(unsigned char *dest, const size_t *length, unsigned char symbol) {
    length += 1;
    dest[*length] = symbol;
}

/**
 * Из принимаемой строки выделяет содержимое до первого делителя
 *
 * \param [in,out] dest Строка, из которой будет выделено содержимое до делителя (пробела).
 * \return Выделенное содержимое до делителя (пробела)
 */
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

/**
 * Из принимаемой строки выделяет содержимое до первого делителя
 *
 * \param [in,out] dest Строка, из которой будет выделено содержимое до делителя.
 * \param [in] delimiter Делитель, до которого выделяется содержимое.
 * \return Выделенное содержимое до делителя.
 */
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
