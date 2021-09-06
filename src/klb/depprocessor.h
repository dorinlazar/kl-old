#pragma once
#include "kl/klds.h"

template <typename T>
class DependencyProcessor {
  kl::Set<T> output;
  kl::Queue<T> processingQueue;

public:
  void add(const T& item) { processingQueue.push(item); }
  void add(const kl::List<T>& items) { processingQueue.push(items); }
  void process(std::function<kl::List<T>(const T&)> processor) {
    while (!processingQueue.empty()) {
      auto item = processingQueue.pop();
      output.add(item);
      auto res = processor(item);
      for (const auto& v: res) {
        if (!output.has(v) && !processingQueue.has(v)) {
          processingQueue.push(v);
        }
      }
    }
  }

  const auto& result() const { return output; }
};
