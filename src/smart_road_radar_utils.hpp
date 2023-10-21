/**
 * \file
 * \brief Заголовочный файл, содержащий вспомогательные константы, структуры и методы для работы SmartRoadRadar
 *
 * \authors Александр Горбунов
 * \date 11 марта 2023
 */

#ifndef SMART_ROAD_SMART_ROAD_RADAR_UTILS_HPP
#define SMART_ROAD_SMART_ROAD_RADAR_UTILS_HPP

#include "serial.hpp"

/// Возвращаемое значение при успешно выполненном действии
#define SMART_ROAD_RADAR_OK     0
/// Возвращаемое значение при невыполненном действии
#define SMART_ROAD_RADAR_ERROR  1

/// Байт в пакете данных при успешном действии
#define SUCCESS     0x0A
/// Байт в пакете данных, если действие не было выполнено
#define FAILURE     0xFF

/// Команда запроса версии ПО радара
#define CMD_REQUEST_VERSION     0x10
/// Команда ответа радара на запрос версии ПО
#define CMD_READ_VERSION        0x11

/// Команда установки параметров
#define CMD_SET_PARAMETERS      0x36
/// Команда запроса параметров
#define CMD_GET_PARAMETERS      0x37

/// Команда ответа радара на запрос параметров
#define CMD_READ_PARAMETERS     0x38

/// Команда установки максимального числа целей
#define CMD_SET_TARGET_NUM      0x12

/// Команда ответа радара на действия
#define CMD_READ_STATUS         0x50

/// Команда считывания данных от радара
#define CMD_READ_TARGET_DATA    0x42

/// Команда запуска передачи
#define CMD_ENABLE_TRANSMIT     0x20
/// Команда остановки передачи
#define CMD_DISABLE_TRANSMIT    0x21

/// Команда установки частоты передачи данных о целях
#define CMD_SET_DATA_FREQ       0x18

/// Команда установки передачи нулевых данных
#define CMD_SET_ZERO_REPORT     0x19

/// Первый байт заголовка пакета
#define HEADER_DATA_FRAME_1     0x55
/// Второй байт заголовка пакета
#define HEADER_DATA_FRAME_2     0xAA

/// Длина заголовка
#define LENGTH_HEADER           2
/// Длина числа, характеризующего размер данных
#define LENGTH_DATA_LENGTH      2
/// Длина команды
#define LENGTH_COMMAND_WORD     1
/// Длина контрольной суммы
#define LENGTH_CHECKSUM         1

/// Минимальная дальность
#define MIN_DISTANCE    0.0f
/// Максимальная дальность
#define MAX_DISTANCE    13.0f

/// Минимальная скорость
#define MIN_SPEED       0.0f
/// Максимальная скорость
#define MAX_SPEED       5.0f

/// Минимальный угол
#define MIN_ANGLE      -60.0f
/// Максимальный угол
#define MAX_ANGLE       60.0f

/// Левая граница
#define LEFT_BORDER    -6.0f
/// Правая граница
#define RIGHT_BORDER    6.0f

/// Масштабный коэффициент для пересчёта принятых данных о целях
#define SCALE   0.01f

/// Размер данных об одной цели
#define TARGET_DATA_BYTE_LENGTH 10

/// Количество байт, отвечающих за облако точек
#define TARGET_DATA_BYTE_OFFSET 2502

/// Частота передачи данных (1 раз в секунду)
#define DATA_FREQ_1     0x01
/// Частота передачи данных (2 раза в секунду)
#define DATA_FREQ_2     0x02
/// Частота передачи данных (3 раза в секунду)
#define DATA_FREQ_3     0x03
/// Частота передачи данных (4 раза в секунду)
#define DATA_FREQ_4     0x04
/// Частота передачи данных (5 раз в секунду)
#define DATA_FREQ_5     0x05
/// Частота передачи данных (10 раз в секунду)
#define DATA_FREQ_10    0x0A
/// Частота передачи данных (15 раз в секунду)
#define DATA_FREQ_15    0x0F
/// Частота передачи данных (20 раз в секунду)
#define DATA_FREQ_20    0x14

/// Передача нулевых данных
#define ZERO_DATA_REPORT        0x0A
/// Передача только ненулевых данных
#define ZERO_DATA_NOT_REPORT    0xFF

/// Структура кадра
struct frame {
    bool is_valid = false;      ///< Флаг целости кадра

    u_byte_t header[2] = {HEADER_DATA_FRAME_1, HEADER_DATA_FRAME_2};    ///< Заголовок

    union {
        u_byte_t b[2];
        u_short_t s;
    } data_length{};            ///< Размер данных

    u_byte_t word{};            ///< Командное слово
    u_byte_t *data = nullptr;   ///< Данные
    u_byte_t checksum{};        ///< Контрольная сумма
};

/// Структура параметров радара
struct parameters {

    union {
        float f = MIN_DISTANCE;
        u_byte_t b[4];
    } min_dist;                ///< Минимальное расстояние

    union {
        float f = MAX_DISTANCE;
        u_byte_t b[4];
    } max_dist;                 ///< Максимальное расстояние

    union {
        float f = MIN_SPEED;
        u_byte_t b[4];
    } min_speed;                ///< Минимальная скорость

    union {
        float f = MAX_SPEED;
        u_byte_t b[4];
    } max_speed;                ///< Максимальная скорость

    union {
        float f = MIN_ANGLE;
        u_byte_t b[4];
    } min_angle;                ///< Минимальный угол

    union {
        float f = MAX_ANGLE;
        u_byte_t b[4];
    } max_angle;                ///< Максимальный угол

    union {
        float f = LEFT_BORDER;
        u_byte_t b[4];
    } left_border;              ///< Левая граница

    union {
        float f = RIGHT_BORDER;
        u_byte_t b[4];
    } right_border;             ///< Правая граница
};

/// Структура данных о цели
struct target_data {
    u_byte_t num{};             ///< Номер цели

    float distance{};           ///< Расстояние
    float speed{};              ///< Скорость
    float angle{};              ///< Угол
    float snr{};                ///< Отношение сигнал-шум
};

/**
 * Метод для расчёта контрольной суммы кадра
 *
 * \param [in] target_frame Кадр, для которого рассчитывается контрольная сумма
 * \return Контрольная сумма в формате одного байта типа u_byte_t
 */
u_byte_t calculate_checksum(frame target_frame) {
    u_byte_t checksum = 0x00;

    checksum += target_frame.data_length.b[0];
    checksum += target_frame.data_length.b[1];

    checksum += target_frame.word;

    if (target_frame.data_length.s > 1) {
        for (unsigned short i = 0; i < target_frame.data_length.s - 1; ++i) {
            checksum += target_frame.data[i];
        }
    }

    return checksum;
}

/**
 * Метод для перевода двух байт типа u_byte_t в число типа float
 *
 * \param [in] u_bytes Массив, состоящий из двух байт
 * \return число типа float
 */
float u_byte_to_float(const u_byte_t u_bytes[2]) {
    // TODO: Проверить это место на реальном радаре
    int data = (u_bytes[1] << 8) | u_bytes[0];
    return (float) data * SCALE;
}

/**
 * Метод формирующий кадр, который состоит только из одного командного слова
 *
 * \param [in] word Командное слово
 * \return Готовый кадр с рассчитанной контрольной суммой
 */
frame configure_frame(u_byte_t word) {
    frame configured_frame{};

    configured_frame.data_length.s = (u_short_t) 1;
    configured_frame.word = word;

    configured_frame.checksum = calculate_checksum(configured_frame);

    return configured_frame;
}

/**
 * Метод формирующий кадр, который состоит из командного слова и одного байта данных
 *
 * \param [in] word Командное слово
 * \param [in] data Один байт данных
 * \return Готовый кадр с рассчитанной контрольной суммой
 */
frame configure_frame(u_byte_t word, u_byte_t data) {
    frame configured_frame{};

    configured_frame.data_length.s = (u_short_t) 2;
    configured_frame.word = word;

    configured_frame.data = new u_byte_t[1];
    configured_frame.data[0] = data;

    configured_frame.checksum = calculate_checksum(configured_frame);

    return configured_frame;
}

/**
 * Метод формирующий кадр, который состоит из командного слова и массива данных
 *
 * \param [in] word Командное слово
 * \param [in] data Указатель на массив данных
 * \param [in] length Размер массива данных
 * \return Готовый кадр с рассчитанной контрольной суммой
 */
frame configure_frame(u_byte_t word, u_byte_t *data, u_short_t length) {
    frame configured_frame{};

    configured_frame.data_length.s = (u_short_t) (length + 1);
    configured_frame.word = word;

    configured_frame.data = new u_byte_t[length];
    configured_frame.data = data;

    configured_frame.checksum = calculate_checksum(configured_frame);

    return configured_frame;
}

#endif //SMART_ROAD_SMART_ROAD_RADAR_UTILS_HPP
