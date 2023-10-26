/**
 * \file
 * \brief Заголовочный файл, содержащий класс SmartRoadRadar для взаимодействия с радаром по протоколу
 *
 * \authors Александр Горбунов
 * \date 11 марта 2023
 */

#ifndef SMART_ROAD_SMART_ROAD_RADAR_HPP
#define SMART_ROAD_SMART_ROAD_RADAR_HPP

#include "smart_road_radar_utils.hpp"

/**
 * \brief Объект для взаимодействия с радаром
 *
 * Объект содержит в себе конструкторы для инициализации подключения к радару, функции чтения и отправки кадров
 * в радар, также, содержит функции для отправки определённых протоколом команд.
 */
class SmartRoadRadar {

private:
    /// Объект для подключения к радару по последовательному интерфейсу
    Serial data_bus{};


    /**
     * \brief Чтение данных с радара.
     * Читает данные, которые отправляет радар и формирует из них кадр
     *
     * \return Возвращает сформированный кадр
     */
    frame read_frame() {
        frame received_frame{};

        /// Ожидание начала кадра
        while (data_bus.read_u_byte() != HEADER_DATA_FRAME_1);

        /// Если следующий байт данных не равен второму байту заголовка, то кадр считается невалидным
        if (data_bus.read_u_byte() != HEADER_DATA_FRAME_2) {
            received_frame.is_valid = false;
            return received_frame;
        }

        /// Чтение первого байта длины данных
        received_frame.data_length.b[0] = data_bus.read_u_byte();
        /// Чтение второго байта длины данных
        received_frame.data_length.b[1] = data_bus.read_u_byte();

        /// Чтение командного слова
        received_frame.word = data_bus.read_u_byte();

        if (received_frame.word == CMD_READ_TARGET_DATA) {
            /// Пропуск пустого байта
            data_bus.read_u_byte();

            if (received_frame.data_length.i > 1) {
                received_frame.data = new u_byte_t[received_frame.data_length.i - 3];

                /// Чтение данных
                data_bus.read_u_bytes(received_frame.data, received_frame.data_length.i - 3);
            }

            /// Пропуск пустого байта
            data_bus.read_u_byte();

            /// Чтение контрольной суммы
            received_frame.checksum = data_bus.read_u_byte();
        } else {
            if (received_frame.data_length.i > 1) {
                received_frame.data = new u_byte_t[received_frame.data_length.i - 1];

                /// Чтение данных
                data_bus.read_u_bytes(received_frame.data, received_frame.data_length.i - 1);
            }

            /// Чтение контрольной суммы
            received_frame.checksum = data_bus.read_u_byte();
        }

        u_byte_t calc_checksum = calculate_checksum(received_frame);

        if (calc_checksum == received_frame.checksum) {
            received_frame.is_valid = true;
        } else {
            received_frame.is_valid = false;
        }

        return received_frame;
    }

    /**
     * \brief Чтение данных с радара с требуемым командным словом.
     *
     * \param [in] expected_word Командное слово, ожидаемое в кадре
     * \return Возвращает кадр с требуемым командным словом
     */
    frame read_expected_frame(u_byte_t expected_word) {
        frame received_frame;
        int attempts = 10;

        do {
            received_frame = read_frame();
            --attempts;
        } while (received_frame.word != expected_word && attempts > 0);

        if (received_frame.word != expected_word && attempts == 0) {
            received_frame.is_valid = false;
        }

        return received_frame;
    }

    /**
     * \brief Чтение статуса.
     * Функция, которая ожидает кадр с командным словом чтения статуса от радара
     *
     * \return Если от радара пришёл статус SUCCESS, то возвращается код SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     */
    int read_status() {
        frame received_frame = read_expected_frame(CMD_READ_STATUS);

        if (received_frame.is_valid) {
            if (received_frame.data[0] == SUCCESS) {
                return SMART_ROAD_RADAR_OK;
            } else {
                return SMART_ROAD_RADAR_ERROR;
            }
        } else {
            return SMART_ROAD_RADAR_ERROR;
        }
    }

    /**
     * \brief Отправка кадра.
     * Из кадра, передаваемого в качестве аргумента, формируется массив
     * типа u_byte_t, который затем передаётся на устройство.
     *
     * \param [in] target_frame Кадр, который требуется отправить
     * \return Возвращает SERIAL_OK, если данные были успешно отправлены. В противном случае - SERIAL_ERROR
     */
    int write_frame(frame target_frame) {
        int packet_length =
                LENGTH_HEADER +
                LENGTH_DATA_LENGTH +
                LENGTH_COMMAND_WORD +
                (target_frame.data_length.i - 1) +
                LENGTH_CHECKSUM;

        u_byte_t packet[packet_length];

        packet[0] = target_frame.header[0];
        packet[1] = target_frame.header[1];

        packet[2] = target_frame.data_length.b[0];
        packet[3] = target_frame.data_length.b[1];

        packet[4] = target_frame.word;

        if (target_frame.data != nullptr) {
            for (int i = 5; i < packet_length - 1; ++i) {
                packet[i] = target_frame.data[i - 5];
            }
        }

        packet[packet_length - 1] = target_frame.checksum;

        int result = data_bus.write_u_bytes(packet, packet_length);
        return result;
    }

public:
    /**
     * \brief Стандартный конструктор
     *
     * **Пример**
     * \code
     * SmartRoadRadar radar;
     * \endcode
     */
    SmartRoadRadar() = default;

    /**
     * \brief Конструктор, в который передаётся порт радара.
     *
     * Создаётся подключение к радару со следующими параметрами:
     * - baud_rate = 115200
     * - byte_size = 8
     * - stop_bits = ONESTOPBIT
     * - parity = NOPARITY
     *
     * \param [in] address Адрес радара
     *
     * **Пример**
     * \code
     * SmartRoadRadar radar("COM1");
     * \endcode
     */
    explicit SmartRoadRadar(LPTSTR address) {
        port_config config{};

        config.baud_rate = 115200;
        config.byte_size = 8;
        config.stop_bits = ONESTOPBIT;
        config.parity = NOPARITY;

        data_bus = Serial(address, config);
    }

    /**
     * \brief Конструктор, в который передаётся порт радара и настройки подключения.
     *
     * Создаёт подключение к радару с параметрами, которые передаются в структуре типа port_config.
     *
     * \param [in] address Адрес радара
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
     * SmartRoadRadar radar("COM1", config);
     * \endcode
     */
    SmartRoadRadar(LPTSTR address, port_config config) {
        data_bus = Serial(address, config);
    }

    /**
     * \brief Запрос версии ПО у радара.
     *
     * \param [out] version_buffer Указатель на буфер типа u_byte_t[3].
     * \return Если запрос выполнен успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * u_byte_t version_buffer[3];
     *
     * if (radar.get_firmware_version(version_buffer) == SMART_ROAD_RADAR_OK) {
     *     printf("Firmware version: V%d.%d.%d\n",
     *             (int) version_buffer[0],
     *             (int) version_buffer[1],
     *             (int) version_buffer[2]);
     * } else {
     *     printf("Can't get firmware version from radar\n");
     * }
     * \endcode
     */
    virtual int get_firmware_version(u_byte_t *version_buffer) {
        frame target_frame = configure_frame(CMD_REQUEST_VERSION);
        write_frame(target_frame);

        frame received_frame = read_expected_frame(CMD_READ_VERSION);

        if (received_frame.is_valid) {
            version_buffer[0] = received_frame.data[0];
            version_buffer[1] = received_frame.data[1];
            version_buffer[2] = received_frame.data[2];

            return SMART_ROAD_RADAR_OK;
        } else {
            return SMART_ROAD_RADAR_ERROR;
        }
    }

    /**
     * \brief Изменение настроек радара.
     *
     * Желаемые настроки передаются в структуре parameters.
     *
     * \param [in] target_parameters Требуемые настройки радара
     * \return Если изменение настроек прошло успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * parameters target_parameters;
     * if (*args != "default") {
     *     target_parameters.min_dist.f     = 0.0f;
     *     target_parameters.max_dist.f     = 13.0f;
     *     target_parameters.min_speed.f    = 0.0f;
     *     target_parameters.max_speed.f    = 5.0f;
     *     target_parameters.min_angle.f    = -60.0f;
     *     target_parameters.max_angle.f    = 60.0f;
     *     target_parameters.left_border.f  = -6.0f;
     *     target_parameters.right_border.f = 6.0f;
     * }
     * if (radar.set_parameters(target_parameters) == SMART_ROAD_RADAR_OK) {
     *     printf("Parameters are loaded\n");
     * } else {
     *     printf("Can't load parameters into radar\n");
     * }
     * \endcode
     */
    virtual int set_parameters(parameters target_parameters) {
        u_short_t length = sizeof target_parameters;
        u_byte_t data[length];

        data[0]  = target_parameters.min_dist.b[0];
        data[1]  = target_parameters.min_dist.b[1];
        data[2]  = target_parameters.min_dist.b[2];
        data[3]  = target_parameters.min_dist.b[3];
 
        data[4]  = target_parameters.max_dist.b[0];
        data[5]  = target_parameters.max_dist.b[1];
        data[6]  = target_parameters.max_dist.b[2];
        data[7]  = target_parameters.max_dist.b[3];
 
        data[8]  = target_parameters.min_speed.b[0];
        data[9]  = target_parameters.min_speed.b[1];
        data[10] = target_parameters.min_speed.b[2];
        data[11] = target_parameters.min_speed.b[3];

        data[12] = target_parameters.max_speed.b[0];
        data[13] = target_parameters.max_speed.b[1];
        data[14] = target_parameters.max_speed.b[2];
        data[15] = target_parameters.max_speed.b[3];

        data[16] = target_parameters.min_angle.b[0];
        data[17] = target_parameters.min_angle.b[1];
        data[18] = target_parameters.min_angle.b[2];
        data[19] = target_parameters.min_angle.b[3];

        data[20] = target_parameters.max_angle.b[0];
        data[21] = target_parameters.max_angle.b[1];
        data[22] = target_parameters.max_angle.b[2];
        data[23] = target_parameters.max_angle.b[3];

        data[24] = target_parameters.left_border.b[0];
        data[25] = target_parameters.left_border.b[1];
        data[26] = target_parameters.left_border.b[2];
        data[27] = target_parameters.left_border.b[3];

        data[28] = target_parameters.right_border.b[0];
        data[29] = target_parameters.right_border.b[1];
        data[30] = target_parameters.right_border.b[2];
        data[31] = target_parameters.right_border.b[3];

        frame target_frame = configure_frame(CMD_SET_PARAMETERS, data, length);

        int attempts = 10;
        int result{};

        do {
            write_frame(target_frame);
            result = read_status();

            --attempts;
        } while (result != SMART_ROAD_RADAR_OK && attempts > 0);

        return result;
    }

    /**
     * \brief Чтение настроек с радара.
     *
     * При получении кадра данных от радара, функция выделяет из принятого кадра
     * настройки радара и записывает их в структуру по переданному указателю.
     *
     * \warning Приходящие от радара минимальное и максимальное расстояния перепутаны местами.
     * Также перепутаны местами и минимальная и максимальная скорости.
     *
     * \param [out] received_parameters Указатель на структуру, в которую будут записаны принятые значения
     * \return Если запрос настроек выполнен успешно, то возвращает SMART_ROAD_RADAR_OK/
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * parameters received_parameters;
     * if (radar.get_parameters(&received_parameters) == SMART_ROAD_RADAR_OK)
     *     printf("Min distance = %f\n", received_parameters.min_dist.f);
     *     printf("Max distance = %f\n", received_parameters.max_dist.f);
     *     printf("Min speed    = %f\n", received_parameters.min_speed.f);
     *     printf("Max speed    = %f\n", received_parameters.max_speed.f);
     *     printf("Min angle    = %f\n", received_parameters.min_angle.f);
     *     printf("max angle    = %f\n", received_parameters.max_angle.f);
     *     printf("Left border  = %f\n", received_parameters.left_border.f);
     *     printf("Right border = %f\n", received_parameters.right_border.f);
     * } else {
     *     printf("Can't get parameters from radar\n");
     * }
     * \endcode
     */
    virtual int get_parameters(parameters *received_parameters) {
        frame target_frame = configure_frame(CMD_GET_PARAMETERS);
        write_frame(target_frame);
        
        frame received_frame = read_expected_frame(CMD_READ_PARAMETERS);
        
        if (received_frame.is_valid) {
            received_parameters->min_dist.b[0] = received_frame.data[4];
            received_parameters->min_dist.b[1] = received_frame.data[5];
            received_parameters->min_dist.b[2] = received_frame.data[6];
            received_parameters->min_dist.b[3] = received_frame.data[7];

            received_parameters->max_dist.b[0] = received_frame.data[0];
            received_parameters->max_dist.b[1] = received_frame.data[1];
            received_parameters->max_dist.b[2] = received_frame.data[2];
            received_parameters->max_dist.b[3] = received_frame.data[3];

            received_parameters->min_speed.b[0] = received_frame.data[12];
            received_parameters->min_speed.b[1] = received_frame.data[13];
            received_parameters->min_speed.b[2] = received_frame.data[14];
            received_parameters->min_speed.b[3] = received_frame.data[15];

            received_parameters->max_speed.b[0] = received_frame.data[8];
            received_parameters->max_speed.b[1] = received_frame.data[9];
            received_parameters->max_speed.b[2] = received_frame.data[10];
            received_parameters->max_speed.b[3] = received_frame.data[11];

            received_parameters->min_angle.b[0] = received_frame.data[16];
            received_parameters->min_angle.b[1] = received_frame.data[17];
            received_parameters->min_angle.b[2] = received_frame.data[18];
            received_parameters->min_angle.b[3] = received_frame.data[19];

            received_parameters->max_angle.b[0] = received_frame.data[20];
            received_parameters->max_angle.b[1] = received_frame.data[21];
            received_parameters->max_angle.b[2] = received_frame.data[22];
            received_parameters->max_angle.b[3] = received_frame.data[23];

            received_parameters->left_border.b[0] = received_frame.data[24];
            received_parameters->left_border.b[1] = received_frame.data[25];
            received_parameters->left_border.b[2] = received_frame.data[26];
            received_parameters->left_border.b[3] = received_frame.data[27];

            received_parameters->right_border.b[0] = received_frame.data[28];
            received_parameters->right_border.b[1] = received_frame.data[29];
            received_parameters->right_border.b[2] = received_frame.data[30];
            received_parameters->right_border.b[3] = received_frame.data[31];

            return SMART_ROAD_RADAR_OK;
        } else {
            return SMART_ROAD_RADAR_ERROR;
        }
    }

    /**
     * \brief Установка числа целей.
     *
     * Функция, позволяющая задать количество целей, которые будут распознаваться радаром.
     *
     * \warning Возможно, не количество целей, а максимальный номер цели
     * \param [in] number Количество целей
     *
     * \return Если команда выполнена успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * u_byte_t target_num = std::stoi(num);
     * if (radar.set_target_number(target_num) == SMART_ROAD_RADAR_OK) {
     *     printf("Target number inserted\n");
     * } else {
     *     printf("Can't insert target number into radar\n");
     * }
     * \endcode
     */
    virtual int set_target_number(u_byte_t number) {
        frame target_frame = configure_frame(CMD_SET_TARGET_NUM, number);

        int attempts = 10;
        int result{};

        do {
            write_frame(target_frame);
            result = read_status();

            --attempts;
        } while (result != SMART_ROAD_RADAR_OK && attempts > 0);

        return result;
    }

    /**
     * \brief Чтение данных о целях.
     *
     * Функция преобразует полученные кадры в структуры target_data, которые записываются
     * в массив по переданному указателю
     *
     * \param [out] data Указатель на массив структур target_data
     *
     * \return Если чтение кадра выполнено успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * target_data data[35];
     *
     * printf(" # |  dist  |   speed  |  angle  \n");
     * printf("---------------------------------\n");
     *
     * if (radar.get_target_data(data) == SMART_ROAD_RADAR_OK) {
     *     for (int i = 0; i < target_count; ++i) {
     *         printf("\r%2d | %2.2f m | %2.2f m/i | %2.2f deg\n",
     *                data[i].num,
     *                data[i].distance,
     *                data[i].speed,
     *                data[i].angle);
     *         data[i] = target_data{};
     *     }
     * }
     * \endcode
     */
    virtual int get_target_data(target_data *data, int target_data_capacity) {
        frame received_frame;
        
        do {
            received_frame = read_expected_frame(CMD_READ_TARGET_DATA);
        } while (received_frame.data_length.i <= 1);
        
        if (received_frame.is_valid) {
            for (int pos = 0; pos < target_data_capacity; ++pos) {
                data[pos].num = received_frame.data[0 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET];

                data[pos].distance = u_byte_to_float(new u_byte_t[2] {
                        received_frame.data[2 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET],
                        received_frame.data[3 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET]
                });

                data[pos].speed = u_byte_to_float(new u_byte_t[2] {
                        received_frame.data[4 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET],
                        received_frame.data[5 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET]
                });

                data[pos].angle = u_byte_to_float(new u_byte_t[2] {
                        received_frame.data[6 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET],
                        received_frame.data[7 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET]
                });

                data[pos].snr =u_byte_to_float(new u_byte_t[2] {
                        received_frame.data[8 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET],
                        received_frame.data[9 + TARGET_DATA_BYTE_LENGTH * pos + TARGET_DATA_BYTE_OFFSET]
                });
            }

            return SMART_ROAD_RADAR_OK;
        } else {
            return SMART_ROAD_RADAR_ERROR;
        }
    }

    /**
     * \brief Запуск передачи данных.
     *
     * Функция отправляет запрос на радар о запуске передачи данных.
     *
     * \return Если запрос выполнен успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * if (radar.enable_data_transmit() == SMART_ROAD_RADAR_OK) {
     *     printf("Data transmit enabled\n");
     * } else {
     *     printf("Can't enable data transmit\n");
     * }
     * \endcode
     */
    virtual int enable_data_transmit() {
        frame target_frame = configure_frame(CMD_ENABLE_TRANSMIT);

        int attempts = 10;
        int result{};

        do {
            write_frame(target_frame);
            result = read_status();

            --attempts;
        } while (result != SMART_ROAD_RADAR_OK && attempts > 0);

        return result;
    }

    /**
     * \brief Остановка передачи данных.
     *
     * Функция отправляет запрос на радар об остановке передачи данных.
     *
     * \return Если запрос выполнен успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * if (radar.disable_data_transmit() == SMART_ROAD_RADAR_OK) {
     *     printf("Data transmit disabled\n");
     * } else {
     *     printf("Can't disable data transmit\n");
     * }
     * \endcode
     */
    virtual int disable_data_transmit() {
        frame target_frame = configure_frame(CMD_DISABLE_TRANSMIT);

        int attempts = 10;
        int result{};

        do {
            write_frame(target_frame);
            result = read_status();

            --attempts;
        } while (result != SMART_ROAD_RADAR_OK && attempts > 0);

        return result;
    }

    /**
     * \brief Установка частоты отправки данных.
     *
     * Функция устанавливает частоту передаваемы данных, которая передаётся в качестве аргумента.
     * Возможные варианты частоты:
     * - DATA_FREQ_1 - один раз в секунду
     * - DATA_FREQ_2 - два раза в секунду
     * - DATA_FREQ_3 - три раза в секунду
     * - DATA_FREQ_4 - четыре раза в секунду
     * - DATA_FREQ_5 - пять раз в секунду
     * - DATA_FREQ_10 - десять раз в секунду
     * - DATA_FREQ_15 - пятнадцать раз в секунду
     * - DATA_FREQ_20 - двадцать раз в секунду
     *
     * \param [in] freq Частота передаваемых данных
     *
     * \return Если запрос выполнен успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     *  u_byte_t data_freq = DATA_FREQ_1
     *
     *  if (radar.set_data_transmit_freq(data_freq) == SMART_ROAD_RADAR_OK) {
     *      printf("Frequency value uploaded into radar\n");
     *  } else {
     *      printf("Can't upload frequency value\n");
     *  }
     * \endcode
     */
    virtual int set_data_transmit_freq(u_byte_t freq) {
        frame target_frame = configure_frame(CMD_SET_DATA_FREQ, freq);

        int attempts = 10;
        int result{};

        do {
            write_frame(target_frame);
            result = read_status();

            --attempts;
        } while (result != SMART_ROAD_RADAR_OK && attempts > 0);

        return result;
    }

    /**
     * \brief Включение передачи нулевых данных.
     *
     * Функция позволяет включить передачу нулевых данных.
     *
     * \return Если запрос выполнен успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * if (radar.enable_zero_data_reporting() == SMART_ROAD_RADAR_OK) {
     *     printf("Zero data reporting enabled\n");
     * } else {
     *     printf("Can't enable zero data reporting\n");
     * }
     * \endcode
     */
    virtual int enable_zero_data_reporting() {
        frame target_frame = configure_frame(CMD_SET_ZERO_REPORT, ZERO_DATA_REPORT);

        int attempts = 10;
        int result{};

        do {
            write_frame(target_frame);
            result = read_status();

            --attempts;
        } while (result != SMART_ROAD_RADAR_OK && attempts > 0);

        return result;
    }

    /**
     * \brief Выключение передачи нулевых данных.
     *
     * Функция позволяет выключить передачу нулевых данных.
     *
     * \return Если запрос выполнен успешно, то возвращает SMART_ROAD_RADAR_OK.
     * В противном случае - SMART_ROAD_RADAR_ERROR.
     *
     * **Пример**
     * \code
     * if (radar.disable_zero_data_reporting() == SMART_ROAD_RADAR_OK) {
     *     printf("Zero data reporting disabled\n");
     * } else {
     *     printf("Can't disable zero data reporting\n");
     * }
     * \endcode
     */
    virtual int disable_zero_data_reporting() {
        frame target_frame = configure_frame(CMD_SET_ZERO_REPORT, ZERO_DATA_NOT_REPORT);

        int attempts = 10;
        int result{};

        do {
            write_frame(target_frame);
            result = read_status();

            --attempts;
        } while (result != SMART_ROAD_RADAR_OK && attempts > 0);

        return result;
    }
};


#endif //SMART_ROAD_SMART_ROAD_RADAR_HPP
