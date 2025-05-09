#ifndef XY_SKXXX_H
#define XY_SKXXX_H

#include <Arduino.h>
#include <ModbusMaster.h>

// Define Modbus register addresses (follow protocol naming convention, p.3 of documentation)
#define REG_V_SET 0x0000        // Voltage setting, 2 bytes, 2 decimal places, unit: V, Read and Write
#define REG_I_SET 0x0001        // Current setting, 2 bytes, 3 decimal places, unit: A, Read and Write

#define REG_VOUT 0x0002         // Output voltage display value, 2 bytes, 2 decimal places, unit: V, Read only
#define REG_IOUT 0x0003         // Output current display value, 2 bytes, 3 decimal places, unit: A, Read only

#define REG_POWER 0x0004        // Output power display value, 2 bytes, 2 decimal places, unit: W, Read only
#define REG_UIN 0x0005          // Input voltage display value, 2 bytes, 2 decimal places, unit: V, Read only

#define REG_AH_LOW 0x0006       // Amp-hour low register, 2 bytes, 0 decimal places, unit: mAh, Read only
#define REG_AH_HIGH 0x0007      // Amp-hour high register, 2 bytes, 0 decimal places, unit: mAh, Read only

#define REG_WH_LOW 0x0008       // Watt-hour low register, 2 bytes, 0 decimal places, unit: mWh, Read only
#define REG_WH_HIGH 0x0009      // Watt-hour high register, 2 bytes, 0 decimal places, unit: mWh, Read only

#define REG_OUT_H 0x000A        // Hours of output time, 2 bytes, 0 decimal places, unit: h, Read only
#define REG_OUT_M 0x000B        // Minutes of output time, 2 bytes, 0 decimal places, unit: min, Read only
#define REG_OUT_S 0x000C        // Seconds of output time, 2 bytes, 0 decimal places, unit: s, Read only

#define REG_T_IN 0x000D         // Internal temperature, 2 bytes, 1 decimal place, unit: °F / °C, Read only
#define REG_T_EX 0x000E         // External temperature, 2 bytes, 1 decimal place, unit: °F / °C, Read only

#define REG_LOCK 0x000F         // Key lock status, 2 bytes, 0 decimal places, unit: 0/1 (0: unlock, 1: lock), Read and Write
#define REG_PROTECT 0x0010      // WIP: Protection status???, 2 bytes, 0 decimal places, unit: 0/1, Read and Write - XY6020L-Modbus-Interface.pdf: note 3

#define REG_CVCC 0x0011         // CC/CV (constant current / constant voltage) mode status, 2 bytes, 0 decimal places, unit: 0/1, Read only

#define REG_ONOFF 0x0012        // Output on/off status, 2 bytes, 0 decimal places, unit: 0/1, Read and Write

#define REG_F_C 0x0013          // WIP: temperature unit, 2 bytes, 0 decimal places, unit: 0/1 (°F / °C), Read and Write

#define REG_B_LED 0x0014        // WIP: Backlight brightness, 2 bytes, 0 decimal places, unit: 0-5, Read and Write, factory default: 5 (brightest)

#define REG_SLEEP 0x0015        // Sleep timeout, 2 bytes, 0 decimal places, unit: min, Read and Write, factory default: 2 min

#define REG_MODEL 0x0016        // WIP: Model number: XY-SK120 returns 22873, 2 bytes, 0 decimal places, unit: ???, Read only
#define REG_VERSION 0x0017      // DPS Firmware Version number, 2 bytes, 0 decimal places, unit: ???, Read only

#define REG_SLAVE_ADDR 0x0018   // Modbus Slave address, 2 bytes, 0 decimal places, unit: 0-247, Read and Write, factory default: 1
#define REG_BAUDRATE_L 0x0019   // Baud rate setting, 2 bytes, 0 decimal places, unit: 0-8 (0: 9600, 1: 14400, 2: 19200, 3: 38400, 4: 56000, 5: 576000, 6: 115200, 7: 2400, 8: 4800), Read and Write, factory default: 6 (115200)

#define REG_T_IN_CAL 0x001A     // Internal temperature calibration, 2 bytes, 1 decimal place, unit: °F / °C, Read and Write
#define REG_T_EXT_CAL 0x001B    // External temperature calibration, 2 bytes, 1 decimal place, unit: °F / °C, Read and Write

#define REG_BUZZER 0x001C       // Buzzer enable/disable, 2 bytes, 0 decimal places, unit: 0/1, Read and Write

#define REG_EXTRACT_M 0x001D    // Data group selection, 2 bytes, 0 decimal places, unit: 0-9, Read and Write

#define REG_SYS_STATUS 0x001E   // WIP: System status???, 2 bytes, 0 decimal places, unit: ???, Read and Write

// Additional Modbus register addresses for XY-SK120 0x0030 - 0x0034, will not implement these as it's related to the Sinilink ESP8285H16 module

// Additional Modbus register addresses for XY-SK120 0x0050 - 0x005D
#define REG_CV_SET         0x0050    // CV (constant voltage) setting, 2 bytes, 2 decimal places, unit: V, Read and Write
#define REG_CC_SET         0x0051    // CC (constant current) setting, 2 bytes, 3 decimal places, unit: A, Read and Write

// done
#define REG_S_VLP          0x0052    // LVP (input under voltage protection) setting, 2 bytes, 2 decimal places, unit: W, Read and Write
#define REG_S_OVP          0x0053    // OVP (output over voltage protection) setting, 2 bytes, 2 decimal places, unit: V, Read and Write

//done
#define REG_S_OCP          0x0054    // OCP (output over current protection) setting, 2 bytes, 3 decimal places, unit: A, Read and Write
#define REG_S_OPP          0x0055    // OPP (output over power protection) setting, 2 bytes, 2 decimal places, unit: W, Read and Write

// done
#define REG_S_OHP_H        0x0056    // OHP_H (output high power protection - hours) setting, 2 bytes, 0 decimal places, unit: h, Read and Write
#define REG_S_OHP_M        0x0057    // OHP_M (output high power protection - minutes) setting, 2 bytes, 0 decimal places, unit: min, Read and Write

// done
#define REG_S_OAH_L        0x0058    // OAH_LOW (over-amp-hour protection - low) setting, 2 bytes, 0 decimal places, unit: mAh, Read and Write
#define REG_S_OAH_H        0x0059    // OAH_HIGH (over-amp-hour protection - high) setting, 2 bytes, 0 decimal places, unit: mAh, Read and Write

// done
#define REG_S_OWH_L        0x005A    // OWH_LOW (over-watt-hour protection - low) setting, 2 bytes, 0 decimal places, unit: 10mWh, Read and Write
#define REG_S_OWH_H        0x005B    // OWH_HIGH (over-watt-hour protection - high) setting, 2 bytes, 0 decimal places, unit: 10mWh, Read and Write

// done
#define REG_S_OTP          0x005C   // Over temperature protection setting, 2 bytes, 1 decimal place, unit: °F / °C, Read and Write

#define REG_S_INI          0x005D    // WIP: Power-on initialization setting, 2 bytes, 0 decimal places, unit: 0/1 (0: output off upon power on, 1: output on upon power on), Read and Write

// Additional Modbus register addresses for XY-SK120 0x0100 - 0x0103, will not implement these as it's related to RTC (Real Time Clock) settings

// Additional Modbus register addresses for XY-SK120 0x0110 - 0x011D, will not implement these as it's related to weather infomration???

// undocumented / missing registers, available in the XY-SK120 manual and OSD (On-Screen Display) but not in the Modbus register map documentation
// beeper settings (beeper enable)
// FET setting (quick adjustment of voltage, current or power)
// PPT Setting (MPPT Solar Charging Settings)
// CLU setting (Calibrate output voltage)
// CLA setting (Calibrate output current)
// Zero setting (Current zero calibration)
// CLOF setting (Force power output off when switching data sets)
// RET setting (Restore factory settings)
// POFF (Shutdown function)

// Device status cache structure
struct DeviceStatus {
  // Output measurements
  float outputVoltage;     // Current output voltage (V)
  float outputCurrent;     // Current output current (A)
  float outputPower;       // Current output power (W)
  float inputVoltage;      // Input voltage (V)
  
  // Energy measurements
  // WIP: understand why a low and high registers for amp-hours and watt-hours is needed
  uint32_t ampHours;       // Accumulated amp-hours (mAh)
  uint32_t wattHours;      // Accumulated watt-hours (mWh)
  uint32_t outputTime;     // Output time in seconds
  
  // Temperature readings
  float internalTemp;      // Internal temperature (°C/°F)
  float externalTemp;      // External temperature (°C/°F)
  
  // Device state
  bool outputEnabled;      // Output state (on/off)
  bool keyLocked;          // Key lock status
  uint16_t protectionStatus; // Protection status
  uint16_t cvccMode;       // CC/CV mode (0: CV, 1: CC)
  uint16_t systemStatus;   // System status
  
  // Device settings
  float setVoltage;        // Set voltage (V)
  float setCurrent;        // Set current (A)
  uint8_t backlightLevel;  // Backlight level
  uint8_t sleepTimeout;    // Sleep timeout in minutes
};

// Protection settings cache structure
struct ProtectionSettings {
  // Constant Voltage/Current settings
  float constantVoltage;    // CV setting (V)
  float constantCurrent;    // CC setting (A)
  
  // Protection thresholds
  float underVoltageProtection;  // Input under voltage protection (V)
  float overVoltageProtection;   // Output over voltage protection (V)
  float overCurrentProtection;   // Output over current protection (A)
  float overPowerProtection;     // Output over power protection (W)
  
  // Time-based protection
  uint16_t highPowerHours;       // Output high power protection hours
  uint16_t highPowerMinutes;     // Output high power protection minutes
  
  // Energy-based protection
  uint16_t overAmpHoursLow;      // Over-amp-hour protection low
  uint16_t overAmpHoursHigh;     // Over-amp-hour protection high
  uint16_t overWattHoursLow;     // Over-watt-hour protection low
  uint16_t overWattHoursHigh;    // Over-watt-hour protection high
  
  // Temperature protection
  float overTemperature;         // Over temperature protection (°C/°F)
  
  // Initialization setting
  bool outputOnAtStartup;        // Power-on initialization setting
};

class XY_SKxxx {
public:
  XY_SKxxx(uint8_t rxPin, uint8_t txPin, uint8_t slaveID);
  void begin(long baudRate);
  bool testConnection();
  
  // Basic device information - rename read* to get*
  uint16_t getModel();
  uint16_t getVersion();
  
  // Status cache methods
  bool updateAllStatus(bool force = false);
  bool updateOutputStatus(bool force = false);
  bool updateDeviceSettings(bool force = false);
  bool updateEnergyMeters(bool force = false);
  bool updateTemperatures(bool force = false);
  bool updateDeviceState(bool force = false);
  
  // Cached value access methods
  float getOutputVoltage(bool refresh = false);
  float getOutputCurrent(bool refresh = false);
  float getOutputPower(bool refresh = false);
  float getInputVoltage(bool refresh = false);
  uint32_t getAmpHours(bool refresh = false);
  uint32_t getWattHours(bool refresh = false);
  uint32_t getOutputTime(bool refresh = false);
  float getInternalTemperature(bool refresh = false);
  float getExternalTemperature(bool refresh = false);
  bool isOutputEnabled(bool refresh = false);
  bool isKeyLocked(bool refresh = false);
  uint16_t getProtectionStatus(bool refresh = false);
  bool isInConstantCurrentMode(bool refresh = false);
  bool isInConstantVoltageMode(bool refresh = false);
  float getSetVoltage(bool refresh = false);
  float getSetCurrent(bool refresh = false);
  
  // Output settings
  bool setVoltage(float voltage);
  bool setCurrent(float current);
  bool getOutput(float &voltage, float &current, float &power); // rename readOutput to getOutput
  
  // Measurements
  float getInputVoltage();
  uint32_t getAmpHours();
  uint32_t getWattHours();
  uint32_t getOutputTime();
  float getInternalTemperature();
  float getExternalTemperature();
  
  // System control
  bool setKeyLock(bool lock);
  uint16_t getProtectionStatus();
  uint16_t getCVCCState();
  bool setOutputState(bool on);
  bool setBacklightBrightness(uint8_t level);
  bool setSleepTimeout(uint8_t minutes);
  bool setSlaveAddress(uint8_t address);
  bool setBaudRate(uint8_t baudRate);
  uint16_t getBaudRate(bool refresh = false);
  bool setInternalTempCalibration(float offset);
  bool setExternalTempCalibration(float offset);
  bool setBuzzer(bool on);
  bool setDataGroup(uint8_t group);
  uint16_t getSystemStatus(bool refresh = false);
  
  // Higher-level convenience methods with built-in timing
  bool setVoltageAndCurrent(float voltage, float current);
  bool turnOutputOn();
  bool turnOutputOff();
  bool getOutputStatus(float &voltage, float &current, float &power, bool &isOn);
  
  // Improved Modbus RTU timing methods
  unsigned long silentInterval(unsigned long baudRate);
  void waitForSilentInterval();
  bool preTransmission();
  bool postTransmission();
  
  // Protection settings methods
  bool setOverVoltageProtection(float voltage);
  bool setOverCurrentProtection(float current);
  bool setOverPowerProtection(float power);
  bool setUnderVoltageProtection(float voltage);
  
  bool getOverVoltageProtection(float &voltage);
  bool getOverCurrentProtection(float &current);
  bool getOverPowerProtection(float &power);
  bool getUnderVoltageProtection(float &voltage);
  
  // Amp-hour protection methods
  bool setOverAmpHourProtection(uint16_t ampHoursLow, uint16_t ampHoursHigh);
  bool getOverAmpHourProtection(uint16_t &ampHoursLow, uint16_t &ampHoursHigh);

  // Watt-hour protection methods
  bool setOverWattHourProtection(uint16_t wattHoursLow, uint16_t wattHoursHigh);
  bool getOverWattHourProtection(uint16_t &wattHoursLow, uint16_t &wattHoursHigh);

  // High power protection time methods
  bool setHighPowerProtectionTime(uint16_t hours, uint16_t minutes);
  bool getHighPowerProtectionTime(uint16_t &hours, uint16_t &minutes);

  // Over-temperature protection methods
  bool setOverTemperatureProtection(float temperature);
  bool getOverTemperatureProtection(float &temperature);

  // Power-on initialization setting methods
  bool setPowerOnInitialization(bool outputOnAtStartup);
  bool getPowerOnInitialization(bool &outputOnAtStartup);

  // Temperature unit control
  bool setTemperatureUnit(bool celsius); // true for Celsius, false for Fahrenheit
  bool getTemperatureUnit(bool &celsius);

  // Constant Voltage (CV) and Constant Current (CC) mode methods
  bool setConstantVoltage(float voltage);
  bool getConstantVoltage(float &voltage);
  bool setConstantCurrent(float current);
  bool getConstantCurrent(float &current);
  
  // Protection cache methods
  bool updateAllProtectionSettings(bool force = false);
  bool updateConstantVoltageCurrentSettings(bool force = false);
  bool updateVoltageCurrentProtection(bool force = false);
  bool updatePowerProtection(bool force = false);
  bool updateEnergyProtection(bool force = false);
  bool updateTemperatureProtection(bool force = false);
  bool updateStartupSetting(bool force = false);
  
  // Cached protection value access methods
  float getCachedConstantVoltage(bool refresh = false);
  float getCachedConstantCurrent(bool refresh = false);
  float getCachedUnderVoltageProtection(bool refresh = false);
  float getCachedOverVoltageProtection(bool refresh = false);
  float getCachedOverCurrentProtection(bool refresh = false);
  float getCachedOverPowerProtection(bool refresh = false);
  void getCachedHighPowerProtectionTime(uint16_t &hours, uint16_t &minutes, bool refresh = false);
  void getCachedOverAmpHourProtection(uint16_t &ampHoursLow, uint16_t &ampHoursHigh, bool refresh = false);
  void getCachedOverWattHourProtection(uint16_t &wattHoursLow, uint16_t &wattHoursHigh, bool refresh = false);
  float getCachedOverTemperatureProtection(bool refresh = false);
  bool getCachedPowerOnInitialization(bool refresh = false);

  // Additional get methods for existing settings
  bool getBacklightBrightness(uint8_t &level, bool refresh = false);
  uint8_t getSleepTimeout(bool refresh = false);
  bool getSlaveAddress(uint8_t &address);  // Direct read, no caching needed
  float getInternalTempCalibration(bool refresh = false);
  float getExternalTempCalibration(bool refresh = false);
  bool getBuzzer(bool &enabled);  // Direct read, no caching needed
  uint8_t getSelectedDataGroup();  // Direct read, no caching needed
  
  // If REG_PROTECT and REG_SYS_STATUS are writable (check documentation):
  bool setProtectionStatus(uint16_t status);
  bool setSystemStatus(uint16_t status);

private:
  uint8_t _rxPin;
  uint8_t _txPin;
  ModbusMaster node;
  uint8_t _slaveID;
  unsigned long _baudRate;
  unsigned long _lastCommsTime;
  unsigned long _silentIntervalTime;
  
  // Cache management
  DeviceStatus _status;
  ProtectionSettings _protection;
  unsigned long _lastOutputUpdate;
  unsigned long _lastSettingsUpdate;
  unsigned long _lastEnergyUpdate;
  unsigned long _lastTempUpdate;
  unsigned long _lastStateUpdate;
  unsigned long _cacheTimeout;
  bool _cacheValid;
  unsigned long _lastConstantVCUpdate;
  unsigned long _lastVoltageCurrentProtectionUpdate;
  unsigned long _lastPowerProtectionUpdate;
  unsigned long _lastEnergyProtectionUpdate;
  unsigned long _lastTempProtectionUpdate;
  unsigned long _lastStartupSettingUpdate;
  
  // Static members for callbacks
  static XY_SKxxx* _instance;
  static void staticPreTransmission();
  static void staticPostTransmission();

  // Additional cache fields if needed
  float _internalTempCalibration;
  float _externalTempCalibration;
  bool _buzzerEnabled;
  uint8_t _selectedDataGroup;
  unsigned long _lastCalibrationUpdate;
  
  // Update methods for new cached values
  bool updateCalibrationSettings(bool force = false);
};

#endif // XY_SKXXX_H
