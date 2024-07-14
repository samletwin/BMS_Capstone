
#include "Mcp320x.h"
#include "driver/spi_master.h"
#include "esp_log.h"

static const char* TAG = "MCP320x";

// divide n by d and round to next integer
#define div_round(n,d) (((n) + ((d) >> 2)) / (d))

// channel configurations
using MCP3201Ch = MCP320xTypes::MCP3201::Channel;
using MCP3202Ch = MCP320xTypes::MCP3202::Channel;
using MCP3204Ch = MCP320xTypes::MCP3204::Channel;
using MCP3208Ch = MCP320xTypes::MCP3208::Channel;

template <typename T>
MCP320x<T>::MCP320x(uint16_t vref, uint8_t csPin, spi_device_handle_t spi)
  : mVref(vref)
  , mSplSpeed(0)
  , mSpi(spi) {
    if (csPin < GPIO_NUM_MAX) {
        mCsPin = static_cast<gpio_num_t>(csPin);
    } else {
        ESP_LOGE(TAG, "Invalid GPIO pin number for MCP320X Chip Select: %d", csPin);
        // Handle the error appropriately, e.g., by setting a default pin, raising an exception, etc.
        mCsPin = GPIO_NUM_NC; // GPIO_NUM_NC (Not Connected) or some default value
    }
  }

template <typename T>
MCP320x<T>::MCP320x(uint16_t vref, uint8_t csPin)
  : MCP320x(vref, csPin, NULL) {}

template <typename T>
void MCP320x<T>::calibrate(Channel ch)
{
  mSplSpeed = testSplSpeed(ch, 256);
}

template <typename T>
uint16_t MCP320x<T>::read(Channel ch) const
{
  return execute(createCmd(ch));
}

template <typename T>
uint32_t MCP320x<T>::testSplSpeed(Channel ch) const
{
  return testSplSpeed(ch, 64);
}

template <typename T>
uint32_t MCP320x<T>::testSplSpeed(Channel ch, uint16_t num) const
{
  auto cmd = createCmd(ch);
  // start time
  uint32_t t1 = esp_timer_get_time();
  // perform sampling
  for (uint16_t i = 0; i < num; i++) execute(cmd);
  // stop time
  uint32_t t2 = esp_timer_get_time();

  // return average sampling speed
  return div_round((t2 - t1) * 1000, num);
}

template <typename T>
uint32_t MCP320x<T>::testSplSpeed(Channel ch, uint16_t num, uint32_t splFreq)
{
  // required delay
  uint16_t delay = getSplDelay(ch, splFreq);

  auto cmd = createCmd(ch);
  // start time
  uint32_t t1 = esp_timer_get_time();
  // perform sampling
  for (uint16_t i = 0; i < num; i++) {
    execute(cmd);
    ets_delay_us(delay);
  }
  // stop time
  uint32_t t2 = esp_timer_get_time();

  // return average sampling speed
  return div_round((t2 - t1) * 1000, num);
}

template <typename T>
uint16_t MCP320x<T>::toAnalog(uint16_t raw) const
{
  return (static_cast<uint32_t>(raw) * mVref) / (kRes - 1);
}

template <typename T>
uint16_t MCP320x<T>::toDigital(uint16_t val) const
{
  return (static_cast<uint32_t>(val) * (kRes - 1)) / mVref;
}

template <typename T>
uint16_t MCP320x<T>::getVref() const
{
  return mVref;
}

template <typename T>
uint16_t MCP320x<T>::getAnalogRes() const
{
  return (static_cast<uint32_t>(mVref) * 1000) / (kRes - 1);
}

template <typename T>
uint16_t MCP320x<T>::getSplDelay(Channel ch, uint32_t splFreq)
{
  // requested sampling period (ns)
  uint32_t splTime = div_round(1000000000, splFreq);

  // measure speed if uncalibrated
  if (!mSplSpeed) calibrate(ch);

  // calculate delay in us
  int16_t delay =  (splTime - mSplSpeed) / 1000;
  return (delay < 0) ? 0 : static_cast<uint16_t>(delay);
}

template <>
MCP3201::Command<MCP3201Ch> MCP3201::createCmd(MCP3201Ch ch)
{
  // no command required
  return {};
}

template <>
MCP3202::Command<MCP3202Ch> MCP3202::createCmd(MCP3202Ch ch)
{
  // base command structure
  // 0b00000001cc100000
  // c: channel config
  return {
    // add channel to basic command structure
    .value = static_cast<uint16_t>((0x0120 | (ch << 6)))
  };
}

template <>
MCP3204::Command<MCP3204Ch> MCP3204::createCmd(MCP3204Ch ch)
{
  // base command structure
  // 0b000001cxcc000000
  // c: channel config
  return {
    // add channel to basic command structure
    .value = static_cast<uint16_t>((0x0400 | (ch << 6)))
  };
}

template <>
MCP3208::Command<MCP3208Ch> MCP3208::createCmd(MCP3208Ch ch)
{
  // base command structure
  // 0b000001cccc000000
  // c: channel config
  return {
    // add channel to basic command structure
    .value = static_cast<uint16_t>((0x0400 | (ch << 6)))
  };
}

template <>
uint16_t MCP3201::execute(Command<MCP3201Ch> cmd) const
{
  return transfer();
}

template <>
uint16_t MCP3202::execute(Command<MCP3202Ch> cmd) const
{
  return transfer(cmd);
}

template <>
uint16_t MCP3204::execute(Command<MCP3204Ch> cmd) const
{
  return transfer(cmd);
}

template <>
uint16_t MCP3208::execute(Command<MCP3208Ch> cmd) const
{
  return transfer(cmd);
}

template <typename T>
uint16_t MCP320x<T>::transfer() const
{
  SpiData adc;

  spi_transaction_t t;
  t.length = 16;             //Command is 16 bits
  t.tx_buffer = nullptr;
  t.rx_buffer = &adc.value;

  // activate ADC with chip select
  gpio_set_level(mCsPin, 0);

  // send/receive data
  esp_err_t ret = spi_device_transmit(mSpi, &t);
  assert(ret == ESP_OK);  // Should have had no issues.

  // deactivate ADC with chip select
  gpio_set_level(mCsPin, 1);

  return adc.value;
}

template <typename T>
uint16_t MCP320x<T>::transfer(SpiData cmd) const
{
  SpiData adc;

  spi_transaction_t t;
  t.length = 16;             //Command is 16 bits
  t.tx_buffer = &cmd.value;
  t.rx_buffer = &adc.value;

  // activate ADC with chip select
  gpio_set_level(mCsPin, 0);

  // send/receive data
  esp_err_t ret = spi_device_transmit(mSpi, &t);
  assert(ret == ESP_OK);  // Should have had no issues.

  // deactivate ADC with chip select
  gpio_set_level(mCsPin, 1);

  return adc.value;
}

/*
 * Explicit template instantiation for the channel types.
 */
template class MCP320x<MCP3201Ch>;
template class MCP320x<MCP3202Ch>;
template class MCP320x<MCP3204Ch>;
template class MCP320x<MCP3208Ch>;
