/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_LOGGER_HPP
#define KAGOME_LOGGER_HPP

#include <sstream>

#include <boost/assert.hpp>
#include <optional>
#include <soralog/level.hpp>
#include <soralog/logger.hpp>
#include <soralog/logging_system.hpp>
#include <soralog/macro.hpp>

#include "common/hexutil.hpp"

namespace kagome::log {

  using Level = soralog::Level;
  using Logger = std::shared_ptr<soralog::Logger>;
  using WLogger = std::weak_ptr<soralog::Logger>;

  enum class Error : uint8_t { WRONG_LEVEL = 1, WRONG_GROUP, WRONG_LOGGER };

  outcome::result<Level> str2lvl(std::string_view str);

  void setLoggingSystem(std::shared_ptr<soralog::LoggingSystem> logging_system);

  void tuneLoggingSystem(const std::vector<std::string> &cfg);

  static const std::string defaultGroupName("kagome");

  [[nodiscard]] Logger createLogger(const std::string &tag);

  [[nodiscard]] Logger createLogger(const std::string &tag,
                                    const std::string &group);

  [[nodiscard]] Logger createLogger(const std::string &tag,
                                    const std::string &group,
                                    Level level);

  bool setLevelOfGroup(const std::string &group_name, Level level);
  bool resetLevelOfGroup(const std::string &group_name);

  bool setLevelOfLogger(const std::string &logger_name, Level level);
  bool resetLevelOfLogger(const std::string &logger_name);

  template <typename T, typename Ret>
  Ret format_arg(const T &t) {
    return static_cast<Ret>(t);
  }

  template <typename T>
  auto format_arg(const T *t) {
    return fmt::ptr(t);
  }
  inline std::string_view format_arg(std::string_view s) {
    return s;
  }

  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  const T &format_arg(const T &arg) {
    return arg;
  }

  inline std::string format_arg(gsl::span<const uint8_t> buffer) {
    if (buffer.size() == 0) {
      return "";
    }
    std::string res;
    if (std::all_of(buffer.begin(), buffer.end(), isalnum)) {
      res.resize(buffer.size());
      std::copy_n(buffer.begin(), buffer.size(), res.begin());
    } else {
      res = ::kagome::common::hex_lower(buffer);
    }
    if (res.size() > 256) {
      return res.substr(0, 256) + "…";
    }
    return res;
  }

  template <typename T>
  auto format_arg(std::reference_wrapper<T> t) {
    return format_arg(t.get());
  }

  template <typename T>
  auto format_arg(const std::optional<T> &t) {
    return fmt::format("{}", t);
  }

  template <typename Ret, typename... Args>
  void trace_function_call(const Logger &logger,
                           std::string_view func_name,
                           Ret &&ret,
                           Args &&...args) {
    if (sizeof...(args) > 0) {
      std::string ss;
      (fmt::format_to(std::back_inserter(ss),
                      "{}, ",
                      format_arg(std::forward<Args>(args))),
       ...);
      logger->trace("call '{}', args: {}-> ret: {}",
                    func_name,
                    ss,
                    format_arg(std::forward<Ret>(ret)));
    } else {
      logger->trace("call '{}' -> ret: {}",
                    func_name,
                    format_arg(std::forward<Ret>(ret)));
    }
  }

  template <typename... Args>
  void trace_void_function_call(const Logger &logger,
                                std::string_view func_name,
                                Args &&...args) {
    if (sizeof...(args) > 0) {
      std::string ss;
      (fmt::format_to(std::back_inserter(ss),
                      "{}, ",
                      format_arg(std::forward<Args>(args))),
       ...);
      logger->trace("call '{}', args: {}", func_name, ss);
    } else {
      logger->trace("call '{}'", func_name);
    }
  }

#ifdef NDEBUG

#define SL_TRACE_FUNC_CALL(logger, ret, ...)
#define SL_TRACE_VOID_FUNC_CALL(logger, ...)

#else

#define SL_TRACE_FUNC_CALL(logger, ret, ...) \
  ::kagome::log::trace_function_call(        \
      (logger), __FUNCTION__, (ret), ##__VA_ARGS__)

#define SL_TRACE_VOID_FUNC_CALL(logger, ...) \
  ::kagome::log::trace_void_function_call((logger), __FUNCTION__, ##__VA_ARGS__)

#endif

}  // namespace kagome::log

OUTCOME_HPP_DECLARE_ERROR(kagome::log, Error);

template <typename T>
struct fmt::formatter<std::optional<T>> {
  // Parses format specifications. Must be empty
  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();

    // Check if reached the end of the range:
    if (it != end && *it != '}') {
      throw format_error("invalid format");
    }

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the optional value
  template <typename FormatContext>
  auto format(const std::optional<T> &opt, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.

    if (opt.has_value()) {
      return format_to(ctx.out(), "{}", opt.value());
    } else {
      return format_to(ctx.out(), "<none>");
    }
  }
};

#endif  // KAGOME_LOGGER_HPP
