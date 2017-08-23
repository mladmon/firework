// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: all_types.proto

#ifndef PROTOBUF_all_5ftypes_2eproto__INCLUDED
#define PROTOBUF_all_5ftypes_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace tutorial {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_all_5ftypes_2eproto();
void protobuf_AssignDesc_all_5ftypes_2eproto();
void protobuf_ShutdownFile_all_5ftypes_2eproto();

class AllTypes;
class DataArray;

// ===================================================================

class AllTypes : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tutorial.AllTypes) */ {
 public:
  AllTypes();
  virtual ~AllTypes();

  AllTypes(const AllTypes& from);

  inline AllTypes& operator=(const AllTypes& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const AllTypes& default_instance();

  void Swap(AllTypes* other);

  // implements Message ----------------------------------------------

  inline AllTypes* New() const { return New(NULL); }

  AllTypes* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AllTypes& from);
  void MergeFrom(const AllTypes& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(AllTypes* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional double myDouble = 1;
  void clear_mydouble();
  static const int kMyDoubleFieldNumber = 1;
  double mydouble() const;
  void set_mydouble(double value);

  // optional float myFloat = 2;
  void clear_myfloat();
  static const int kMyFloatFieldNumber = 2;
  float myfloat() const;
  void set_myfloat(float value);

  // optional int32 myInt32 = 3;
  void clear_myint32();
  static const int kMyInt32FieldNumber = 3;
  ::google::protobuf::int32 myint32() const;
  void set_myint32(::google::protobuf::int32 value);

  // optional int64 myInt64 = 4;
  void clear_myint64();
  static const int kMyInt64FieldNumber = 4;
  ::google::protobuf::int64 myint64() const;
  void set_myint64(::google::protobuf::int64 value);

  // optional uint32 myUint32 = 5;
  void clear_myuint32();
  static const int kMyUint32FieldNumber = 5;
  ::google::protobuf::uint32 myuint32() const;
  void set_myuint32(::google::protobuf::uint32 value);

  // optional uint64 myUint64 = 6;
  void clear_myuint64();
  static const int kMyUint64FieldNumber = 6;
  ::google::protobuf::uint64 myuint64() const;
  void set_myuint64(::google::protobuf::uint64 value);

  // optional sint32 mySint32 = 7;
  void clear_mysint32();
  static const int kMySint32FieldNumber = 7;
  ::google::protobuf::int32 mysint32() const;
  void set_mysint32(::google::protobuf::int32 value);

  // optional sint64 mySint64 = 8;
  void clear_mysint64();
  static const int kMySint64FieldNumber = 8;
  ::google::protobuf::int64 mysint64() const;
  void set_mysint64(::google::protobuf::int64 value);

  // optional fixed32 myFixed32 = 9;
  void clear_myfixed32();
  static const int kMyFixed32FieldNumber = 9;
  ::google::protobuf::uint32 myfixed32() const;
  void set_myfixed32(::google::protobuf::uint32 value);

  // optional fixed64 myFixed64 = 10;
  void clear_myfixed64();
  static const int kMyFixed64FieldNumber = 10;
  ::google::protobuf::uint64 myfixed64() const;
  void set_myfixed64(::google::protobuf::uint64 value);

  // optional sfixed32 mySfixed32 = 11;
  void clear_mysfixed32();
  static const int kMySfixed32FieldNumber = 11;
  ::google::protobuf::int32 mysfixed32() const;
  void set_mysfixed32(::google::protobuf::int32 value);

  // optional sfixed64 mySfixed64 = 12;
  void clear_mysfixed64();
  static const int kMySfixed64FieldNumber = 12;
  ::google::protobuf::int64 mysfixed64() const;
  void set_mysfixed64(::google::protobuf::int64 value);

  // optional bool myBool = 13;
  void clear_mybool();
  static const int kMyBoolFieldNumber = 13;
  bool mybool() const;
  void set_mybool(bool value);

  // optional string myString = 14;
  void clear_mystring();
  static const int kMyStringFieldNumber = 14;
  const ::std::string& mystring() const;
  void set_mystring(const ::std::string& value);
  void set_mystring(const char* value);
  void set_mystring(const char* value, size_t size);
  ::std::string* mutable_mystring();
  ::std::string* release_mystring();
  void set_allocated_mystring(::std::string* mystring);

  // optional bytes myBytes = 15;
  void clear_mybytes();
  static const int kMyBytesFieldNumber = 15;
  const ::std::string& mybytes() const;
  void set_mybytes(const ::std::string& value);
  void set_mybytes(const char* value);
  void set_mybytes(const void* value, size_t size);
  ::std::string* mutable_mybytes();
  ::std::string* release_mybytes();
  void set_allocated_mybytes(::std::string* mybytes);

  // @@protoc_insertion_point(class_scope:tutorial.AllTypes)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  double mydouble_;
  float myfloat_;
  ::google::protobuf::int32 myint32_;
  ::google::protobuf::int64 myint64_;
  ::google::protobuf::uint64 myuint64_;
  ::google::protobuf::uint32 myuint32_;
  ::google::protobuf::int32 mysint32_;
  ::google::protobuf::int64 mysint64_;
  ::google::protobuf::uint64 myfixed64_;
  ::google::protobuf::uint32 myfixed32_;
  ::google::protobuf::int32 mysfixed32_;
  ::google::protobuf::int64 mysfixed64_;
  ::google::protobuf::internal::ArenaStringPtr mystring_;
  ::google::protobuf::internal::ArenaStringPtr mybytes_;
  bool mybool_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_all_5ftypes_2eproto();
  friend void protobuf_AssignDesc_all_5ftypes_2eproto();
  friend void protobuf_ShutdownFile_all_5ftypes_2eproto();

  void InitAsDefaultInstance();
  static AllTypes* default_instance_;
};
// -------------------------------------------------------------------

class DataArray : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tutorial.DataArray) */ {
 public:
  DataArray();
  virtual ~DataArray();

  DataArray(const DataArray& from);

  inline DataArray& operator=(const DataArray& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const DataArray& default_instance();

  void Swap(DataArray* other);

  // implements Message ----------------------------------------------

  inline DataArray* New() const { return New(NULL); }

  DataArray* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const DataArray& from);
  void MergeFrom(const DataArray& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(DataArray* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .tutorial.AllTypes data = 1;
  int data_size() const;
  void clear_data();
  static const int kDataFieldNumber = 1;
  const ::tutorial::AllTypes& data(int index) const;
  ::tutorial::AllTypes* mutable_data(int index);
  ::tutorial::AllTypes* add_data();
  ::google::protobuf::RepeatedPtrField< ::tutorial::AllTypes >*
      mutable_data();
  const ::google::protobuf::RepeatedPtrField< ::tutorial::AllTypes >&
      data() const;

  // @@protoc_insertion_point(class_scope:tutorial.DataArray)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::RepeatedPtrField< ::tutorial::AllTypes > data_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_all_5ftypes_2eproto();
  friend void protobuf_AssignDesc_all_5ftypes_2eproto();
  friend void protobuf_ShutdownFile_all_5ftypes_2eproto();

  void InitAsDefaultInstance();
  static DataArray* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// AllTypes

// optional double myDouble = 1;
inline void AllTypes::clear_mydouble() {
  mydouble_ = 0;
}
inline double AllTypes::mydouble() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myDouble)
  return mydouble_;
}
inline void AllTypes::set_mydouble(double value) {
  
  mydouble_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myDouble)
}

// optional float myFloat = 2;
inline void AllTypes::clear_myfloat() {
  myfloat_ = 0;
}
inline float AllTypes::myfloat() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myFloat)
  return myfloat_;
}
inline void AllTypes::set_myfloat(float value) {
  
  myfloat_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myFloat)
}

// optional int32 myInt32 = 3;
inline void AllTypes::clear_myint32() {
  myint32_ = 0;
}
inline ::google::protobuf::int32 AllTypes::myint32() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myInt32)
  return myint32_;
}
inline void AllTypes::set_myint32(::google::protobuf::int32 value) {
  
  myint32_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myInt32)
}

// optional int64 myInt64 = 4;
inline void AllTypes::clear_myint64() {
  myint64_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 AllTypes::myint64() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myInt64)
  return myint64_;
}
inline void AllTypes::set_myint64(::google::protobuf::int64 value) {
  
  myint64_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myInt64)
}

// optional uint32 myUint32 = 5;
inline void AllTypes::clear_myuint32() {
  myuint32_ = 0u;
}
inline ::google::protobuf::uint32 AllTypes::myuint32() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myUint32)
  return myuint32_;
}
inline void AllTypes::set_myuint32(::google::protobuf::uint32 value) {
  
  myuint32_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myUint32)
}

// optional uint64 myUint64 = 6;
inline void AllTypes::clear_myuint64() {
  myuint64_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 AllTypes::myuint64() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myUint64)
  return myuint64_;
}
inline void AllTypes::set_myuint64(::google::protobuf::uint64 value) {
  
  myuint64_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myUint64)
}

// optional sint32 mySint32 = 7;
inline void AllTypes::clear_mysint32() {
  mysint32_ = 0;
}
inline ::google::protobuf::int32 AllTypes::mysint32() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.mySint32)
  return mysint32_;
}
inline void AllTypes::set_mysint32(::google::protobuf::int32 value) {
  
  mysint32_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.mySint32)
}

// optional sint64 mySint64 = 8;
inline void AllTypes::clear_mysint64() {
  mysint64_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 AllTypes::mysint64() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.mySint64)
  return mysint64_;
}
inline void AllTypes::set_mysint64(::google::protobuf::int64 value) {
  
  mysint64_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.mySint64)
}

// optional fixed32 myFixed32 = 9;
inline void AllTypes::clear_myfixed32() {
  myfixed32_ = 0u;
}
inline ::google::protobuf::uint32 AllTypes::myfixed32() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myFixed32)
  return myfixed32_;
}
inline void AllTypes::set_myfixed32(::google::protobuf::uint32 value) {
  
  myfixed32_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myFixed32)
}

// optional fixed64 myFixed64 = 10;
inline void AllTypes::clear_myfixed64() {
  myfixed64_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 AllTypes::myfixed64() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myFixed64)
  return myfixed64_;
}
inline void AllTypes::set_myfixed64(::google::protobuf::uint64 value) {
  
  myfixed64_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myFixed64)
}

// optional sfixed32 mySfixed32 = 11;
inline void AllTypes::clear_mysfixed32() {
  mysfixed32_ = 0;
}
inline ::google::protobuf::int32 AllTypes::mysfixed32() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.mySfixed32)
  return mysfixed32_;
}
inline void AllTypes::set_mysfixed32(::google::protobuf::int32 value) {
  
  mysfixed32_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.mySfixed32)
}

// optional sfixed64 mySfixed64 = 12;
inline void AllTypes::clear_mysfixed64() {
  mysfixed64_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 AllTypes::mysfixed64() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.mySfixed64)
  return mysfixed64_;
}
inline void AllTypes::set_mysfixed64(::google::protobuf::int64 value) {
  
  mysfixed64_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.mySfixed64)
}

// optional bool myBool = 13;
inline void AllTypes::clear_mybool() {
  mybool_ = false;
}
inline bool AllTypes::mybool() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myBool)
  return mybool_;
}
inline void AllTypes::set_mybool(bool value) {
  
  mybool_ = value;
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myBool)
}

// optional string myString = 14;
inline void AllTypes::clear_mystring() {
  mystring_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& AllTypes::mystring() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myString)
  return mystring_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void AllTypes::set_mystring(const ::std::string& value) {
  
  mystring_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myString)
}
inline void AllTypes::set_mystring(const char* value) {
  
  mystring_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tutorial.AllTypes.myString)
}
inline void AllTypes::set_mystring(const char* value, size_t size) {
  
  mystring_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tutorial.AllTypes.myString)
}
inline ::std::string* AllTypes::mutable_mystring() {
  
  // @@protoc_insertion_point(field_mutable:tutorial.AllTypes.myString)
  return mystring_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* AllTypes::release_mystring() {
  // @@protoc_insertion_point(field_release:tutorial.AllTypes.myString)
  
  return mystring_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void AllTypes::set_allocated_mystring(::std::string* mystring) {
  if (mystring != NULL) {
    
  } else {
    
  }
  mystring_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), mystring);
  // @@protoc_insertion_point(field_set_allocated:tutorial.AllTypes.myString)
}

// optional bytes myBytes = 15;
inline void AllTypes::clear_mybytes() {
  mybytes_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& AllTypes::mybytes() const {
  // @@protoc_insertion_point(field_get:tutorial.AllTypes.myBytes)
  return mybytes_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void AllTypes::set_mybytes(const ::std::string& value) {
  
  mybytes_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tutorial.AllTypes.myBytes)
}
inline void AllTypes::set_mybytes(const char* value) {
  
  mybytes_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tutorial.AllTypes.myBytes)
}
inline void AllTypes::set_mybytes(const void* value, size_t size) {
  
  mybytes_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tutorial.AllTypes.myBytes)
}
inline ::std::string* AllTypes::mutable_mybytes() {
  
  // @@protoc_insertion_point(field_mutable:tutorial.AllTypes.myBytes)
  return mybytes_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* AllTypes::release_mybytes() {
  // @@protoc_insertion_point(field_release:tutorial.AllTypes.myBytes)
  
  return mybytes_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void AllTypes::set_allocated_mybytes(::std::string* mybytes) {
  if (mybytes != NULL) {
    
  } else {
    
  }
  mybytes_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), mybytes);
  // @@protoc_insertion_point(field_set_allocated:tutorial.AllTypes.myBytes)
}

// -------------------------------------------------------------------

// DataArray

// repeated .tutorial.AllTypes data = 1;
inline int DataArray::data_size() const {
  return data_.size();
}
inline void DataArray::clear_data() {
  data_.Clear();
}
inline const ::tutorial::AllTypes& DataArray::data(int index) const {
  // @@protoc_insertion_point(field_get:tutorial.DataArray.data)
  return data_.Get(index);
}
inline ::tutorial::AllTypes* DataArray::mutable_data(int index) {
  // @@protoc_insertion_point(field_mutable:tutorial.DataArray.data)
  return data_.Mutable(index);
}
inline ::tutorial::AllTypes* DataArray::add_data() {
  // @@protoc_insertion_point(field_add:tutorial.DataArray.data)
  return data_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::tutorial::AllTypes >*
DataArray::mutable_data() {
  // @@protoc_insertion_point(field_mutable_list:tutorial.DataArray.data)
  return &data_;
}
inline const ::google::protobuf::RepeatedPtrField< ::tutorial::AllTypes >&
DataArray::data() const {
  // @@protoc_insertion_point(field_list:tutorial.DataArray.data)
  return data_;
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace tutorial

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_all_5ftypes_2eproto__INCLUDED
