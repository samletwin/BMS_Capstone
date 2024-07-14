#ifndef MCP320X_H
#define MCP320X_H

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

namespace MCP320xTypes {

namespace MCP3201 {
  enum Channel {
    SINGLE_0 = 0x0  /**< single channel 0 */
  };
};

namespace MCP3202 {
  enum Channel {
    SINGLE_0 = 0b10,  /**< single channel 0 */
    SINGLE_1 = 0b11,  /**< single channel 1 */
    DIFF_0PN = 0b00,  /**< differential channel 0 (input 0+,1-) */
    DIFF_0NP = 0b01   /**< differential channel 0 (input 0-,1+) */
  };
};

namespace MCP3204 {
  enum Channel {
    SINGLE_0 = 0b1000,  /**< single channel 0 */
    SINGLE_1 = 0b1001,  /**< single channel 1 */
    SINGLE_2 = 0b1010,  /**< single channel 2 */
    SINGLE_3 = 0b1011,  /**< single channel 3 */
    DIFF_0PN = 0b0000,  /**< differential channel 0 (input 0+,1-) */
    DIFF_0NP = 0b0001,  /**< differential channel 0 (input 0-,1+) */
    DIFF_1PN = 0b0010,  /**< differential channel 1 (input 2+,3-) */
    DIFF_1NP = 0b0011   /**< differential channel 1 (input 2-,3+) */
  };
};

namespace MCP3208 {
  enum Channel {
    SINGLE_0 = 0b1000,  /**< single channel 0 */
    SINGLE_1 = 0b1001,  /**< single channel 1 */
    SINGLE_2 = 0b1010,  /**< single channel 2 */
    SINGLE_3 = 0b1011,  /**< single channel 3 */
    SINGLE_4 = 0b1100,  /**< single channel 4 */
    SINGLE_5 = 0b1101,  /**< single channel 5 */
    SINGLE_6 = 0b1110,  /**< single channel 6 */
    SINGLE_7 = 0b1111,  /**< single channel 7 */
    DIFF_0PN = 0b0000,  /**< differential channel 0 (input 0+,1-) */
    DIFF_0NP = 0b0001,  /**< differential channel 0 (input 0-,1+) */
    DIFF_1PN = 0b0010,  /**< differential channel 1 (input 2+,3-) */
    DIFF_1NP = 0b0011,  /**< differential channel 1 (input 2-,3+) */
    DIFF_2PN = 0b0100,  /**< differential channel 2 (input 4+,5-) */
    DIFF_2NP = 0b0101,  /**< differential channel 2 (input 5-,5+) */
    DIFF_3PN = 0b0110,  /**< differential channel 3 (input 6+,7-) */
    DIFF_3NP = 0b0111   /**< differential channel 3 (input 6-,7+) */
  };
};

}; // namespace MCP320xTypes

template <typename ChannelType>
class MCP320x {

public:

  static const uint8_t kResBits = 12;
  static const uint16_t kRes = (1 << kResBits);

  using Channel = ChannelType;

  MCP320x(uint16_t vref, uint8_t csPin, spi_device_handle_t spi);

  MCP320x(uint16_t vref, uint8_t csPin);

  void calibrate(Channel ch);

  uint16_t read(Channel ch) const;

  template <typename T, size_t N>
  void read(Channel ch, T (&data)[N]) const
  {
    readn(ch, data, N);
  }

  template <typename T, size_t N>
  void read(Channel ch, T (&data)[N], uint32_t splFreq)
  {
    readn(ch, data, N, splFreq);
  }

  template <typename T, size_t N, typename Predicate>
  void read_if(Channel ch, T (&data)[N], Predicate p) const
  {
    readn_if(ch, data, N, p);
  }

  template <typename T, size_t N, typename Predicate>
  void read_if(Channel ch, T (&data)[N], uint32_t splFreq, Predicate p)
  {
    readn_if(ch, data, N, splFreq, p);
  }

  template <typename T>
  void readn(Channel ch, T *data, uint16_t num) const
  {
    execute(createCmd(ch), data, num);
  }

  template <typename T>
  void readn(Channel ch, T *data, uint16_t num, uint32_t splFreq)
  {
    execute(createCmd(ch), data, num, getSplDelay(ch, splFreq));
  }

  template <typename T, typename Predicate>
  void readn_if(Channel ch, T *data, uint16_t num, Predicate p) const
  {
    auto cmd = createCmd(ch);
    while (!p(execute(cmd))) {}
    execute(cmd, data, num);
  }

  template <typename T, typename Predicate>
  void readn_if(Channel ch, T *data, uint16_t num, uint32_t splFreq, Predicate p)
  {
    auto cmd = createCmd(ch);
    while (!p(execute(cmd))) {}
    execute(cmd, data, num, getSplDelay(ch, splFreq));
  }

  uint32_t testSplSpeed(Channel ch) const;

  uint32_t testSplSpeed(Channel ch, uint16_t num) const;

  uint32_t testSplSpeed(Channel ch, uint16_t num, uint32_t splFreq);

  uint16_t toAnalog(uint16_t raw) const;

  uint16_t toDigital(uint16_t val) const;

  uint16_t getVref() const;

  uint16_t getAnalogRes() const;

private:

  union SpiData {
    uint16_t value;
    struct {
      uint8_t loByte;
      uint8_t hiByte;
    };
  };

  template <typename>
  using Command = SpiData;

  uint16_t getSplDelay(Channel ch, uint32_t splFreq);

  static Command<Channel> createCmd(Channel ch);

  uint16_t execute(Command<Channel> cmd) const;

  template <typename T>
  void execute(Command<Channel> cmd, T *data, uint16_t num) const
  {
    for (decltype(num) i=0; i < num; i++)
      data[i] = static_cast<T>(execute(cmd));
  }

  template <typename T>
  void execute(Command<Channel> cmd, T *data, uint16_t num, uint16_t delay) const
  {
    for (decltype(num) i=0; i < num; i++) {
      data[i] = static_cast<T>(execute(cmd));
      ets_delay_us(delay);
    }
  }

  uint16_t transfer() const;

  uint16_t transfer(SpiData cmd) const;

private:

  uint16_t mVref;
  gpio_num_t mCsPin;
  uint32_t mSplSpeed;
  spi_device_handle_t mSpi;
};

using MCP3201 = MCP320x<MCP320xTypes::MCP3201::Channel>;
using MCP3202 = MCP320x<MCP320xTypes::MCP3202::Channel>;
using MCP3204 = MCP320x<MCP320xTypes::MCP3204::Channel>;
using MCP3208 = MCP320x<MCP320xTypes::MCP3208::Channel>;



#endif /* MCP320X_H */