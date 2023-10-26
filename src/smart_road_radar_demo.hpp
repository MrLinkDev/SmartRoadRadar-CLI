/**
 * \file
 * \brief Заголовочный файл, содержащий класс SmartRoadRadarDemo для эмуляции взаимодействия с радаром по протоколу
 *
 * \authors Александр Горбунов
 * \date 21 октября 2023
 */

#ifndef SMART_ROAD_SMART_ROAD_RADAR_DEMO_HPP
#define SMART_ROAD_SMART_ROAD_RADAR_DEMO_HPP

#include <random>
#include <thread>
#include <chrono>
#include "smart_road_radar.hpp"
#include "smart_road_radar_utils.hpp"

using namespace std::chrono_literals;

typedef std::uniform_int_distribution<std::mt19937::result_type> rnd_int;
typedef std::uniform_real_distribution<float> rnd_float;

/**
 * \brief Объект для эмуляции взаимодействия с радаром
 *
 * Объект содержит в себе конструкторы для эмуляции инициализации подключения к радару, функции чтения и отправки кадров
 * в радар, также, содержит функции для отправки определённых протоколом команд.
 */
class SmartRoadRadarDemo : public SmartRoadRadar {

private:
    struct DemoParameters {
        float min_distance  = MIN_DISTANCE;
        float max_distance  = MAX_DISTANCE;

        float min_speed     = MIN_SPEED;
        float max_speed     = MAX_SPEED;

        float min_angle     = MIN_ANGLE;
        float max_angle     = MAX_ANGLE;

        float left_border   = LEFT_BORDER;
        float right_border  = RIGHT_BORDER;

        u_byte_t sleep_time = DATA_FREQ_1;
    } demo_parameters;

    std::random_device dev;
    std::mt19937 gen;

    rnd_int rnd_num;

    rnd_float rnd_dist;
    rnd_float rnd_speed;
    rnd_float rnd_angle;

    void init_rnd_float() {
        rnd_dist = rnd_float(demo_parameters.min_distance, demo_parameters.max_distance);
        rnd_speed = rnd_float(demo_parameters.min_speed, demo_parameters.max_speed);
        rnd_angle = rnd_float(demo_parameters.min_angle, demo_parameters.max_angle);
    }

public:
    /**
     * \brief Стандартный конструктор
     *
     * **Пример**
     * \code
     * SmartRoadRadarDemo radar;
     * \endcode
     */
    SmartRoadRadarDemo() {
        gen = std::mt19937(dev());

        rnd_num = rnd_int(1, 35);
        init_rnd_float();
    };

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
    int get_firmware_version(u_byte_t *version_buffer) override {
        version_buffer[0] = 255;
        version_buffer[1] = 255;
        version_buffer[2] = 255;

        return SMART_ROAD_RADAR_OK;
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
    int set_parameters(parameters target_parameters) override {
        demo_parameters.min_distance = target_parameters.min_dist.f;
        demo_parameters.max_distance = target_parameters.max_dist.f;

        demo_parameters.min_speed = target_parameters.min_speed.f;
        demo_parameters.max_speed = target_parameters.max_speed.f;

        demo_parameters.min_angle = target_parameters.min_angle.f;
        demo_parameters.max_angle = target_parameters.max_angle.f;

        demo_parameters.left_border = target_parameters.left_border.f;
        demo_parameters.right_border = target_parameters.right_border.f;

        init_rnd_float();

        return SMART_ROAD_RADAR_OK;
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
    int get_parameters(parameters *received_parameters) override {
        received_parameters->min_dist.f = demo_parameters.min_distance;
        received_parameters->max_dist.f = demo_parameters.max_distance;

        received_parameters->min_speed.f = demo_parameters.min_speed;
        received_parameters->max_speed.f = demo_parameters.max_speed;

        received_parameters->min_angle.f = demo_parameters.min_angle;
        received_parameters->max_angle.f = demo_parameters.max_angle;

        received_parameters->left_border.f = demo_parameters.left_border;
        received_parameters->right_border.f = demo_parameters.right_border;

        return SMART_ROAD_RADAR_OK;
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
    int set_target_number(u_byte_t number) override {
        return SMART_ROAD_RADAR_OK;
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
    int get_target_data(target_data *data, int target_data_capacity) override {
        /// Задержка для эмуляции заданной скорости передачи данных
        std::this_thread::sleep_for((1000ms / (int) demo_parameters.sleep_time));

        for (int pos = 0; pos < target_data_capacity; ++pos) {

            data[pos].num = rnd_num(gen);

            data[pos].distance = rnd_dist(gen);
            data[pos].speed = rnd_speed(gen);
            data[pos].angle = rnd_angle(gen);

            data[pos].snr = 0;
        }

        return SMART_ROAD_RADAR_OK;
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
    int enable_data_transmit() override {
        return SMART_ROAD_RADAR_OK;
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
    int disable_data_transmit() override {
        return SMART_ROAD_RADAR_OK;
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
    int set_data_transmit_freq(u_byte_t freq) override {
        demo_parameters.sleep_time = freq;

        return SMART_ROAD_RADAR_OK;
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
    int enable_zero_data_reporting() override {
        return SMART_ROAD_RADAR_OK;
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
    int disable_zero_data_reporting() override {
        return SMART_ROAD_RADAR_OK;
    }
};


#endif //SMART_ROAD_SMART_ROAD_RADAR_DEMO_HPP
