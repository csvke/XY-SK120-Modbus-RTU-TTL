#ifndef XY_SKXXX_H
#define XY_SKXXX_H

#include <Arduino.h>
#include <ModbusMaster.h>
#include "XY-SKxxx-cd-data-group.h" // Add this include for Memory Group definitions

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

#define REG_F_C 0x0013          // temperature unit, 2 bytes, 0 decimal places, unit: 0=Celsius, 1=Fahrenheit, Read and Write

#define REG_B_LED 0x0014        // WIP: Backlight brightness, 2 bytes, 0 decimal places, unit: 0-5, Read and Write, factory default: 5 (brightest)

#define REG_SLEEP 0x0015        // Sleep timeout, 2 bytes, 0 decimal places, unit: min, Read and Write, factory default: 2 min

#define REG_MODEL 0x0016        // WIP: Model number: XY-SK120 returns 22873, 2 bytes, 0 decimal places, unit: ???, Read only
#define REG_VERSION 0x0017      // DPS Firmware Version number, 2 bytes, 0 decimal places, unit: ???, Read only

#define REG_SLAVE_ADDR 0x0018   // Modbus Slave address, 2 bytes, 0 decimal places, unit: 0-247, Read and Write, factory default: 1
#define REG_BAUDRATE_L 0x0019   // Baud rate setting, 2 bytes, 0 decimal places, unit: 0-8 (0: 9600, 1: 14400, 2: 19200, 3: 38400, 4: 56000, 5: 576000, 6: 115200, 7: 2400, 8: 4800), Read and Write, factory default: 6 (115200)

#define REG_T_IN_CAL 0x001A     // Internal temperature calibration, 2 bytes, 1 decimal place, unit: °F / °C, Read and Write
#define REG_T_EXT_CAL 0x001B    // External temperature calibration, 2 bytes, 1 decimal place, unit: °F / °C, Read and Write

#define REG_EXTRACT_M 0x001D    // Data group selection, 2 bytes, 0 decimal places, unit: 0-9, Read and Write

#define REG_SYS_STATUS 0x001E   // WIP: System status???, 2 bytes, 0 decimal places, unit: ???, Read and Write

// Additional Modbus register addresses for XY-SK120 0x0030 - 0x0034, will not implement these as it's related to the Sinilink ESP8285H16 module

// Additional Modbus register addresses for XY-SK120 0x0050 - 0x005D
#define REG_CV_SET         0x0050    // CV (constant voltage) setting, 2 bytes, 2 decimal places, unit: V, Read and Write
#define REG_CC_SET         0x0051    // CC (constant current) setting, 2 bytes, 3 decimal places, unit: A, Read and Write

#define REG_S_LVP          0x0052    // LVP (input low voltage protection) setting, 2 bytes, 2 decimal places, unit: V, Read and Write
#define REG_S_OVP          0x0053    // OVP (output over voltage protection) setting, 2 bytes, 2 decimal places, unit: V, Read and Write

#define REG_S_OCP          0x0054    // OCP (output over current protection) setting, 2 bytes, 3 decimal places, unit: A, Read and Write
#define REG_S_OPP          0x0055    // OPP (output over power protection) setting, 2 bytes, 1 decimal place, unit: W, Read and Write

#define REG_S_OHP_H        0x0056    // OHP_H (output high power protection - hours) setting, 2 bytes, 0 decimal places, unit: h, Read and Write
#define REG_S_OHP_M        0x0057    // OHP_M (output high power protection - minutes) setting, 2 bytes, 0 decimal places, unit: min, Read and Write

#define REG_S_OAH_L        0x0058    // OAH_LOW (over-amp-hour protection - low) setting, 2 bytes, 0 decimal places, unit: mAh, Read and Write
#define REG_S_OAH_H        0x0059    // OAH_HIGH (over-amp-hour protection - high) setting, 2 bytes, 0 decimal places, unit: mAh, Read and Write

#define REG_S_OWH_L        0x005A    // OWH_LOW (over-watt-hour protection - low) setting, 2 bytes, 0 decimal places, unit: 10mWh, Read and Write
#define REG_S_OWH_H        0x005B    // OWH_HIGH (over-watt-hour protection - high) setting, 2 bytes, 0 decimal places, unit: 10mWh, Read and Write

#define REG_S_OTP          0x005C   // Over temperature protection setting, 2 bytes, 1 decimal place, unit: °F / °C, Read and Write

#define REG_S_INI          0x005D    // Power-on initialization setting, 2 bytes, 0 decimal places, unit: 0/1 (0: output off upon power on, 1: output on upon power on), Read and Write

// Additional Modbus register addresses for XY-SK120 0x0100 - 0x0103, will not implement these as it's related to RTC (Real Time Clock) settings

// Additional Modbus register addresses for XY-SK120 0x0110 - 0x011D, will not implement these as it's related to weather infomration???

/* Below are undocumented registers, available in the XY-SK120 manual and OSD (On-Screen Display) 
but not in the Modbus register map documentation
*/

#define REG_S_ETP           0x005E    // WIP: Not implemented.
// REG_S_ETP: both 0x005E and 0x005F stores the ETP (External Temperature Protection) value, but 0x005E is read-write and 0x005F is read-only and both values seems to be mirrored. 
// However, writing to 0x005E does not seem to have any effect on the device, so it's likely not implemented or used.

// beeper settings (beeper enable)
#define REG_BEEPER          0x001C       // Beeper enable/disable, 2 bytes, 0 decimal places, unit: 0/1, Read and Write

// RET setting (Restore factory settings) (discovered through testing)
#define REG_FACTORY_RESET   0x0025 // Factory reset, write 0x1 to trigger reset to defaults

// FET setting (quick adjustment of voltage, current or power with the rotary encoder knob)
// Cannot probe the register for FET setting despite the best effort, so it's not implemented

// PPT Setting (MPPT Solar Charging Settings)
#define REG_MPPT_ENABLE     0x001F  // MPPT enable/disable, 2 bytes, 0 decimal places, unit: 0/1, Read and Write
#define REG_MPPT_THRESHOLD  0x0020 // MPPT threshold percentage, 2 bytes, 2 decimal places, unit: ratio (0.00-1.00), Read and Write

// BTF setting (Battery Full)
#define REG_BTF             0x0021 // Battery charge cut off current, 2 bytes, 3 decimal places, unit: A, Read and Write, set 0 to turn off

// CP Setting (Constant Power Mode)
#define REG_CP_ENABLE     0x0022  // Constant Power mode enable/disable, 2 bytes, 0 decimal places, unit: 0/1, Read and Write
#define REG_CP_SET        0x0023  // Constant Power setting, 2 bytes, 1 decimal place, unit: W, Read and Write

// BCH setting (Battery Charging)


// CLOF setting (Force power output off when switching data sets)
// Cannot probe the register for CLOF setting despite the best effort, so it's not implemented

// POFF (Shutdown function) (On: Enable the shutdown function by pressing the OSD power button for 5 seconds, Off: Disable the shutdown function, cannot locate register)
// Cannot probe the register for POFF setting despite the best effort, so it's not implemented

/* Below are calibration registers, available in the XY-SK120 manual and OSD (On-Screen Display) */

// Voltage calibration registers
#define REG_VOLTAGE_ZERO      0x1007  // Voltage readback zero point (*10), 2 bytes, 0 decimal places, Read and Write
#define REG_VOLTAGE_SCALE_L   0x1008  // Voltage readback scale factor low 16 bits, 2 bytes, 0 decimal places, Read and Write
#define REG_VOLTAGE_SCALE_H   0x1009  // Voltage readback scale factor high 16 bits, 2 bytes, 0 decimal places, Read and Write

// CLU setting (Calibrate output voltage)

// CLA setting (Calibrate output current)

// Zero setting (Current zero calibration)


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

  // Add Constant Power mode related fields
  bool cpModeEnabled;       // Constant Power mode enabled state
  float constantPower;      // Constant Power setting (W)
};

// Protection settings cache structure
struct ProtectionSettings {
  // Constant Voltage/Current settings
  float constantVoltage;    // CV setting (V)
  float constantCurrent;    // CC setting (A)
  
  // Protection thresholds
  float lowVoltageProtection;   // Input low voltage protection (LVP) (V)
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
  
  // Battery settings
  float batteryCutoffCurrent;    // Battery charge cutoff current (A)
};

class XY_SKxxx {
public:
  XY_SKxxx(uint8_t rxPin, uint8_t txPin, uint8_t slaveID);
  void begin(long baudRate);
  bool testConnection();
  
  // Basic device information
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
  bool getOutput(float &voltage, float &current, float &power);
  
  // Combined measurement methods for convenience
  bool getMeasurements(float &outVoltage, float &outCurrent, float &outPower, 
                      float &inVoltage, bool refresh = true);
  bool getEnergyMeasurements(uint32_t &ampHours, uint32_t &wattHours, 
                           uint32_t &outputTime, bool refresh = true);
  bool getTemperatures(float &internalTemp, float &externalTemp, bool refresh = true);
  
  // System control
  bool setKeyLock(bool lock);
  uint16_t getCVCCState(bool refresh = false);
  bool setOutputState(bool on);

  // Device settings - direct register access without caching
  bool setBacklightBrightness(uint8_t level);
  uint8_t getBacklightBrightness();
  bool setSleepTimeout(uint8_t minutes);
  uint8_t getSleepTimeout();
  bool setSlaveAddress(uint8_t address);
  bool getSlaveAddress(uint8_t &address);
  bool setBaudRate(uint8_t baudRate);
  uint8_t getBaudRateCode();
  long getActualBaudRate();
  bool setBeeper(bool enabled);
  bool getBeeper(bool &enabled);
  bool setTemperatureUnit(bool celsius);
  bool getTemperatureUnit(bool &celsius);
  
  // MPPT (Maximum Power Point Tracking) settings
  bool setMPPTEnable(bool enabled);
  bool getMPPTEnable(bool &enabled);
  bool setMPPTThreshold(float threshold);
  bool getMPPTThreshold(float &threshold);
  
  // Temperature calibration
  bool setInternalTempCalibration(float offset);
  bool setExternalTempCalibration(float offset);
  float getInternalTempCalibration(bool refresh = false);
  float getExternalTempCalibration(bool refresh = false);

  // System status
  uint16_t getSystemStatus(bool refresh = false);
  bool setProtectionStatus(uint16_t status);
  bool setSystemStatus(uint16_t status);
  
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
  bool setLowVoltageProtection(float voltage);
  
  bool getOverVoltageProtection(float &voltage);
  bool getOverCurrentProtection(float &current);
  bool getOverPowerProtection(float &power);
  bool getLowVoltageProtection(float &voltage);
  
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

  // Constant Voltage (CV) and Constant Current (CC) mode methods
  bool setConstantVoltage(float voltage);
  bool getConstantVoltage(float &voltage);
  bool setConstantCurrent(float current);
  bool getConstantCurrent(float &current);
  
  // Constant Power (CP) mode methods
  bool setConstantPowerMode(bool enabled);
  bool getConstantPowerMode(bool &enabled);
  bool isConstantPowerModeEnabled(bool refresh = false);
  
  bool setConstantPower(float power);
  bool getConstantPower(float &power);
  float getCachedConstantPower(bool refresh = false);
  
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
  float getCachedLowVoltageProtection(bool refresh = false);
  float getCachedOverVoltageProtection(bool refresh = false);
  float getCachedOverCurrentProtection(bool refresh = false);
  float getCachedOverPowerProtection(bool refresh = false);
  void getCachedHighPowerProtectionTime(uint16_t &hours, uint16_t &minutes, bool refresh = false);
  void getCachedOverAmpHourProtection(uint16_t &ampHoursLow, uint16_t &ampHoursHigh, bool refresh = false);
  void getCachedOverWattHourProtection(uint16_t &wattHoursLow, uint16_t &wattHoursHigh, bool refresh = false);
  float getCachedOverTemperatureProtection(bool refresh = false);
  bool getCachedPowerOnInitialization(bool refresh = false);

  // Add direct register access methods for memory groups
  bool readRegisters(uint16_t addr, uint16_t count, uint16_t* buffer);
  bool readRegister(uint16_t addr, uint16_t& value); // Add this method
  bool writeRegister(uint16_t addr, uint16_t value);
  bool writeRegisters(uint16_t addr, uint16_t count, uint16_t* buffer);

  // Make ModbusMaster available to external code
  ModbusMaster modbus;

  // Debug functions for direct register access
  bool debugReadRegisters(uint16_t addr, uint8_t count, uint16_t* values);
  bool debugWriteRegister(uint16_t addr, uint16_t value);
  bool debugWriteRegisters(uint16_t addr, uint8_t count, const uint16_t* values);

  // Memory Group Methods
  
  /**
   * Read all registers from a memory group
   * 
   * @param group Memory group to read from
   * @param data Array to store the read data (must be able to hold DATA_GROUP_REGISTERS values)
   * @param force Force read from device even if cache is valid
   * @return true if successful
   */
  bool readMemoryGroup(xy_sk::MemoryGroup group, uint16_t* data, bool force = false);
  
  /**
   * Write all registers to a memory group
   * 
   * @param group Memory group to write to
   * @param data Array containing the data to write (must contain DATA_GROUP_REGISTERS values)
   * @return true if successful
   */
  bool writeMemoryGroup(xy_sk::MemoryGroup group, const uint16_t* data);
  
  /**
   * Call a memory group to make it active (copy to M0)
   * 
   * @param group Memory group to call (M1-M9, M0 is ignored)
   * @return true if successful
   */
  bool callMemoryGroup(xy_sk::MemoryGroup group);
  
  /**
   * Read a specific register from a memory group
   * 
   * @param group Memory group to read from
   * @param regOffset Specific register offset from GroupRegisterOffset enum
   * @param value Reference to store the read value
   * @return true if successful
   */
  bool readGroupRegister(xy_sk::MemoryGroup group, xy_sk::GroupRegisterOffset regOffset, uint16_t& value);
  
  /**
   * Write to a specific register in a memory group
   * 
   * @param group Memory group to write to
   * @param regOffset Specific register offset from GroupRegisterOffset enum
   * @param value Value to write
   * @return true if successful
   */
  bool writeGroupRegister(xy_sk::MemoryGroup group, xy_sk::GroupRegisterOffset regOffset, uint16_t value);
  
  /**
   * Get memory group data from cache, optionally refreshing from device
   * 
   * @param group Memory group to get
   * @param data Array to store the group data
   * @param refresh Whether to refresh the cache from device
   * @return true if successful
   */
  bool getCachedMemoryGroup(xy_sk::MemoryGroup group, uint16_t* data, bool refresh = false);
  
  /**
   * Update the memory group cache from the device
   * 
   * @param group Memory group to update
   * @param force Force update even if cache is still valid
   * @return true if successful
   */
  bool updateMemoryGroupCache(xy_sk::MemoryGroup group, bool force = false);

  // Factory reset method
  bool restoreFactoryDefaults();

  // Battery cutoff current methods
  bool setBatteryCutoffCurrent(float current);
  bool getBatteryCutoffCurrent(float &current);
  float getCachedBatteryCutoffCurrent(bool refresh = false);
  bool updateBatteryCutoffCurrent(bool force = false);

private:
  uint8_t _rxPin;
  uint8_t _txPin;
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

  // Additional cache fields 
  float _internalTempCalibration;
  float _externalTempCalibration;
  bool _beeperEnabled;
  uint8_t _selectedDataGroup;
  bool _mpptEnabled;         // Add MPPT enable state cache
  float _mpptThreshold;      // Add MPPT threshold cache
  unsigned long _lastCalibrationUpdate;
  
  // Additional cache timestamps
  unsigned long _lastBatteryCutoffUpdate;
  
  // Communication settings cache
  uint8_t _cachedSlaveAddress;
  uint8_t _cachedBaudRateCode;
  unsigned long _lastCommunicationSettingsUpdate;

  // Update methods for new cached values
  bool updateCalibrationSettings(bool force = false);
  bool updateCommunicationSettings(bool force = false);

  // Memory group cache to avoid repeated reads
  xy_sk::MemoryGroupData groupCache[10]; // 10 groups: M0-M9

  // Add CP mode cache management
  bool updateConstantPowerSettings(bool force = false);
  unsigned long _lastConstantPowerUpdate;
};

#endif // XY_SKXXX_H