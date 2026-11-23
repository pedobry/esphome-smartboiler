#include "SBProtocol.h"
#include <cstring>

namespace esphome {
namespace smartboiler {

void SBProtocolRequest::append_(uint8_t b) {
  if (this->size_ < this->data_.size()) {
    this->data_[this->size_++] = b;
  } else {
    this->overflow_ = true;
  }
}

void SBProtocolRequest::write_le(uint8_t s) { this->append_(s); }

// write value in Little Endian
void SBProtocolRequest::write_le(uint16_t s) {
  for (int i = 0; i < 2; i++)
    this->append_((uint8_t) (s >> (8 * i)));
}

// write value in Little Endian
void SBProtocolRequest::write_le(uint32_t s) {
  for (int i = 0; i < 4; i++)
    this->append_((uint8_t) (s >> (8 * i)));
}

// write value in Little Endian
void SBProtocolRequest::write_le(uint64_t s) {
  for (int i = 0; i < 8; i++)
    this->append_((uint8_t) (s >> (8 * i)));
}

void SBProtocolRequest::write_string(const std::string &s) {
  for (char c : s)
    this->append_((uint8_t) c);
}

uint32_t SBProtocolResult::load_uint32_le(size_t position) const {
  uint32_t number;
  number = this->byte_data_[position];
  number |= uint32_t(this->byte_data_[position + 1]) << 8;
  number |= uint32_t(this->byte_data_[position + 2]) << 16;
  number |= uint32_t(this->byte_data_[position + 3]) << 24;
  return number;
}

SBProtocolResult::SBProtocolResult(const uint8_t *value, uint16_t value_len) {
  if (value == nullptr || value_len < 2)
    return;

  // First two bytes contain a decimal value from SbcPacket as a string
  SBPacket cmd = static_cast<SBPacket>((value[0] - '0') * 10 + (value[1] - '0'));
  this->rq_type = cmd;
  if (cmd == SBPacket::SBC_PACKET_NIGHT_GETDAYS) {
    // copy next 12 bytes
  } else if (cmd == SBPacket::SBC_PACKET_NIGHT_GETDAYS2) {
    // copy next 9 bytes
  } else if (cmd == SBPacket::SBC_PACKET_GLOBAL_CONFIRMUID) {
    // confirmation packet includes UID of original request followed by up to 16 bytes of data
    if (value_len >= 4)
      this->uid = uint16_t(value[2]) | (uint16_t(value[3]) << 8);
    for (size_t i = 4; i < value_len && this->byte_data_size_ < this->byte_data_.size(); i++)
      this->byte_data_[this->byte_data_size_++] = value[i];
  } else if (cmd == SBPacket::SBC_PACKET_GLOBAL_FIRSTLOG || cmd == SBPacket::SBC_PACKET_GLOBAL_NEXTLOG) {
    // two uint32 located at index 2 and 10
  } else if (value_len > 4) {
    // the rest is a string (last two bytes are not part of it)
    // HOLIDAY_GET is also a string: two 8 digit decimal numbers (days to begin, length)
    size_t len = value_len - 4;
    // the original app treats the payload as NUL terminated string
    const void *nul = memchr(value + 2, '\0', len);
    if (nul != nullptr)
      len = static_cast<const uint8_t *>(nul) - (value + 2);
    if (len > SB_MAX_TEXT_LENGTH)
      len = SB_MAX_TEXT_LENGTH;
    memcpy(this->text_, value + 2, len);
    this->text_[len] = '\0';
    this->text_length_ = len;
  }
}

}  // namespace smartboiler
}  // namespace esphome
