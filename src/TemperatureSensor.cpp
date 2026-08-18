#include "TemperatureSensor.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

TemperatureSensor::TemperatureSensor()
    : state_(SensorState::UNINITIALIZED)
{
    Logger::log(LogLevel::DEBUG, "TemperatureSensor created (uninitialized)");
}

TemperatureSensor::TemperatureSensor(const SensorConfig &config)
{
    if (!initialize(config))
    {
        state_ = SensorState::ERROR;
        throw std::invalid_argument("Invalid sensor configuration parameters.");
    }
}

bool TemperatureSensor::initialize(const SensorConfig &config)
{
    Logger::log(LogLevel::INFO, "Initializing TemperatureSensor: " + config.sensorModel);

    // Validation of configuration
    if (config.minOperationalTemp >= config.maxOperationalTemp)
    {
        Logger::log(LogLevel::ERROR, "Config error: min operational temp must be less than max.");
        state_ = SensorState::ERROR;
        return false;
    }

    if (config.calTs1Raw == config.calTs2Raw)
    {
        Logger::log(LogLevel::ERROR, "Config error: calibration raw values cannot be equal.");
        state_ = SensorState::ERROR;
        return false;
    }

    if (config.calTs1Temp >= config.calTs2Temp)
    {
        Logger::log(LogLevel::ERROR, "Config error: calibration temp 1 must be less than temp 2.");
        state_ = SensorState::ERROR;
        return false;
    }

    if (config.adcResolution < 8 || config.adcResolution > 16)
    {
        Logger::log(LogLevel::ERROR, "Config error: ADC resolution must be between 8 and 16 bits.");
        state_ = SensorState::ERROR;
        return false;
    }

    config_ = config;
    state_ = SensorState::READY;
    readingHistory_.clear();
    highTempAlarmActive_ = false;
    lowTempAlarmActive_ = false;

    Logger::log(LogLevel::INFO, "TemperatureSensor initialized successfully.");
    return true;
}

SensorState TemperatureSensor::getState() const
{
    return state_;
}

bool TemperatureSensor::isInitialized() const
{
    return state_ == SensorState::READY;
}

const SensorConfig &TemperatureSensor::getConfig() const
{
    return config_;
}

uint16_t TemperatureSensor::getMaxAdcValue() const
{
    return static_cast<uint16_t>((1U << config_.adcResolution) - 1U);
}

// Unit Conversions
double TemperatureSensor::celsiusToFahrenheit(double celsius)
{
    return (celsius * 9.0 / 5.0) + 32.0;
}

double TemperatureSensor::celsiusToKelvin(double celsius)
{
    return celsius + 273.15;
}

double TemperatureSensor::fahrenheitToCelsius(double fahrenheit)
{
    return (fahrenheit - 32.0) * 5.0 / 9.0;
}

double TemperatureSensor::kelvinToCelsius(double kelvin)
{
    return kelvin - 273.15;
}

// ADC Conversion
double TemperatureSensor::rawToCelsius(uint16_t rawAdc) const
{
    if (state_ != SensorState::READY)
    {
        throw std::runtime_error("Cannot convert ADC: Sensor is not in READY state.");
    }

    if (rawAdc > getMaxAdcValue())
    {
        throw std::invalid_argument("ADC raw value exceeds maximum resolution limit.");
    }

    // Linear interpolation based on factory calibration values
    double slope = (config_.calTs2Temp - config_.calTs1Temp) /
                   static_cast<double>(static_cast<int32_t>(config_.calTs2Raw) - static_cast<int32_t>(config_.calTs1Raw));

    double temperature = slope * (static_cast<int32_t>(rawAdc) - static_cast<int32_t>(config_.calTs1Raw)) + config_.calTs1Temp;

    return temperature;
}

void TemperatureSensor::evaluateAlarms(double currentTemp)
{
    if (highTempThreshold_.has_value())
    {
        highTempAlarmActive_ = (currentTemp >= highTempThreshold_.value());
        if (highTempAlarmActive_)
        {
            Logger::log(LogLevel::WARNING, "High temperature alarm triggered: " + std::to_string(currentTemp));
        }
    }
    else
    {
        highTempAlarmActive_ = false;
    }

    if (lowTempThreshold_.has_value())
    {
        lowTempAlarmActive_ = (currentTemp <= lowTempThreshold_.value());
        if (lowTempAlarmActive_)
        {
            Logger::log(LogLevel::WARNING, "Low temperature alarm triggered: " + std::to_string(currentTemp));
        }
    }
    else
    {
        lowTempAlarmActive_ = false;
    }
}

double TemperatureSensor::processReading(uint16_t rawAdc)
{
    double tempCelsius = rawToCelsius(rawAdc);
    readingHistory_.push_back(tempCelsius);
    evaluateAlarms(tempCelsius);
    return tempCelsius;
}

double TemperatureSensor::getLatestTemperature(TemperatureUnit unit) const
{
    if (readingHistory_.empty())
    {
        throw std::runtime_error("No temperature readings available in history.");
    }

    double tempCelsius = readingHistory_.back();

    switch (unit)
    {
    case TemperatureUnit::FAHRENHEIT:
        return celsiusToFahrenheit(tempCelsius);
    case TemperatureUnit::KELVIN:
        return celsiusToKelvin(tempCelsius);
    case TemperatureUnit::CELSIUS:
    default:
        return tempCelsius;
    }
}

bool TemperatureSensor::isWithinOperationalRange(double tempCelsius) const
{
    return tempCelsius >= config_.minOperationalTemp && tempCelsius <= config_.maxOperationalTemp;
}

bool TemperatureSensor::isOverheating() const
{
    if (readingHistory_.empty())
    {
        return false;
    }
    return readingHistory_.back() > config_.maxOperationalTemp;
}

bool TemperatureSensor::isFreezing() const
{
    if (readingHistory_.empty())
    {
        return false;
    }
    return readingHistory_.back() <= 0.0;
}

// Thresholds & Alarms
void TemperatureSensor::setHighTempThreshold(double thresholdCelsius)
{
    highTempThreshold_ = thresholdCelsius;
    if (!readingHistory_.empty())
    {
        evaluateAlarms(readingHistory_.back());
    }
}

void TemperatureSensor::setLowTempThreshold(double thresholdCelsius)
{
    lowTempThreshold_ = thresholdCelsius;
    if (!readingHistory_.empty())
    {
        evaluateAlarms(readingHistory_.back());
    }
}

void TemperatureSensor::clearThresholds()
{
    highTempThreshold_.reset();
    lowTempThreshold_.reset();
    highTempAlarmActive_ = false;
    lowTempAlarmActive_ = false;
}

std::optional<double> TemperatureSensor::getHighTempThreshold() const
{
    return highTempThreshold_;
}

std::optional<double> TemperatureSensor::getLowTempThreshold() const
{
    return lowTempThreshold_;
}

bool TemperatureSensor::isHighTempAlarmActive() const
{
    return highTempAlarmActive_;
}

bool TemperatureSensor::isLowTempAlarmActive() const
{
    return lowTempAlarmActive_;
}

// History & Statistics
const std::vector<double> &TemperatureSensor::getReadingHistory() const
{
    return readingHistory_;
}

std::size_t TemperatureSensor::getReadingCount() const
{
    return readingHistory_.size();
}

double TemperatureSensor::getAverageTemperature() const
{
    if (readingHistory_.empty())
    {
        throw std::runtime_error("Cannot compute average: reading history is empty.");
    }

    double sum = std::accumulate(readingHistory_.begin(), readingHistory_.end(), 0.0);
    return sum / static_cast<double>(readingHistory_.size());
}

double TemperatureSensor::getMaxRecordedTemperature() const
{
    if (readingHistory_.empty())
    {
        throw std::runtime_error("Cannot get max temperature: reading history is empty.");
    }

    return *std::max_element(readingHistory_.begin(), readingHistory_.end());
}

double TemperatureSensor::getMinRecordedTemperature() const
{
    if (readingHistory_.empty())
    {
        throw std::runtime_error("Cannot get min temperature: reading history is empty.");
    }

    return *std::min_element(readingHistory_.begin(), readingHistory_.end());
}

void TemperatureSensor::clearHistory()
{
    readingHistory_.clear();
    highTempAlarmActive_ = false;
    lowTempAlarmActive_ = false;
    Logger::log(LogLevel::INFO, "Reading history cleared.");
}

void TemperatureSensor::reset()
{
    clearHistory();
    clearThresholds();
    if (state_ == SensorState::READY)
    {
        Logger::log(LogLevel::INFO, "TemperatureSensor reset to initial ready state.");
    }
}
