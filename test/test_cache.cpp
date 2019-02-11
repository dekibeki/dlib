#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/cache.hpp>
#include <unordered_map>
#include <string>

namespace {
  template<typename K, typename V>
  struct Get_as_shared_ptr {
    std::optional<std::shared_ptr<const V>> operator()(std::unordered_map<K,V>* map, K key) const noexcept {
      const auto found = map->find(key);
      if (found == map->end()) {
        return std::nullopt;
      } else {
        return std::make_shared<const V>(found->second);
      }
    }
  };
}

BOOST_AUTO_TEST_CASE(cache_string_string_no_source) {
  auto cache_res = dlib::make<dlib::Cache<std::string, std::string>>();
  BOOST_TEST(!!cache_res);
  auto&& cache{ cache_res.value() };

  BOOST_TEST(!cache->read_through("test 1"));
  BOOST_TEST(!cache->read_through("test 2"));
}

BOOST_AUTO_TEST_CASE(cache_reads) {
  using Cache = dlib::Cache<int, int>;
  using Map = std::unordered_map<int, int>;
  auto map{ std::make_shared<Map>() };

  (*map)[0] = 1;

  

  Cache::Source map_interface{ std::move(map), dlib::finder_get(Get_as_shared_ptr<int,int>{}) };
 
  

  auto cache_res = dlib::make<Cache>();
  BOOST_TEST(!!cache_res);
  auto&& cache{ cache_res.value() };
  BOOST_TEST(!cache->shallow_read(0));
  BOOST_TEST(!cache->read_through(0));
  BOOST_TEST(!cache->deep_read(0));
  cache->add_source(std::move(map_interface));
  BOOST_TEST(!cache->shallow_read(0));
  BOOST_TEST(!!cache->read_through(0));
  BOOST_TEST(!!cache->shallow_read(0));
  cache->flush(0);
  BOOST_TEST(!cache->shallow_read(0));
  BOOST_TEST(!!cache->deep_read(0));
  BOOST_TEST(!!cache->shallow_read(0));
  cache->flush();
  BOOST_TEST(!cache->shallow_read(0));
  BOOST_TEST(!!cache->read_through(0));
  BOOST_TEST(!!cache->deep_read(0));
}