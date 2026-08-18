#include <gtest/gtest.h>
#include "TemperatureSensor.hpp"
#include <memory>

// ============================================================================
// PART 1: Standard Unit Tests (TEST Macro)
// Practice writing tests for standalone functions and input validation.
// ============================================================================

// TODO 1: Test Celsius to Fahrenheit conversion
// Tip: Use EXPECT_DOUBLE_EQ or EXPECT_NEAR(val1, val2, tolerance)
// Test cases to check: 0°C -> 32°F, 100°C -> 212°F, -40°C -> -40°F
TEST(TemperatureConversionTest, CelsiusToFahrenheit)
{
    EXPECT_DOUBLE_EQ(TemperatureSensor::celsiusToFahrenheit(0), 32);
    EXPECT_DOUBLE_EQ(TemperatureSensor::celsiusToFahrenheit(100), 212);
    EXPECT_DOUBLE_EQ(TemperatureSensor::celsiusToFahrenheit(-40), -40);
}

// TODO 2: Test Celsius to Kelvin conversion
// Test cases to check: 0°C -> 273.15 K, -273.15°C -> 0 K, 25°C -> 298.15 K
TEST(TemperatureConversionTest, CelsiusToKelvin)
{
    EXPECT_NEAR(TemperatureSensor::celsiusToKelvin(0), 273.15, 0.5);
    EXPECT_NEAR(TemperatureSensor::celsiusToKelvin(-273.15), 0, 0.5);
    EXPECT_NEAR(TemperatureSensor::celsiusToKelvin(25), 298.15, 0.5);
}

// TODO 3: Test Fahrenheit to Celsius conversion
// Test cases to check: 32°F -> 0°C, 212°F -> 100°C
TEST(TemperatureConversionTest, FahrenheitToCelsius)
{
    EXPECT_NEAR(TemperatureSensor::fahrenheitToCelsius(32), 0, 0.2);
    EXPECT_NEAR(TemperatureSensor::fahrenheitToCelsius(212), 100, 0.2);
}

// TODO 4: Test that a default-constructed sensor is UNINITIALIZED
// Check:
// 1. sensor.isInitialized() should be false
// 2. sensor.getState() should be SensorState::UNINITIALIZED
// 3. sensor.processReading(1000) should throw std::runtime_error (use EXPECT_THROW)
TEST(TemperatureSensorInitTest, DefaultConstructorState)
{
    TemperatureSensor sensor;
    EXPECT_FALSE(sensor.isInitialized());
    EXPECT_EQ(sensor.getState(), SensorState::UNINITIALIZED);
    EXPECT_THROW(sensor.processReading(1000), std::runtime_error);
}

// TODO 5: Test that constructor give valid response on config
// Check cases such as:
// - Check Sensor Model
// - Check Min operational temp value
// - Check Max operational temp value
// - Check ADC resolution
TEST(TemperatureSensorInitTest, ValidConfiguration)
{
    TemperatureSensor sensor;
    SensorConfig senConfig;
    EXPECT_EQ(sensor.getConfig().sensorModel, "STM32F031K6T");
    EXPECT_EQ(sensor.getConfig().minOperationalTemp, -40);
    EXPECT_EQ(sensor.getConfig().maxOperationalTemp, 105);
    EXPECT_EQ(sensor.getConfig().adcResolution, 12);
}
// TODO 6: Test that constructor throws std::invalid_argument on invalid config
// Check cases such as:
// - minOperationalTemp >= maxOperationalTemp
// - calTs1Raw == calTs2Raw
// - calTs1Temp >= calTs2Temp
// - adcResolution not in [8, 16]
TEST(TemperatureSensorInitTest, InvalidConfigurationThrows)
{
    SensorConfig badConfig;
    badConfig.minOperationalTemp = 150;
    badConfig.maxOperationalTemp = 50;
    EXPECT_THROW(TemperatureSensor sensor(badConfig), std::invalid_argument);
    badConfig = SensorConfig{};
    badConfig.calTs1Raw = 1000;
    badConfig.calTs2Raw = 1000;
    EXPECT_THROW(TemperatureSensor sensor(badConfig), std::invalid_argument);
    badConfig = SensorConfig{};
    badConfig.calTs1Temp = 30;
    badConfig.calTs2Temp = 15;
    EXPECT_THROW(TemperatureSensor sensor(badConfig), std::invalid_argument);
    badConfig = SensorConfig{};
    badConfig.adcResolution = 7;
    EXPECT_THROW(TemperatureSensor sensor(badConfig), std::invalid_argument);
}
// ============================================================================
// PART 2: Test Fixture Tests (TEST_F Macro)
// Practice using a Test Fixture with SetUp() and TearDown() for shared state.
// ============================================================================

class TemperatureSensorTest : public ::testing::Test
{
protected:
    SensorConfig config;
    std::unique_ptr<TemperatureSensor> sensor;

    // SetUp() runs before EVERY TEST_F test case
    void SetUp() override
    {
        config.sensorModel = "STM32F031K6T";
        config.minOperationalTemp = -40.0;
        config.maxOperationalTemp = 105.0;
        config.calTs1Raw = 1750; // Raw ADC value at 30.0 °C
        config.calTs2Raw = 950;  // Raw ADC value at 110.0 °C
        config.calTs1Temp = 30.0;
        config.calTs2Temp = 110.0;
        config.adcResolution = 12; // 12-bit ADC (0 - 4095)

        sensor = std::make_unique<TemperatureSensor>(config);
    }

    // TearDown() runs after EVERY TEST_F test case
    void TearDown() override
    {
        sensor.reset();
    }
};

// TODO 6: Verify the initial state of the sensor created in SetUp()
// Check:
// 1. sensor->isInitialized() is true
// 2. sensor->getState() is SensorState::READY
// 3. sensor->getReadingCount() is 0
// 4. sensor->getLatestTemperature() throws std::runtime_error when empty
TEST_F(TemperatureSensorTest, InitialStateAfterSetUp)
{
    EXPECT_TRUE(sensor->isInitialized());
    EXPECT_EQ(sensor->getState(), SensorState::READY);
    EXPECT_EQ(sensor->getReadingCount(), 0);
    EXPECT_THROW(sensor->getLatestTemperature(), std::runtime_error);
}

// TODO 7: Test ADC raw to Celsius conversion accuracy at calibration points
// Check:
// 1. sensor->rawToCelsius(1750) should be approx 30.0 °C
// 2. sensor->rawToCelsius(950) should be approx 110.0 °C
// 3. sensor->rawToCelsius(1350) (midpoint) should be approx 70.0 °C
TEST_F(TemperatureSensorTest, RawAdcToCelsiusConversion)
{
    EXPECT_NEAR(sensor->rawToCelsius(1750), 30.0, 0.2);
    EXPECT_NEAR(sensor->rawToCelsius(950), 110.0, 0.2);
    EXPECT_NEAR(sensor->rawToCelsius(1350), 70.0, 0.2);
}

// TODO 8: Test that exceeding ADC resolution throws std::invalid_argument
// 12-bit ADC maximum allowed value is 4095 (2^12 - 1).
// Check that rawToCelsius(4096) or processReading(4096) throws std::invalid_argument.
TEST_F(TemperatureSensorTest, RawAdcExceedingMaxResolutionThrows)
{
    EXPECT_THROW(sensor->processReading(4096), std::invalid_argument);
    EXPECT_THROW(sensor->rawToCelsius(4096), std::invalid_argument);
}

// TODO 9: Test processing a reading and retrieving temperature in different units
// 1. Call sensor->processReading(1750)
// 2. Check sensor->getReadingCount() is 1
// 3. Check getLatestTemperature(TemperatureUnit::CELSIUS) is ~30.0
// 4. Check getLatestTemperature(TemperatureUnit::FAHRENHEIT) is ~86.0
// 5. Check getLatestTemperature(TemperatureUnit::KELVIN) is ~303.15
TEST_F(TemperatureSensorTest, ProcessReadingAndUnitConversions)
{
    auto readingVal = sensor->processReading(1750);
    EXPECT_EQ(sensor->getReadingCount(), 1);
    EXPECT_NEAR(sensor->getLatestTemperature(TemperatureUnit::CELSIUS), 30.0, 0.2);
    EXPECT_NEAR(sensor->getLatestTemperature(TemperatureUnit::FAHRENHEIT), 86.0, 0.2);
    EXPECT_NEAR(sensor->getLatestTemperature(TemperatureUnit::KELVIN), 303.0, 0.2);
}

// TODO 10: Test reading history and statistical calculations
// 1. Process 3 readings: raw 1750 (30°C), 1350 (70°C), 950 (110°C)
// 2. Check sensor->getReadingCount() is 3
// 3. Check sensor->getMinRecordedTemperature() is ~30.0
// 4. Check sensor->getMaxRecordedTemperature() is ~110.0
// 5. Check sensor->getAverageTemperature() is ~70.0
// 6. Check sensor->getReadingHistory() contains all 3 values
TEST_F(TemperatureSensorTest, HistoryAndStatistics)
{
    sensor->processReading(1750);
    sensor->processReading(1350);
    sensor->processReading(950);

    EXPECT_EQ(sensor->getReadingCount(), 3);
    EXPECT_NEAR(sensor->getMinRecordedTemperature(), 30.0, 0.2);
    EXPECT_NEAR(sensor->getMaxRecordedTemperature(), 110.0, 0.2);
    EXPECT_NEAR(sensor->getAverageTemperature(), 70.0, 0.2);
    // History temp data test
    const auto &historyTemp = sensor->getReadingHistory();
    ASSERT_EQ(historyTemp.size(), 3);
    EXPECT_NEAR(historyTemp[0], 30.0, 0.2);
    EXPECT_NEAR(historyTemp[1], 70.0, 0.2);
    EXPECT_NEAR(historyTemp[2], 110.0, 0.2);
}

// TODO 11: Test high and low temperature alarm triggers
// 1. Set high threshold = 80.0 °C, low threshold = 10.0 °C
// 2. Process reading for 30°C -> alarms should both be false
// 3. Process reading for 110°C (raw 950) -> high alarm should be true, low alarm false
// 4. Process reading for 0°C (raw 2050) -> low alarm should be true, high alarm false
// 5. Call sensor->clearThresholds() -> both alarms should be false
TEST_F(TemperatureSensorTest, HighAndLowTemperatureAlarms)
{
    sensor->setHighTempThreshold(80);
    sensor->setLowTempThreshold(10);
    sensor->processReading(1750);
    EXPECT_FALSE(sensor->isLowTempAlarmActive());
    EXPECT_FALSE(sensor->isHighTempAlarmActive());
    sensor->processReading(950);
    EXPECT_FALSE(sensor->isLowTempAlarmActive());
    EXPECT_TRUE(sensor->isHighTempAlarmActive());
    sensor->processReading(2050);
    EXPECT_TRUE(sensor->isLowTempAlarmActive());
    EXPECT_FALSE(sensor->isHighTempAlarmActive());
    sensor->clearThresholds();
    EXPECT_FALSE(sensor->isLowTempAlarmActive());
    EXPECT_FALSE(sensor->isHighTempAlarmActive());
}

// TODO 12: Test operational limits and status flags
// 1. Check sensor->isWithinOperationalRange() for -40.0, 25.0, 105.0 (true) and -41.0, 106.0 (false)
// 2. Process reading > 105.0°C and check sensor->isOverheating() is true
// 3. Process reading <= 0.0°C and check sensor->isFreezing() is true
TEST_F(TemperatureSensorTest, OperationalRangeAndStatusFlags)
{
    // Within operational temp
    EXPECT_TRUE(sensor->isWithinOperationalRange(-40.0));
    EXPECT_TRUE(sensor->isWithinOperationalRange(25.0));
    EXPECT_TRUE(sensor->isWithinOperationalRange(105.0));

    // Outside operational temp
    EXPECT_FALSE(sensor->isWithinOperationalRange(-41.0));
    EXPECT_FALSE(sensor->isWithinOperationalRange(106.0));

    // Process reading over heating
    sensor->processReading(950);
    EXPECT_TRUE(sensor->isOverheating());
    sensor->processReading(2050);
    EXPECT_TRUE(sensor->isFreezing());
}

// TODO 13: Test sensor reset behavior
// 1. Set threshold, process reading to trigger alarm
// 2. Call sensor->reset()
// 3. Check reading count is 0, alarms are inactive, thresholds cleared, sensor remains initialized
TEST_F(TemperatureSensorTest, ResetClearsHistoryAndAlarms)
{
    sensor->setLowTempThreshold(80);
    sensor->reset();
    EXPECT_EQ(sensor->getReadingCount(), 0);
    EXPECT_FALSE(sensor->isLowTempAlarmActive());
    EXPECT_FALSE(sensor->isHighTempAlarmActive());
    EXPECT_TRUE(sensor->isInitialized());
}
