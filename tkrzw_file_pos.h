/*************************************************************************************************
 * File implementations by positional access
 *
 * Copyright 2020 Google LLC
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License.  You may obtain a copy of the License at
 *     https://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.  See the License for the specific language governing permissions
 * and limitations under the License.
 *************************************************************************************************/

#ifndef _TKRZW_FILE_POS_H
#define _TKRZW_FILE_POS_H

#include <memory>
#include <string>
#include <vector>

#include <cinttypes>

#include "tkrzw_file.h"
#include "tkrzw_lib_common.h"

namespace tkrzw {

class PositionalParallelFileImpl;

/**
 * File implementation with positional access and locking for parallel operations.
 * @details Reading and writing operations are thread-safe; Multiple threads can access the same
 * file concurrently.  Other operations including Open, Close, Truncate, and Synchronize are not
 * thread-safe.  Moreover, locking doesn't assure atomicity of reading and writing operations.
 */
class PositionalParallelFile final : public File {
 public:
  /**
   * Default constructor
   */
  PositionalParallelFile();

  /**
   * Destructor.
   */
  virtual ~PositionalParallelFile();

  /**
   * Copy and assignment are disabled.
   */
  explicit PositionalParallelFile(const PositionalParallelFile& rhs) = delete;
  PositionalParallelFile& operator =(const PositionalParallelFile& rhs) = delete;

  /**
   * Opens a file.
   * @param path A path of the file.
   * @param writable If true, the file is writable.  If false, it is read-only.
   * @param options Bit-sum options.
   * @return The result status.
   * @details By default, exclusive locking against other processes is done for a writer and
   * shared locking against other processes is done for a reader.
   */
  Status Open(const std::string& path, bool writable, int32_t options = OPEN_DEFAULT) override;

  /**
   * Closes the file.
   * @return The result status.
   */
  Status Close() override;

  /**
   * Reads data.
   * @param off The offset of a source region.
   * @param buf The pointer to the destination buffer.
   * @param size The size of the data to be read.
   * @return The result status.
   */
  Status Read(int64_t off, void* buf, size_t size) override;

  /**
   * Writes data.
   * @param off The offset of the destination region.
   * @param buf The pointer to the source buffer.
   * @param size The size of the data to be written.
   * @return The result status.
   */
  Status Write(int64_t off, const void* buf, size_t size) override;

  /**
   * Appends data at the end of the file.
   * @param buf The pointer to the source buffer.
   * @param size The size of the data to be written.
   * @param off The pointer to an integer object to contain the offset at which the data has been
   * put.  If it is nullptr, it is ignored.
   * @return The result status.
   */
  Status Append(const void* buf, size_t size, int64_t* off = nullptr) override;

  /**
   * Expands the file size without writing data.
   * @param inc_size The size to increment the file size by.
   * @param old_size The pointer to an integer object to contain the old size of the file.
   * put.  If it is nullptr, it is ignored.
   * @return The result status.
   */
  Status Expand(size_t inc_size, int64_t* old_size = nullptr) override;

  /**
   * Truncates the file.
   * @param size The new size of the file.
   * @return The result status.
   */
  Status Truncate(int64_t size) override;

  /**
   * Synchronizes the content of the file to the file system.
   * @param hard True to do physical synchronization with the hardware or false to do only
   * logical synchronization with the file system.
   * @return The result status.
   * @details The pysical file size can be larger than the logical size in order to improve
   * performance by reducing frequency of allocation.  Thus, you should call this function before
   * accessing the file with external tools.
   */
  Status Synchronize(bool hard) override;

  /**
   * Gets the size of the file.
   * @param size The pointer to an integer object to contain the result size.
   * @return The result status.
   */
  Status GetSize(int64_t* size) override;

  /**
   * Sets allocation strategy.
   * @param init_size An initial size of allocation.
   * @param inc_factor A factor to increase the size of allocation.
   * @return The result status.
   * @details By default, the initial size is 1MB and the increasing factor is 2.  This method
   * must be called before the file is opened.
   */
  Status SetAllocationStrategy(int64_t init_size, double inc_factor) override;

  /**
   * Checks whether operations are done by memory mapping.
   * @return True if operations are done by memory mapping, or false if not.
   */
  bool IsMemoryMapping() const override {
    return false;
  }

  /**
   * Checks whether updating operations are atomic and thread-safe.
   * @return Always false.  Atomicity is not assured.  Some operations are not thread-safe.
   */
  bool IsAtomic() const override {
    return false;
  }

  /**
   * Makes a new file object of the same concrete class.
   * @return The new file object.
   */
  std::unique_ptr<File> MakeFile() const override {
    return std::make_unique<PositionalParallelFile>();
  }

 private:
  /** Pointer to the actual implementation. */
  PositionalParallelFileImpl* impl_;
};

class PositionalAtomicFileImpl;

/**
 * File implementation by positional access and locking for atomic operations.
 * @details All operations are thread-safe; Multiple threads can access the same file concurrently.
 * Also, locking assures that every operation is observed in an atomic manner.
 */
class PositionalAtomicFile final : public File {
 public:
  /**
   * Default constructor
   */
  PositionalAtomicFile();

  /**
   * Destructor.
   */
  virtual ~PositionalAtomicFile();

  /**
   * Copy and assignment are disabled.
   */
  explicit PositionalAtomicFile(const PositionalAtomicFile& rhs) = delete;
  PositionalAtomicFile& operator =(const PositionalAtomicFile& rhs) = delete;

  /**
   * Opens a file.
   * @param path A path of the file.
   * @param writable If true, the file is writable.  If false, it is read-only.
   * @param options Bit-sum options.
   * @return The result status.
   * @details By default, exclusive locking against other processes is done for a writer and
   * shared locking against other processes is done for a reader.
   */
  Status Open(const std::string& path, bool writable, int32_t options = OPEN_DEFAULT) override;

  /**
   * Closes the file.
   * @return The result status.
   */
  Status Close() override;

  /**
   * Reads data.
   * @param off The offset of a source region.
   * @param buf The pointer to the destination buffer.
   * @param size The size of the data to be read.
   * @return The result status.
   */
  Status Read(int64_t off, void* buf, size_t size) override;

  /**
   * Writes data.
   * @param off The offset of the destination region.
   * @param buf The pointer to the source buffer.
   * @param size The size of the data to be written.
   * @return The result status.
   */
  Status Write(int64_t off, const void* buf, size_t size) override;

  /**
   * Appends data at the end of the file.
   * @param buf The pointer to the source buffer.
   * @param size The size of the data to be written.
   * @param off The pointer to an integer object to contain the offset at which the data has been
   * put.  If it is nullptr, it is ignored.
   * @return The result status.
   */
  Status Append(const void* buf, size_t size, int64_t* off = nullptr) override;

  /**
   * Expands the file size without writing data.
   * @param inc_size The size to increment the file size by.
   * @param old_size The pointer to an integer object to contain the old size of the file.
   * put.  If it is nullptr, it is ignored.
   * @return The result status.
   */
  Status Expand(size_t inc_size, int64_t* old_size = nullptr) override;

  /**
   * Truncates the file.
   * @param size The new size of the file.
   * @return The result status.
   */
  Status Truncate(int64_t size) override;

  /**
   * Synchronizes the content of the file to the file system.
   * @param hard True to do physical synchronization with the hardware or false to do only
   * logical synchronization with the file system.
   * @return The result status.
   * @details The pysical file size can be larger than the logical size in order to improve
   * performance by reducing frequency of allocation.  Thus, you should call this function before
   * accessing the file with external tools.
   */
  Status Synchronize(bool hard) override;

  /**
   * Gets the size of the file.
   * @param size The pointer to an integer object to contain the result size.
   * @return The result status.
   */
  Status GetSize(int64_t* size) override;

  /**
   * Sets allocation strategy.
   * @param init_size An initial size of allocation.
   * @param inc_factor A factor to increase the size of allocation.
   * @return The result status.
   * @details By default, the initial size is 1MB and the increasing factor is 2.  This method
   * must be called before the file is opened.
   */
  Status SetAllocationStrategy(int64_t init_size, double inc_factor) override;

  /**
   * Checks whether operations are done by memory mapping.
   * @return True if operations are done by memory mapping, or false if not.
   */
  bool IsMemoryMapping() const override {
    return false;
  }

  /**
   * Checks whether updating operations are atomic and thread-safe.
   * @return Always true.  Atomicity is assured.  All operations are thread-safe.
   */
  bool IsAtomic() const override {
    return true;
  }

  /**
   * Makes a new file object of the same concrete class.
   * @return The new file object.
   */
  std::unique_ptr<File> MakeFile() const override {
    return std::make_unique<PositionalAtomicFile>();
  }

 private:
  /** Pointer to the actual implementation. */
  PositionalAtomicFileImpl* impl_;
};

}  // namespace tkrzw

#endif  // _TKRZW_FILE_POS_H

// END OF FILE
