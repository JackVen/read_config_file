#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <stdexcept>

enum class SensorState
{
    UNINITIALIZED,
    READY,
    ERROR
};

enum class TemperatureUnit
{
    CELSIUS,
    FAHRENHEIT,
    KELVIN
};

struct SensorConfig
{
    std::string sensorModel = "STM32F031K6T";
    double minOperationalTemp = -40.0; // in Celsius
    double maxOperationalTemp = 105.0; // in Celsius
    uint16_t calTs1Raw = 1750;         // Factory calibration ADC raw value at TS1
    uint16_t calTs2Raw = 950;          // Factory calibration ADC raw value at TS2
    double calTs1Temp = 30.0;          // Calibration temperature 1 in Celsius
    double calTs2Temp = 110.0;         // Calibration temperature 2 in Celsius
    uint8_t adcResolution = 12;        // 12-bit ADC -> max 4095
};

class TemperatureSensor
{
private:
    SensorConfig config_;
    SensorState state_{SensorState::UNINITIALIZED};
    
    std::vector<double> readingHistory_; // Temperatures stored in Celsius
    std::optional<double> highTempThreshold_;
    std::optional<double> lowTempThreshold_;
    
    bool highTempAlarmActive_{false};
    bool lowTempAlarmActive_{false};

    uint16_t getMaxAdcValue() const;
    void evaluateAlarms(double currentTemp);

public:
    // Constructors
    TemperatureSensor();
    explicit TemperatureSensor(const SensorConfig &config);

    // Initialization & Configuration
    bool initialize(const SensorConfig &config);
    SensorState getState() const;
    bool isInitialized() const;
    const SensorConfig &getConfig() const;

    // Unit Conversion Utilities (Static)
    static double celsiusToFahrenheit(double celsius);
    static double celsiusToKelvin(double celsius);
    static double fahrenheitToCelsius(double fahrenheit);
    static double kelvinToCelsius(double kelvin);

    // ADC Conversion
    double rawToCelsius(uint16_t rawAdc) const;

    // Reading Operations
    double processReading(uint16_t rawAdc);
    double getLatestTemperature(TemperatureUnit unit = TemperatureUnit::CELSIUS) const;

    // Operational checks
    bool isWithinOperationalRange(double tempCelsius) const;
    bool isOverheating() const;
    bool isFreezing() const;

    // Thresholds & Alarms
    void setHighTempThreshold(double thresholdCelsius);
    void setLowTempThreshold(double thresholdCelsius);
    void clearThresholds();
    std::optional<double> getHighTempThreshold() const;
    std::optional<double> getLowTempThreshold() const;
    bool isHighTempAlarmActive() const;
    bool isLowTempAlarmActive() const;

    // History & Statistics
    const std::vector<double> &getReadingHistory() const;
    std::size_t getReadingCount() const;
    double getAverageTemperature() const;
    double getMaxRecordedTemperature() const;
    double getMinRecordedTemperature() const;
    void clearHistory();

    // Reset
    void reset();
};
