#pragma once

#include <assert.h>
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>

// param define
// 简单的变量使用宏来定义
// public:
//     const uint32_t& get_uid() const { return uid_; }
//     void set_uid(const uint32_t& newval) { uid_ = newval; }
// private:
//     uint32_t uid_;

#define GETSETVAR(type, name, default_value)                         \
public:                                                              \
    typedef type TYPE_OF_##name;                                     \
    inline const type& name() const { return name##_; }              \
    inline type& name() { return name##_; }                          \
    inline type& mutable_##name() { return name##_; }                \
    inline void set_##name(const type& newval) { name##_ = newval; } \
                                                                     \
protected:                                                           \
    type name##_ = default_value;

#define STRUCT_VAR(type, name)                                       \
public:                                                              \
    typedef type TYPE_OF_##name;                                     \
    inline const type& name() const { return name##_; }              \
    inline type& name() { return name##_; }                          \
    inline type& mutable_##name() { return name##_; }                \
    inline void set_##name(const type& newval) { name##_ = newval; } \
                                                                     \
protected:                                                           \
    type name##_;

#define GETSETPTR(type, name)                                     \
public:                                                           \
    typedef type TYPE_OF_##name;                                  \
    inline type* name##_ptr() const { return name##_; }           \
    inline const type& name() const { return *name##_; }          \
    inline type& name() { return *name##_; }                      \
    inline bool has_##name() const { return nullptr != name##_; } \
    inline void set_##name(type* newval) { name##_ = newval; }    \
    inline void clear_##name() { name##_ = nullptr; }             \
                                                                  \
private:                                                          \
    type* name##_ = nullptr;

#define GETSETCONSTPTR(type, name)                                   \
public:                                                              \
    typedef type TYPE_OF_##name;                                     \
    inline const type* name##_ptr() const { return name##_; }        \
    inline const type& name() const { return *name##_; }             \
    inline const type& name() { return *name##_; }                   \
    inline bool has_##name() const { return nullptr != name##_; }    \
    inline void set_##name(const type* newval) { name##_ = newval; } \
    inline void clear_##name() { name##_ = nullptr; }                \
                                                                     \
private:                                                             \
    const type* name##_ = nullptr;

#define GETSETARRAY(type, name, capacity)                                       \
public:                                                                         \
    typedef type TYPE_OF_##name;                                                \
    static int name##_capacity() { return capacity; }                           \
    inline int name##_len() const { return name##_len_; }                       \
    inline bool name##_full() const { return name##_len_ >= capacity; }         \
    inline bool name##_empty() const { return name##_len_ <= 0; }               \
                                                                                \
public:                                                                         \
    inline const type& name(int index) const {                                  \
        assert(is_valid_##name(index));                                         \
        return name##_[index];                                                  \
    }                                                                           \
    inline const type& last_##name() const { return name(name##_len_ - 1); }    \
    inline type& mutable_##name(int index) {                                    \
        assert(0 <= index && index < name##_capacity_);                         \
        return name##_[index];                                                  \
    }                                                                           \
    inline void set_##name(int index, const type& newval) {                     \
        assert(0 <= index && index < name##_capacity_);                         \
        name##_[index] = newval;                                                \
    }                                                                           \
    inline bool add_##name(const type& newval) {                                \
        if (name##_len_ >= static_cast<int>(capacity)) return false;            \
        name##_[name##_len_++] = newval;                                        \
        return true;                                                            \
    }                                                                           \
    inline type* add_##name() {                                                 \
        if (name##_len_ >= static_cast<int>(capacity)) return nullptr;          \
        return &name##_[name##_len_++];                                         \
    }                                                                           \
    inline void delete_last_##name() {                                          \
        if (name##_len_ <= 0) return;                                           \
        if (!is_valid_##name(name##_len_ - 1)) {                                \
            assert(false);                                                      \
            return;                                                             \
        }                                                                       \
        bzero(&(name##_[name##_len_ - 1]), sizeof(TYPE_OF_##name));             \
        name##_len_--;                                                          \
    }                                                                           \
    inline void delete_##name(int index) {                                      \
        if (!is_valid_##name(index)) {                                          \
            assert(false);                                                      \
            return;                                                             \
        }                                                                       \
        set_##name(index, last_##name());                                       \
        delete_last_##name();                                                   \
    }                                                                           \
    inline void clear_##name() { name##_len_ = 0; }                             \
    inline void set_##name##_len(int len) {                                     \
        assert(0 <= len && len <= static_cast<int>(capacity));                  \
        name##_len_ = len;                                                      \
    }                                                                           \
    inline bool push_##name(const type& newval) {                               \
        if (name##_len_ >= static_cast<int>(capacity)) {                        \
            for (int m_i = 1; m_i < name##_len_; m_i++) {                       \
                set_##name(m_i - 1, name(m_i));                                 \
            }                                                                   \
            name##_len_--;                                                      \
        }                                                                       \
        name##_[name##_len_++] = newval;                                        \
        return true;                                                            \
    }                                                                           \
    inline bool is_valid_##name(int index) const {                              \
        return (0 <= index && index < name##_len_ && index < name##_capacity_); \
    }                                                                           \
                                                                                \
private:                                                                        \
    static const int name##_capacity_ = capacity;                               \
    type name##_[capacity];                                                     \
    int name##_len_ = 0;

#define GETONLYVAR(type, name)                   \
public:                                          \
    const type& name() const { return name##_; } \
                                                 \
private:                                         \
    type name##_;

#define INCVAR(type, name)                                    \
public:                                                       \
    const type& name() const { return name##_; }              \
    void set_##name(const type& newval) { name##_ = newval; } \
    const type& inc_##name(const type& newval = 1) {          \
        name##_ += newval;                                    \
        return name##_;                                       \
    }                                                         \
    const type& dec_##name(const type& newval = 1) {          \
        name##_ -= newval;                                    \
        return name##_;                                       \
    }                                                         \
                                                              \
private:                                                      \
    type name##_ = 0;

#define BITWISEVAR(name)                                      \
public:                                                       \
    uint64_t name() const { return name##_; }                 \
    void set_##name(uint64_t newval) { name##_ = newval; }    \
    void name##_and(uint64_t val) { name##_ &= val; }         \
    void name##_or(uint64_t val) { name##_ |= val; }          \
    bool name##_check(uint64_t val) { return name##_ & val; } \
                                                              \
private:                                                      \
    uint64_t name##_ = 0;

#define GETSETCSTRING(maxsize, name)             \
public:                                          \
    typedef std::string TYPE_OF_##name;          \
    const char* name() const { return name##_; } \
    static int name##_size() { return maxsize; } \
    void set_##name(const char* newval) {        \
        size_t len = strnlen(newval, maxsize);   \
        if (len > maxsize - 1) {                 \
            len = maxsize - 1;                   \
        }                                        \
        memcpy(name##_, newval, len);            \
        name##_[len] = 0;                        \
    }                                            \
    void set_##name(const std::string& newval) { \
        size_t len = newval.length();            \
        if (len > maxsize - 1) {                 \
            len = maxsize - 1;                   \
        }                                        \
        memcpy(name##_, newval.c_str(), len);    \
        name##_[len] = 0;                        \
    }                                            \
                                                 \
private:                                         \
    char name##_[maxsize];

#define M_INT_WARN_RETURN(int_value, uid, ret)               \
    if (int_value != RET::SUCCESS) {                         \
        LOG_WARN(0, uid, "CHECK_FAIL! error=%d", int_value); \
        return ret;                                          \
    }

#define M_PTR_WARN_RETURN(ptr_value, uid, ret) \
    if (ptr_value == NULL) {                   \
        LOG_WARN(0, uid, "CHECK_FAIL!");       \
        return ret;                            \
    }

#define M_BOOL_WARN_RETURN(bool_value, uid, ret) \
    if (bool_value == false) {                   \
        LOG_WARN(0, uid, "CHECK_FAIL!");         \
        return ret;                              \
    }

#define M_INT_WARN_BREAK(int_value, uid, ret, ret_if_fail)   \
    if (int_value != RET::SUCCESS) {                         \
        ret = ret_if_fail;                                   \
        LOG_WARN(0, uid, "CHECK_FAIL! error=%d", int_value); \
        break;                                               \
    }

#define M_PTR_WARN_BREAK(ptr_value, uid, ret, ret_if_fail) \
    if (ptr_value == NULL) {                               \
        ret = ret_if_fail;                                 \
        LOG_WARN(0, uid, "CHECK_FAIL! %d", ret);           \
        break;                                             \
    }

#define M_BOOL_WARN_BREAK(bool_value, uid, ret, ret_if_fail) \
    if (bool_value == false) {                               \
        ret = ret_if_fail;                                   \
        LOG_WARN(0, uid, "CHECK_FAIL! %d", ret);             \
        break;                                               \
    }

#define M_PTR_WARN_CONTINUE(ptr_value, uid) \
    if (ptr_value == NULL) {                \
        LOG_WARN(0, uid, "CHECK_FAIL!");    \
        continue;                           \
    }

#ifndef BIT_GET_AT
#define BIT_GET_AT(data, n) ((data) >> n & 1)
#endif
#ifndef BIT_SET_AT_1
#define BIT_SET_AT_1(data, n) (data |= (1 << n))
#endif

namespace newborn {
namespace pbp {

template <int length>
class DirtyBits {
public:
    typedef uint8_t BIT_TYPE;
    static const int bit_wide = 8;  // uint8_t
    static const int size = length;
    static const int array_size = (size + bit_wide - 1) / bit_wide;

public:
    DirtyBits() {
        static_assert(array_size > 0, "array_size must be greater than 0.");
        clear();
    }
    inline void clear() { memset(data_, 0, sizeof(data_)); }
    inline std::string DebugString() const {
        std::ostringstream stream;
        stream << "array_size:" << array_size << ";";
        for (int i = 0; i < array_size; i++) {
            stream << i << ":" << static_cast<uint32_t>(data_[i]) << ";";
        }
        return stream.str();
    }
    inline DirtyBits& operator+=(const DirtyBits& other) {
        for (int i = 0; i < array_size; i++) {
            data_[i] |= other.data_[i];
        }
        return *this;
    }
    inline bool is_dirty() const {
        for (int i = 0; i < array_size; i++) {
            if (data_[i]) return true;
        }
        return false;
    }
    inline void set_all_dirty() {
        for (int i = 0; i < array_size; i++) {
            data_[i] = ~0;
        }
    }
    inline bool is_dirty(int idx) const {
        assert(0 <= idx && idx < size);
        return dirty_bits(idx) & dirty_bit_mask(idx);
    }
    inline void set_dirty(int idx) {
        assert(0 <= idx && idx < size);
        mutable_dirty_bits(idx) |= dirty_bit_mask(idx);
    }
    inline void clear_dirty(int idx) {
        assert(0 <= idx && idx < size);
        mutable_dirty_bits(idx) &= ~dirty_bit_mask(idx);
    }
    inline void swap_dirty(int idx_left, int idx_right) {
        auto temp = is_dirty(idx_left);
        if (is_dirty(idx_right)) set_dirty(idx_left);
        if (temp) set_dirty(idx_right);
    }
    static inline int dirty_bit_mask(int idx) {
        assert(0 <= idx && idx < size);
        return 1 << (idx % bit_wide);
    }
    inline const BIT_TYPE& dirty_bits(int idx) const {
        assert(0 <= idx && idx < size);
        return data_[idx / bit_wide];
    }
    inline BIT_TYPE& mutable_dirty_bits(int idx) {
        assert(0 <= idx && idx < size);
        return data_[idx / bit_wide];
    }

private:
    BIT_TYPE data_[array_size];
};

};  // namespace pbp
};  // namespace newborn

#define DEFINE_KEY_FUNC(type, name)                          \
public:                                                      \
    typedef type KEY_TYPE;                                   \
    inline const KEY_TYPE& GetKey() const { return name(); } \
    inline void SetKey(const KEY_TYPE& value) { set_##name(value); }

#define DEFINE_VERSION_FUNC(type, name)                              \
public:                                                              \
    typedef type VERSION_TYPE;                                       \
    inline const VERSION_TYPE& GetVersion() const { return name(); } \
    inline void IncVersion() { set_##name(name() + 1); }

#define DEFINE_VERSION_FUNC_EX(field, type, name)                            \
public:                                                                      \
    typedef type VERSION_TYPE;                                               \
    inline const VERSION_TYPE& GetVersion() const { return field().name(); } \
    inline void IncVersion() { mutable_##field().set_##name(field().name() + 1); }

#define DEFINE_DIRTY_BITS_MESSAGE(SIZE, HISTORY_SIZE)                         \
public:                                                                       \
    typedef newborn::pbp::DirtyBits<SIZE> DIRTY_BITS_TYPE;                    \
    static const int history_size_ = HISTORY_SIZE;                            \
                                                                              \
public:                                                                       \
    inline bool is_dirty() const { return dirty_bits_.is_dirty(); }           \
    inline void init_dirty_bits() { history_len_ = 0; }                       \
    inline void save_dirty_bits() {                                           \
        if (history_size_) {                                                  \
            history_pos_++;                                                   \
            if (history_pos_ >= history_size_) history_pos_ -= history_size_; \
            dirty_bits_history_[history_pos_] = dirty_bits_;                  \
            if (history_len_ < history_size_) history_len_++;                 \
        }                                                                     \
        dirty_bits_.clear();                                                  \
    }                                                                         \
    void get_dirty_bits_history(int num, DIRTY_BITS_TYPE& dirty_bits) const { \
        if (num <= 0) return;                                                 \
        if (num > history_len_) {                                             \
            dirty_bits.set_all_dirty();                                       \
            return;                                                           \
        }                                                                     \
        for (int i = 0; i < num; i++) {                                       \
            int place = history_pos_ - i;                                     \
            if (place < 0) place += history_size_;                            \
            dirty_bits += dirty_bits_history_[place];                         \
        }                                                                     \
        LOG_DEBUG(0, num, "dirty_bits=%s", dirty_bits.DebugString().c_str()); \
    }                                                                         \
                                                                              \
private:                                                                      \
    uint8_t history_pos_ = 0;                                                 \
    uint8_t history_len_ = 0;                                                 \
    DIRTY_BITS_TYPE dirty_bits_;                                              \
    DIRTY_BITS_TYPE dirty_bits_history_[history_size_];

#define DEFINE_DIRTY_BITS_FIELD(tag, name)                                            \
public:                                                                               \
    static const int mask_##tag = 1 << (tag % DIRTY_BITS_TYPE::bit_wide);             \
    inline bool is_dirty_##name() const {                                             \
        return dirty_bits_.dirty_bits(tag) & (DIRTY_BITS_TYPE::BIT_TYPE)mask_##tag;   \
    }                                                                                 \
    inline void set_dirty_##name() {                                                  \
        dirty_bits_.mutable_dirty_bits(tag) |= (DIRTY_BITS_TYPE::BIT_TYPE)mask_##tag; \
    }                                                                                 \
    inline bool is_dirty_##name(const DIRTY_BITS_TYPE& dirty_bits) const {            \
        return dirty_bits.dirty_bits(tag) & (DIRTY_BITS_TYPE::BIT_TYPE)mask_##tag;    \
    }

#define DEFINE_DIRTY_BITS_ARRAY(name, SIZE)                                                 \
public:                                                                                     \
    typedef newborn::pbp::DirtyBits<SIZE> name##_DIRTY_BITS_TYPE;                           \
                                                                                            \
public:                                                                                     \
    inline bool is_dirty_##name(int idx) const { return name##_dirty_bits_.is_dirty(idx); } \
    inline void set_dirty_##name(int idx) { name##_dirty_bits_.set_dirty(idx); }            \
    inline void init_dirty_bits_##name() { name##_history_len_ = 0; }                       \
    void save_dirty_bits_##name() {                                                         \
        if (history_size_) {                                                                \
            name##_history_pos_++;                                                          \
            if (name##_history_pos_ >= history_size_) name##_history_pos_ -= history_size_; \
            name##_dirty_histroy_[name##_history_pos_] = name##_dirty_bits_;                \
            if (name##_history_len_ < history_size_) name##_history_len_++;                 \
        }                                                                                   \
        name##_dirty_bits_.clear();                                                         \
    }                                                                                       \
    void get_dirty_bits_history_##name(int num, name##_DIRTY_BITS_TYPE& dirty_bits) const { \
        if (num <= 0) return;                                                               \
        if (num > name##_history_len_) {                                                    \
            dirty_bits.set_all_dirty();                                                     \
            return;                                                                         \
        }                                                                                   \
        for (int i = 0; i < num; i++) {                                                     \
            int place = name##_history_pos_ - i;                                            \
            if (place < 0) place += history_size_;                                          \
            dirty_bits += name##_dirty_histroy_[place];                                     \
        }                                                                                   \
        LOG_DEBUG(0, num, "dirty_bits_%s=%s", #name, dirty_bits.DebugString().c_str());     \
    }                                                                                       \
                                                                                            \
private:                                                                                    \
    uint8_t name##_history_pos_ = 0;                                                        \
    uint8_t name##_history_len_ = 0;                                                        \
    name##_DIRTY_BITS_TYPE name##_dirty_bits_;                                              \
    name##_DIRTY_BITS_TYPE name##_dirty_histroy_[history_size_];

#define DEFINE_FILTER_BITS_MESSAGE(SIZE)                    \
public:                                                     \
    typedef newborn::pbp::DirtyBits<SIZE> FILTER_BITS_TYPE; \
                                                            \
private:                                                    \
    FILTER_BITS_TYPE filter_bits_;

#define DEFINE_FILTER_BITS_FIELD(tag, name)                                                     \
public:                                                                                         \
    static const int mask_filter_##tag = 1 << (tag % FILTER_BITS_TYPE::bit_wide);               \
    inline bool is_require_##name() const {                                                     \
        return !(filter_bits_.dirty_bits(tag) & (FILTER_BITS_TYPE::BIT_TYPE)mask_filter_##tag); \
    }                                                                                           \
    inline void set_require_##name() {                                                          \
        filter_bits_.mutable_dirty_bits(tag) &= ~(FILTER_BITS_TYPE::BIT_TYPE)mask_filter_##tag; \
    }

#define DEFINE_FILTER_BITS_ARRAY(name, SIZE)                                                    \
public:                                                                                         \
    typedef newborn::pbp::DirtyBits<SIZE> name##_FILTER_BITS_TYPE;                              \
                                                                                                \
public:                                                                                         \
    inline bool is_require_##name(int idx) const { return !name##_filter_bits_.is_dirty(idx); } \
    inline void set_require_##name(int idx) {                                                   \
        set_require_##name();                                                                   \
        name##_filter_bits_.clear_dirty(idx);                                                   \
    }                                                                                           \
                                                                                                \
private:                                                                                        \
    name##_FILTER_BITS_TYPE name##_filter_bits_;

#define GETSETVAR_SYNC(tag, type, name, default_value)                     \
public:                                                                    \
    typedef type TYPE_OF_##name;                                           \
    inline const type& name() const { return name##_; }                    \
    inline type& mutable_##name() {                                        \
        set_dirty_##name();                                                \
        return name##_;                                                    \
    }                                                                      \
    inline void set_##name(const type& newval, bool force_dirty = false) { \
        if (newval == name##_) {                                           \
            if (force_dirty) {                                             \
                set_dirty_##name();                                        \
            }                                                              \
            return;                                                        \
        }                                                                  \
        name##_ = newval;                                                  \
        set_dirty_##name();                                                \
    }                                                                      \
                                                                           \
protected:                                                                 \
    DEFINE_DIRTY_BITS_FIELD(tag, name);                                    \
    DEFINE_FILTER_BITS_FIELD(tag, name);                                   \
                                                                           \
protected:                                                                 \
    type name##_ = default_value;

#define STRUCT_VAR_SYNC(tag, type, name)                \
public:                                                 \
    typedef type TYPE_OF_##name;                        \
    inline const type& name() const { return name##_; } \
    inline type& mutable_##name() {                     \
        set_dirty_##name();                             \
        return name##_;                                 \
    }                                                   \
    inline void set_##name(const type& newval) {        \
        name##_ = newval;                               \
        set_dirty_##name();                             \
    }                                                   \
                                                        \
protected:                                              \
    DEFINE_DIRTY_BITS_FIELD(tag, name);                 \
    DEFINE_FILTER_BITS_FIELD(tag, name);                \
                                                        \
protected:                                              \
    type name##_;

#define GETSETCSTRING_SYNC(tag, maxsize, name)           \
public:                                                  \
    typedef std::string TYPE_OF_##name;                  \
    inline const char* name() const { return name##_; }  \
    static int name##_size() { return maxsize; }         \
    void set_##name(const char* newval) {                \
        if (strcmp(newval, name()) == 0) return;         \
        size_t len = strlen(newval);                     \
        if (len > maxsize - 1) len = maxsize - 1;        \
        memcpy(name##_, newval, len);                    \
        name##_[len] = 0;                                \
        set_dirty_##name();                              \
    }                                                    \
    void set_##name(const std::string& newval) {         \
        if (strcmp(newval.c_str(), name()) == 0) return; \
        size_t len = newval.length();                    \
        if (len > maxsize - 1) len = maxsize - 1;        \
        memcpy(name##_, newval.c_str(), len);            \
        name##_[len] = 0;                                \
        set_dirty_##name();                              \
    }                                                    \
                                                         \
protected:                                               \
    DEFINE_DIRTY_BITS_FIELD(tag, name);                  \
    DEFINE_FILTER_BITS_FIELD(tag, name);                 \
                                                         \
protected:                                               \
    char name##_[maxsize];

#define GETSETARRAY_SYNC(tag, type, name, capacity)                                   \
public:                                                                               \
    typedef type TYPE_OF_##name;                                                      \
    static int name##_capacity() { return capacity; }                                 \
    inline int name##_len() const { return name##_len_; }                             \
    inline bool name##_full() const { return name##_len_ >= capacity; }               \
    inline bool name##_empty() const { return name##_len_ <= 0; }                     \
                                                                                      \
public:                                                                               \
    inline const type& name(int index) const {                                        \
        assert(is_valid_##name(index));                                               \
        return name##_[index];                                                        \
    }                                                                                 \
    inline const type& last_##name() const { return name(name##_len_ - 1); }          \
    inline type& mutable_##name(int index) {                                          \
        assert(is_valid_##name(index));                                               \
        set_dirty_##name();                                                           \
        set_dirty_##name(index);                                                      \
        return name##_[index];                                                        \
    }                                                                                 \
    inline void set_##name(int index, const type& newval) {                           \
        if (!is_valid_##name(index)) {                                                \
            assert(false);                                                            \
            return;                                                                   \
        }                                                                             \
        if (&name##_[index] == &newval) return;                                       \
        set_dirty_##name();                                                           \
        set_dirty_##name(index);                                                      \
        name##_[index] = newval;                                                      \
    }                                                                                 \
    inline bool add_##name(const type& newval) {                                      \
        if (name##_len_ >= static_cast<int>(capacity)) return false;                  \
        int index = name##_len_++;                                                    \
        if (!is_valid_##name(index)) {                                                \
            assert(false);                                                            \
            return false;                                                             \
        }                                                                             \
        memset(&name##_[index], 0, sizeof(type));                                     \
        set_##name(index, newval);                                                    \
        return true;                                                                  \
    }                                                                                 \
    inline type* add_##name() {                                                       \
        if (name##_len_ >= static_cast<int>(capacity)) return nullptr;                \
        int index = name##_len_++;                                                    \
        if (!is_valid_##name(index)) {                                                \
            assert(false);                                                            \
            return nullptr;                                                           \
        }                                                                             \
        set_dirty_##name();                                                           \
        set_dirty_##name(index);                                                      \
        memset(&name##_[index], 0, sizeof(type));                                     \
        return &name##_[index];                                                       \
    }                                                                                 \
    inline void delete_last_##name() {                                                \
        if (name##_len_ <= 0) return;                                                 \
        if (!is_valid_##name(name##_len_ - 1)) {                                      \
            assert(false);                                                            \
            return;                                                                   \
        }                                                                             \
        set_dirty_##name();                                                           \
        set_dirty_##name(name##_len_ - 1);                                            \
        bzero(&(name##_[name##_len_ - 1]), sizeof(TYPE_OF_##name));                   \
        --name##_len_;                                                                \
    }                                                                                 \
    inline void delete_##name(int index) {                                            \
        if (!is_valid_##name(index)) {                                                \
            assert(false);                                                            \
            return;                                                                   \
        }                                                                             \
        set_##name(index, last_##name());                                             \
        delete_last_##name();                                                         \
    }                                                                                 \
    inline void clear_##name() {                                                      \
        if (name##_len_ <= 0) return;                                                 \
        set_dirty_##name();                                                           \
        for (int i = 0; i < name##_len_; i++) {                                       \
            set_dirty_##name(i);                                                      \
        }                                                                             \
        name##_len_ = 0;                                                              \
    }                                                                                 \
    inline bool is_valid_##name(int index) const {                                    \
        return ((0 <= index) && (index < name##_len_) && (index < name##_capacity_)); \
    }                                                                                 \
                                                                                      \
protected:                                                                            \
    DEFINE_DIRTY_BITS_FIELD(tag, name);                                               \
    DEFINE_DIRTY_BITS_ARRAY(name, capacity);                                          \
    DEFINE_FILTER_BITS_FIELD(tag, name);                                              \
    DEFINE_FILTER_BITS_ARRAY(name, capacity);                                         \
                                                                                      \
protected:                                                                            \
    static const int name##_capacity_ = capacity;                                     \
    type name##_[capacity];                                                           \
    int name##_len_ = 0;

///////////////////////////////////////////////////////////////////////////////////////
#define GETSET_PBPVAR(pbp, field)                                         \
public:                                                                   \
    typedef TYPE_OF_##pbp::TYPE_OF_##field TYPE_OF_##field;               \
    inline const TYPE_OF_##field& field() const { return pbp().field(); } \
    inline void set_##field(const TYPE_OF_##field& newval) { pbp().set_##field(newval); }

///////////////////////////////////////////////////////////////////////////////////////
#define GETONLY_PBPVAR(pbp, field)                                        \
public:                                                                   \
    typedef TYPE_OF_##pbp::TYPE_OF_##field TYPE_OF_##field;               \
    inline const TYPE_OF_##field& field() const { return pbp().field(); } \
                                                                          \
protected:                                                                \
    inline void set_##field(const TYPE_OF_##field& newval) { pbp().set_##field(newval); }

///////////////////////////////////////////////////////////////////////////////////////
#define GETSET_PBPCSTR(pbp, field)                                      \
public:                                                                 \
    inline const char* field() const { return pbp().field(); }          \
    void set_##field(const char* newval) { pbp().set_##field(newval); } \
    void set_##field(std::string newval) { pbp().set_##field(newval); }

///////////////////////////////////////////////////////////////////////////////////////
#define GETONLY_PBPCSTR(pbp, field)                                     \
public:                                                                 \
    inline const char* field() const { return pbp().field(); }          \
                                                                        \
protected:                                                              \
    void set_##field(const char* newval) { pbp().set_##field(newval); } \
    void set_##field(std::string newval) { pbp().set_##field(newval); }

///////////////////////////////////////////////////////////////////////////////////////
// 必须在OnRestore函数中显示调用restore_##field
#define GETSET_PBPMAP(pbp, field)                                                                  \
public:                                                                                            \
    typedef TYPE_OF_##pbp::TYPE_OF_##field TYPE_OF_##field;                                        \
    typedef TYPE_OF_##field::KEY_TYPE KEY_TYPE_OF_##field;                                         \
    typedef std::unordered_map<TYPE_OF_##field::KEY_TYPE, int> field##_MAP_TYPE;                   \
                                                                                                   \
public:                                                                                            \
    inline bool field##_empty() const { return field##_map_.empty(); }                             \
    inline int field##_size() const { return field##_map_.size(); }                                \
    inline int field##_max_size() const { return pbp().field##_capacity(); }                       \
    inline bool has_##field(TYPE_OF_##field::KEY_TYPE key) const {                                 \
        return field##_map_.find(key) != field##_map_.end();                                       \
    }                                                                                              \
    inline const TYPE_OF_##field* field(TYPE_OF_##field::KEY_TYPE key) const {                     \
        auto const const_iter = field##_map_.find(key);                                            \
        if (const_iter == field##_map_.end()) return nullptr;                                      \
        if (!pbp().is_valid_##field(const_iter->second)) {                                         \
            LOG_ERROR(0, 0,                                                                        \
                      "field not valid,"                                                           \
                      "pbpmap error|%u:%d",                                                        \
                      static_cast<uint32_t>(key), const_iter->second);                             \
            field##_dump_();                                                                       \
            return nullptr;                                                                        \
        }                                                                                          \
        if (pbp().field(const_iter->second).GetKey() != key) {                                     \
            LOG_ERROR(0, 0, "field key not equal|%u:%u",                                           \
                      static_cast<uint32_t>(pbp().field(const_iter->second).GetKey()),             \
                      static_cast<uint32_t>(key));                                                 \
            field##_dump_();                                                                       \
        }                                                                                          \
        return &pbp().field(const_iter->second);                                                   \
    }                                                                                              \
    inline TYPE_OF_##field* mutable_##field(TYPE_OF_##field::KEY_TYPE key) {                       \
        auto iter = field##_map_.find(key);                                                        \
        if (iter == field##_map_.end()) return nullptr;                                            \
        if (!pbp().is_valid_##field(iter->second)) {                                               \
            LOG_ERROR(0, 0,                                                                        \
                      "mutable field not valid,"                                                   \
                      "pbpmap error|%u:%d",                                                        \
                      static_cast<uint32_t>(key), iter->second);                                   \
            field##_dump_();                                                                       \
            return nullptr;                                                                        \
        }                                                                                          \
        if (pbp().field(iter->second).GetKey() != key) {                                           \
            LOG_ERROR(0, 0, "mutable field key not equal|%u:%u",                                   \
                      static_cast<uint32_t>(pbp().field(iter->second).GetKey()),                   \
                      static_cast<uint32_t>(key));                                                 \
            field##_dump_();                                                                       \
        }                                                                                          \
        return &pbp().mutable_##field(iter->second);                                               \
    }                                                                                              \
    inline TYPE_OF_##field* insert_##field(TYPE_OF_##field::KEY_TYPE key) {                        \
        if (has_##field(key)) return mutable_##field(key);                                         \
        if (pbp().field##_full()) return nullptr;                                                  \
        field##_map_.insert(std::make_pair(key, pbp().field##_len()));                             \
        TYPE_OF_##field* item = pbp().add_##field();                                               \
        if (item != nullptr) item->SetKey(key);                                                    \
        return item;                                                                               \
    }                                                                                              \
    inline int erase_##field(TYPE_OF_##field::KEY_TYPE key) {                                      \
        if (!has_##field(key)) return 0;                                                           \
        auto pos = field##_map_[key];                                                              \
        if (pbp().is_valid_##field(pos)) {                                                         \
            pbp().delete_##field(pos);                                                             \
        }                                                                                          \
        if (pbp().is_valid_##field(pos)) { /*二次检查，因为delete_field可能使其失效*/                      \
            field##_map_[pbp().field(pos).GetKey()] = pos;                                         \
        }                                                                                          \
        field##_map_.erase(key);                                                                   \
        return 1;                                                                                  \
    }                                                                                              \
    inline void clear_##field() {                                                                  \
        pbp().clear_##field();                                                                     \
        restore_##field();                                                                         \
    }                                                                                              \
    inline void field##_dump_() const {                                                            \
        LOG_DEBUG(0, 0, "map_size:%lu, filed_size:%u", field##_map_.size(), pbp().field##_len());  \
        for (const auto& mp_item : field##_map_) {                                                 \
            LOG_DEBUG(0, 0, "mp_item <key:%u, pos:%u>", static_cast<uint32_t>(mp_item.first),      \
                      static_cast<uint32_t>(mp_item.second));                                      \
        }                                                                                          \
        for (int i = 0; i < pbp().field##_len(); i++) {                                            \
            LOG_DEBUG(0, 0, "pbp item <pos:%d, key:%u>", i,                                        \
                      static_cast<uint32_t>(pbp().field(i).GetKey()));                             \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    inline void restore_##field() {                                                                \
        field##_map_.clear();                                                                      \
        for (int i = 0; i < pbp().field##_len(); i++) {                                            \
            field##_map_.insert(std::make_pair(pbp().field(i).GetKey(), i));                       \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    field##_MAP_TYPE field##_map_;
