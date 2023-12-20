#include <benchmark/benchmark.h>
#include <boost/container/flat_map.hpp>
#include <map>
#include <optional>
#include <random>
#include <unordered_map>

using ItemId = size_t;
using Amount = size_t;

template <template <class, class> class Container>
class Orders {
   public:
    template <class T>
    void iterate(T callback) {
        for (auto& [key, value] : storage) {
            callback(key, value);
        }
    }

    void add(ItemId id, Amount record) {
        storage[id] += record;
    }

    std::optional<Amount> get(ItemId id) {
        if (auto iter = storage.find(id); iter != storage.end()) {
            return iter->second;
        }
        return {};
    }

   private:
    Container<ItemId, Amount> storage;
};

template <template <class, class> class Container>
void queriesBench(benchmark::State& state) {
    // play with coeffs!
    const std::size_t totalQueries = 100000;
    const std::size_t maxNewItemId = state.range();
    const std::size_t maxReqItemId = maxNewItemId * 2;
    const float addProbability = 0.5;
    const float iterateProbability = 0.6;

    std::mt19937_64 random;
    std::uniform_real_distribution<float> dice(0, 1.);
    std::uniform_int_distribution<ItemId> newUsers(0, maxNewItemId);
    std::uniform_int_distribution<ItemId> reqUsers(0, maxReqItemId);
    std::uniform_int_distribution<Amount> amounts(0, 10);

    for (auto _ : state) {
        Orders<Container> orders;
        size_t iterateWorkProof{};

        for (size_t i = 0; i < totalQueries; ++i) {
            const float diceValue = dice(random);

            if (diceValue < addProbability) {
                // Warning! Order of evaluation
                orders.add(newUsers(random), amounts(random));
            } else if (diceValue < iterateProbability) {
                orders.iterate([&](const ItemId& id, const Amount& amount) {
                    iterateWorkProof += amount;
                });
            } else {
                benchmark::DoNotOptimize(orders.get(reqUsers(random)));
            }
        }
        benchmark::DoNotOptimize(iterateWorkProof);
    }
}

BENCHMARK_TEMPLATE(queriesBench, std::map)
    ->Arg(10)
    ->Arg(1000)
    ->Arg(100000)
    ->Unit(benchmark::kMillisecond);
;
BENCHMARK_TEMPLATE(queriesBench, std::unordered_map)
    ->Arg(10)
    ->Arg(1000)
    ->Arg(100000)
    ->Unit(benchmark::kMillisecond);
;
BENCHMARK_TEMPLATE(queriesBench, boost::container::flat_map)
    ->Arg(10)
    ->Arg(1000)
    ->Arg(100000)
    ->Unit(benchmark::kMillisecond);
;
BENCHMARK_MAIN();