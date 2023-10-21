/**
 * \file
 * \brief Заголовочный файл, содержащий класс Serial для общения с устройствами по последовательному интерфейсу
 *
 * \authors Александр Горбунов
 * \date 11 марта 2023
 */

#ifndef SMART_ROAD_SERIAL_HPP
#define SMART_ROAD_SERIAL_HPP

#include <windows.h>
#include <iostream>

#include "utils.hpp"

/// Стандартный размер байта
#define BYTE_SIZE   8

/// Стандартная скорость передачи данных
#define DEFAULT_BAUD_RATE   115200

/// Нулевой символ
#define SYMBOL_NULL     '\0'
/// Символ возврата каретки
#define SYMBOL_CR       '\r'
/// Символ переноса строки
#define SYMBOL_LF       '\n'

/// Возвращаемый код при успешной операции
#define SERIAL_OK       0
/// Возвращаемый код при невыполненной операции
#define SERIAL_ERROR    1

/** Структура настроек для создания подключения */
struct port_config {
    DWORD baud_rate;    ///<Скорость передачи данных
    DWORD byte_size;    ///<Размер байта
    DWORD stop_bits;    ///<Количество стоп-битов
    DWORD parity;       ///<Бит чётности
};

/**
 * \brief Объект для общения с устройствами по последовательному интерфейсу
 *
 * Объект содержит в себе конструкторы для инициализации подключения и функции для отправки и чтения
 * данных типа byte_t и u_byte_t
 */
class Serial {

private:
    HANDLE h_serial;

public:
    /** \brief Стандартный конструктор
     *
     * **Пример**
     * \code
     * Serial data_bus;
     * \endcode
     */
    Serial() = default;

    /**
     * \brief Конструктор с одним аргументом - портом устройства.
     *
     * Создаёт подключение к устройству со следующими параметрами:
     * - baud_rate = 115200
     * - byte_size = 8
     * - stop_bits = ONESTOPBIT
     * - parity = NOPARITY
     *
     * \param [in] address Адрес устройства, к которому требуется подключиться
     *
     * **Пример**
     * \code
     * Serial data_bus("COM1");
     * \endcode
     */
    explicit Serial(LPTSTR address) {
        h_serial = ::CreateFile(
                address,
                GENERIC_READ | GENERIC_WRITE,
                0,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr);

        if (h_serial == INVALID_HANDLE_VALUE) {
            printf("Can't open port %s\n", address);
            return;
        }

        DCB dcbSerialParameters = {0};

        dcbSerialParameters.DCBlength = sizeof dcbSerialParameters;
        dcbSerialParameters.BaudRate =  DEFAULT_BAUD_RATE;
        dcbSerialParameters.ByteSize =  BYTE_SIZE;
        dcbSerialParameters.StopBits =  ONESTOPBIT;
        dcbSerialParameters.Parity =    NOPARITY;

        SetCommState(h_serial, &dcbSerialParameters);
    }

    /**
     * \brief Конструктор с двумя аргументами - портом устройства и настройками соединения.
     *
     * Создаёт подключение к устройству с параметрами, которые
     * передаются в структуре типа port_config.
     *
     * \param [in] address Адрес устройства, к которому требуется подключиться
     * \param [in] config Структура, содержащая в себе настройки для подключения
     *
     * **Пример**
     * \code
     * port_config config;
     * config.baud_rate = BAUD_115200;
     * config.byte_size = 8;
     * config.stop_bits = ONESTOPBIT;
     * config.parity = NOPARITY;
     *
     * Serial data_bus("COM1", config);
     * \endcode
     */
    Serial(LPTSTR address, port_config config) {
        h_serial = ::CreateFile(
                address,
                GENERIC_READ | GENERIC_WRITE,
                0,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr);

        if (h_serial == INVALID_HANDLE_VALUE) {
            printf("Can't open port %s\n", address);
            return;
        }

        DCB dcbSerialParameters = {0};

        dcbSerialParameters.DCBlength = sizeof dcbSerialParameters;
        dcbSerialParameters.BaudRate =  config.baud_rate;
        dcbSerialParameters.ByteSize =  config.byte_size;
        dcbSerialParameters.StopBits =  config.stop_bits;
        dcbSerialParameters.Parity =    config.parity;

        SetCommState(h_serial, &dcbSerialParameters);
    }

    /**
     * \brief Отправка одного байта на устройство.
     *
     * Отправляет на подключенное устройство один байт типа byte_t
     *
     * \param [in] data Отправляемый байт
     * \return Возвращает SERIAL_OK, если байт был успешно передан. В противнм случае - SERIAL_ERROR.
     */
    int write_byte(byte_t data) {
        DWORD dw_size = 1;
        DWORD dw_bytes_written;

        WriteFile(
                h_serial,
                &data,
                dw_size,
                &dw_bytes_written,
                nullptr);

        if (dw_size == dw_bytes_written) {
            return SERIAL_OK;
        } else {
            return SERIAL_ERROR;
        }
    }

    /**
     * \brief Отправка массива байтов на устройство.
     *
     * Отправляет на подключенное устройство массив байтов типа byte_t
     *
     * \param [in] data Отправляемый массив
     * \param [in] length Размер отправляемого массива
     * \return Возвращает SERIAL_OK, если массив был успешно передан. В противнм случае - SERIAL_ERROR.
     */
    int write_bytes(byte_t *data, size_t length) {
        DWORD dw_bytes_written;

        WriteFile(
                h_serial,
                data,
                length,
                &dw_bytes_written,
                nullptr);

        if (length == dw_bytes_written) {
            return SERIAL_OK;
        } else {
            return SERIAL_ERROR;
        }
    }

    /**
     * \brief Отправка байта на устройство.
     *
     * Отправляет на подключенное устройство один байт типа u_byte_t
     *
     * \param [in] data Отправляемый байт
     * \return Возвращает SERIAL_OK, если байт был успешно передан. В противнм случае - SERIAL_ERROR.
     */
    int write_u_byte(u_byte_t data) {
        DWORD dw_size = 1;
        DWORD dw_bytes_written;

        WriteFile(
                h_serial,
                &data,
                dw_size,
                &dw_bytes_written,
                nullptr);

        if (dw_size == dw_bytes_written) {
            return SERIAL_OK;
        } else {
            return SERIAL_ERROR;
        }
    }

    /**
     * \brief Отправка массива байтов на устройство.
     *
     * Отправляет на подключенное устройство массив байтов типа u_byte_t
     *
     * \param [in] data Отправляемый массив
     * \param [in] length Размер отправляемого массива
     * \return Возвращает SERIAL_OK, если массив был успешно передан. В противнм случае - SERIAL_ERROR.
     */
    int write_u_bytes(u_byte_t *data, size_t length) {
        DWORD dw_bytes_written;

        WriteFile(
                h_serial,
                data,
                length,
                &dw_bytes_written,
                nullptr);

        if (length == dw_bytes_written) {
            return SERIAL_OK;
        } else {
            return SERIAL_ERROR;
        }
    }

    /**
     * \brief Чтение одного байта.
     *
     * Считывает один байт типа byte_t, отправленный устройством.
     *
     * \return Возвращает байт типа byte_t.
     */
    byte_t read_byte() {
        DWORD size;
        byte_t received_byte;

        ReadFile(
                h_serial,
                &received_byte,
                1,
                &size,
                nullptr);

        if (size > 0) {
            return received_byte;
        }
    }

    /**
     * \brief Чтение массива байтов.
     *
     * Считывает массив байтов типа byte_t, отправленный устройством.
     *
     * \param [out] buffer Указатель на буфер, в который записываются принятые байты.
     * \param [in] buffer_length Размер буфера
     */
    void read_bytes(byte_t *buffer, size_t buffer_length) {
        byte_t received_byte;

        for (size_t pos = 0; pos < buffer_length; ++pos) {
            received_byte = read_byte();
            buffer[pos] = received_byte;
        }
    }

    /**
     * \brief Чтение одного байта.
     *
     * Считывает один байт типа u_byte_t, отправленный устройством.
     *
     * \return Возвращает байт типа u_byte_t.
     */
    u_byte_t read_u_byte() {
        DWORD size;
        u_byte_t received_byte;

        ReadFile(
                h_serial,
                &received_byte,
                1,
                &size,
                nullptr);

        if (size > 0) {
            return received_byte;
        }
    }

    /**
     * \brief Чтение массива байтов.
     *
     * Считывает массив байтов типа u_byte_t, отправленный устройством.
     *
     * \param [out] buffer Указатель на буфер, в который записываются принятые байты.
     * \param [in] buffer_length Размер буфера
     */
    void read_u_bytes(u_byte_t *buffer, size_t buffer_length) {
        u_byte_t received_byte;

        for (size_t pos = 0; pos < buffer_length; ++pos) {
            received_byte = read_u_byte();
            buffer[pos] = received_byte;
        }
    }
};


#endif //SMART_ROAD_SERIAL_HPP
