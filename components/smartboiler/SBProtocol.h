#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <stdint.h>
#include <string>

namespace esphome {
namespace smartboiler {

enum SBPacket : uint16_t {
  SBC_PACKET_ANODE_PARAMS = 95,
  SBC_PACKET_ANODE_VOLTAGE = 94,
  SBC_PACKET_GLOBAL_CONFIRMUID = 51,
  SBC_PACKET_GLOBAL_DEVICEBONDED = 58,
  SBC_PACKET_GLOBAL_ERRORUID = 52,
  SBC_PACKET_GLOBAL_FIRSTLOG = 54,
  SBC_PACKET_GLOBAL_NEXTLOG = 55,
  SBC_PACKET_GLOBAL_PAIRPIN = 53,
  SBC_PACKET_GLOBAL_PINRESULT = 57,
  SBC_PACKET_GLOBAL_RESETBERR = 56,
  SBC_PACKET_GLOBAL_STARTSIMULATION = 50,
  SBC_PACKET_HDO_ALL = 26,
  SBC_PACKET_HDO_FREQUENCY = 24,
  SBC_PACKET_HDO_INFO = 34,
  SBC_PACKET_HDO_LASTHDOTIME = 32,
  SBC_PACKET_HDO_LESSEXPTARIFFAVAILABLENOW = 33,
  SBC_PACKET_HDO_MANUAL_GET = 36,
  SBC_PACKET_HDO_MANUAL_SET = 35,
  SBC_PACKET_HDO_ONOFF = 20,
  SBC_PACKET_HDO_SELECTION_A = 21,
  SBC_PACKET_HDO_SELECTION_B = 22,
  SBC_PACKET_HDO_SELECTION_DP = 23,
  SBC_PACKET_HDO_SETTING = 25,
  SBC_PACKET_HDO_SET_FREQUENCY = 31,
  SBC_PACKET_HDO_SET_ONOFF = 27,
  SBC_PACKET_HDO_SET_SELECTION_A = 28,
  SBC_PACKET_HDO_SET_SELECTION_B = 29,
  SBC_PACKET_HDO_SET_SELECTION_DP = 30,
  SBC_PACKET_HOLIDAY_DELETE = 64,
  SBC_PACKET_HOLIDAY_DISABLE = 63,
  SBC_PACKET_HOLIDAY_ENABLE = 62,
  SBC_PACKET_HOLIDAY_ENABLED = 65,
  SBC_PACKET_HOLIDAY_GET = 60,
  SBC_PACKET_HOLIDAY_SET = 61,
  SBC_PACKET_HOME_ALL = 12,
  SBC_PACKET_HOME_ANTILEGIO = 76,
  SBC_PACKET_HOME_BOILERMODEL = 2,
  SBC_PACKET_HOME_BOILERNAME = 80,
  SBC_PACKET_HOME_CAPACITY = 83,
  SBC_PACKET_HOME_ERROR = 5,
  SBC_PACKET_HOME_FWBEGIN = 86,
  SBC_PACKET_HOME_FWCHECK = 88,
  SBC_PACKET_HOME_FWCONFIRM = 87,
  SBC_PACKET_HOME_FWCOPY = 89,
  SBC_PACKET_HOME_FWRESET = 59,
  SBC_PACKET_HOME_FWVERSION = 3,
  SBC_PACKET_HOME_HSRCSTATE = 6,
  SBC_PACKET_HOME_MODE = 4,
  SBC_PACKET_HOME_NEWERR = 77,
  SBC_PACKET_HOME_SENSOR1 = 7,
  SBC_PACKET_HOME_SENSOR2 = 8,
  SBC_PACKET_HOME_SETBOILERNAME = 81,
  SBC_PACKET_HOME_SETCAPACITY = 84,
  SBC_PACKET_HOME_SETMODE = 18,
  SBC_PACKET_HOME_SETNORMALMODE = 19,
  SBC_PACKET_HOME_SETNORMALTEMPERATURE = 14,
  SBC_PACKET_HOME_SETTEMPNIGHT = 16,
  SBC_PACKET_HOME_SETTEMPNIGHTLOW = 17,
  SBC_PACKET_HOME_SETTIME = 13,
  SBC_PACKET_HOME_TEMPERATURE = 9,
  SBC_PACKET_HOME_TEMPNIGHT = 10,
  SBC_PACKET_HOME_TEMPNIGHTCURR = 82,
  SBC_PACKET_HOME_TEMPNIGHTLOW = 15,
  SBC_PACKET_HOME_TIME = 11,
  SBC_PACKET_LOG_MSG = 1,
  SBC_PACKET_NIGHT_GETDAY = 40,
  SBC_PACKET_NIGHT_GETDAYS = 44,
  SBC_PACKET_NIGHT_GETDAYS2 = 46,
  SBC_PACKET_NIGHT_SAVEDAY = 41,
  SBC_PACKET_NIGHT_SAVEDAYS = 43,
  SBC_PACKET_NIGHT_SAVEDAYS2 = 45,
  SBC_PACKET_NIGHT_SAVEMINMAX = 42,
  SBC_PACKET_NONE = 0,
  SBC_PACKET_POWERCONS_OBTAIN = 74,
  SBC_PACKET_POWERCONS_RESET = 73,
  SBC_PACKET_RQ_GLOBAL_MAC = 68,
  SBC_PACKET_RQ_UI_BUTTONDOWN = 71,
  SBC_PACKET_RQ_UI_BUTTONUP = 70,
  SBC_PACKET_RQ_UI_D7SEG = 72,
  SBC_PACKET_STATISTICS_GETALL = 93,
  SBC_PACKET_STATISTICS_RESET = 92,
  SBC_PACKET_STATISTICS_WEEK = 90,
  SBC_PACKET_STATISTICS_YEAR = 91,
};

enum Mode : uint8_t {
  STOP = 0,
  // MANUAL/HDO is selected based on HDO setting in water heater
  MANUAL = 1,
  HDO = 2,
  // SMART/SMART_HDO is selected based on HDO setting in water heater
  SMART = 3,
  SMART_HDO = 4,
  ANTIFREEZE = 5,
  PROG = 6,
  // following modes are reported by water heater, but cannot be set via SETMODE
  TEST = 7,
  HOLIDAY = 8,
};

// Largest request sent to the water heater: 4 bytes header + 8 bytes payload (time adjustment).
// 20 bytes fit into a single BLE write with the default MTU.
static constexpr size_t SB_MAX_REQUEST_SIZE = 20;
// Confirmation packets carry up to 16 bytes of data after the UID.
static constexpr size_t SB_MAX_CONFIRM_DATA = 16;
// Longest text payload kept from a notification (longer texts are truncated).
static constexpr size_t SB_MAX_TEXT_LENGTH = 64;

/// Fixed-capacity FIFO/list without heap allocation. Not thread safe.
template<typename T, size_t N> class FixedQueue {
 public:
  using iterator = T *;
  using const_iterator = const T *;

  /// Append an item, returns false if the queue is full.
  bool push_back(const T &item) {
    if (this->size_ >= N)
      return false;
    this->items_[this->size_++] = item;
    return true;
  }
  /// Remove the item at pos, returns iterator to the following item.
  iterator erase(iterator pos) {
    std::move(pos + 1, this->end(), pos);
    this->size_--;
    return pos;
  }
  T &front() { return this->items_[0]; }
  void clear() { this->size_ = 0; }
  size_t size() const { return this->size_; }
  bool empty() const { return this->size_ == 0; }
  static constexpr size_t capacity() { return N; }

  iterator begin() { return this->items_.data(); }
  iterator end() { return this->items_.data() + this->size_; }
  const_iterator begin() const { return this->items_.data(); }
  const_iterator end() const { return this->items_.data() + this->size_; }

 protected:
  std::array<T, N> items_{};
  size_t size_{0};
};

class SBProtocolRequest {
 public:
  SBProtocolRequest() = default;
  explicit SBProtocolRequest(SBPacket req_type, uint16_t uid = 0) : rq_type(req_type), uid(uid) {
    this->write_le(uint16_t(req_type));
    this->write_le(uid);
  }

  void write_le(uint8_t s);
  void write_le(uint16_t s);
  void write_le(uint32_t s);
  void write_le(uint64_t s);
  void write_string(const std::string &s);

  const uint8_t *data() const { return this->data_.data(); }
  size_t size() const { return this->size_; }
  /// true if the payload did not fit into SB_MAX_REQUEST_SIZE and was truncated
  bool overflow() const { return this->overflow_; }

  SBPacket rq_type{SBPacket::SBC_PACKET_NONE};
  uint16_t uid{0};
  // millis() of the first and the most recent transmission (0 = not sent yet)
  uint32_t first_sent{0};
  uint32_t last_sent{0};

 protected:
  void append_(uint8_t b);

  std::array<uint8_t, SB_MAX_REQUEST_SIZE> data_{};
  uint8_t size_{0};
  bool overflow_{false};
};

class SBProtocolResult {
 public:
  SBProtocolResult(const uint8_t *value, uint16_t value_len);

  /// data of a confirmation packet (after the UID)
  const uint8_t *byte_data() const { return this->byte_data_.data(); }
  size_t byte_data_size() const { return this->byte_data_size_; }
  /// text payload, always NUL terminated
  const char *text() const { return this->text_; }
  size_t text_length() const { return this->text_length_; }
  /// little endian uint32 from confirmation data, caller must check byte_data_size()
  uint32_t load_uint32_le(size_t position) const;

  SBPacket rq_type{SBPacket::SBC_PACKET_NONE};
  uint16_t uid{0};

 protected:
  std::array<uint8_t, SB_MAX_CONFIRM_DATA> byte_data_{};
  uint8_t byte_data_size_{0};
  char text_[SB_MAX_TEXT_LENGTH + 1]{};
  uint8_t text_length_{0};
};

}  // namespace smartboiler
}  // namespace esphome
