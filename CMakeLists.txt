cmake_minimum_required (VERSION 3.8)

project(dlib CXX)

set(dlibInclude "${CMAKE_CURRENT_SOURCE_DIR}/")
set(dlibSrc "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(dlibTest "${CMAKE_CURRENT_SOURCE_DIR}/test")

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_DEBUG ON)
set(Boost_COMPILER -vc141)

find_package(Boost 
  REQUIRED 
  COMPONENTS unit_test_framework)

find_package(date CONFIG REQUIRED)

add_library(dlib
  ${dlibSrc}/args.cpp
  ${dlibSrc}/arrays.cpp
  ${dlibSrc}/cache.cpp
  ${dlibSrc}/db.cpp
  ${dlibSrc}/dummy_db.cpp
  ${dlibSrc}/error.cpp
  ${dlibSrc}/finder_interface.cpp
  ${dlibSrc}/interface.cpp
  ${dlibSrc}/iterators.cpp
  ${dlibSrc}/meta.cpp
  ${dlibSrc}/math.cpp
  ${dlibSrc}/outcome.cpp
  ${dlibSrc}/pool.cpp
  ${dlibSrc}/quaternion.cpp
  ${dlibSrc}/raft.cpp
  ${dlibSrc}/serialization.cpp
  ${dlibSrc}/soa.cpp
  ${dlibSrc}/soa_reference.cpp
  ${dlibSrc}/strong_db.cpp
  ${dlibSrc}/strong_type.cpp
  ${dlibSrc}/tuples.cpp
  ${dlibSrc}/unique_value.cpp
  ${dlibSrc}/util.cpp
  ${dlibSrc}/vector.cpp
  ${dlibSrc}/vector_adaptors.cpp
)

add_executable(dlibTest 
  ${dlibTest}/testMain.cpp
  ${dlibTest}/test_arrays.cpp
  ${dlibTest}/test_cache.cpp
  ${dlibTest}/test_db.cpp
  ${dlibTest}/test_dummy_db.cpp
  ${dlibTest}/test_error.cpp
  ${dlibTest}/test_finder_interface.cpp
  ${dlibTest}/test_interface.cpp
  ${dlibTest}/test_iterators.cpp
  ${dlibTest}/test_outcome.cpp
  ${dlibTest}/test_pool.cpp
  ${dlibTest}/test_quaternion.cpp
  ${dlibTest}/test_serialization.cpp
  ${dlibTest}/test_soa.cpp
  ${dlibTest}/test_strong_type.cpp
  ${dlibTest}/test_tuples.cpp
  ${dlibTest}/test_vector_adaptors.cpp
  )

target_include_directories(dlib PUBLIC
  ${dlibInclude}
  ${Boost_INCLUDE_DIRS}
  )

target_include_directories(dlibTest PUBLIC 
  ${dlibInclude}
  ${Boost_INCLUDE_DIRS}
  )

target_compile_features(dlib PUBLIC cxx_std_17)
target_compile_features(dlibTest PUBLIC cxx_std_17)

target_link_libraries(dlib
  )

target_link_libraries(dlibTest 
  dlib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

find_package(PostgreSQL)

if(${PostgreSQL_FOUND})
  message("found postgresql")
  set(DLIB_POSTGRESQL_FOUND true)
  
  add_library(dlib_postgresql
    ${dlibSrc}/postgresql.cpp
	${dlibSrc}/strong_postgresql.cpp
    )

  add_executable(dlib_postgresqlTest
    ${dlibTest}/postgresql_testMain.cpp
    ${dlibTest}/test_postgresql.cpp
	#${dlibTest}/test_strong_postgresql.cpp
    )

  target_include_directories(dlib_postgresql
    PUBLIC
    ${dlibInclude}
    ${Boost_INCLUDE_DIRS}
    PRIVATE
    ${PostgreSQL_INCLUDE_DIRS}
    )

  target_include_directories(dlib_postgresqlTest
    PUBLIC
    ${dlibInclude}
    PRIVATE
    ${Boost_INCLUDE_DIRS})

  target_compile_features(dlib_postgresql PUBLIC cxx_std_17)

  target_compile_features(dlib_postgresqlTest PUBLIC cxx_std_17)

  target_link_libraries(dlib_postgresql
    PUBLIC
    dlib
	date::date-tz
	date::date
    PRIVATE
    ${PostgreSQL_LIBRARIES})

  target_link_libraries(dlib_postgresqlTest
    dlib_postgresql
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
else()
  set(DLIB_POSTGRESQL_FOUND false)
  message("did not find postgresql")
endif()

find_package(SQLite3)

if(${SQLite3_FOUND})
  message("found sqlite3")
  set(DLIB_SQLITE_FOUND true)

  find_path(Sqlite3_INCLUDE_DIRS sqlite3.h)

  add_library(dlib_sqlite
    ${dlibSrc}/sqlite.cpp
    )

  add_executable(dlib_sqliteTest
    ${dlibTest}/sqlite_testMain.cpp
    ${dlibTest}/test_sqlite.cpp)

  target_include_directories(dlib_sqlite
    PUBLIC
    ${dlibInclude}
    PRIVATE
    ${Sqlite3_INCLUDE_DIRS}
    )

  target_include_directories(dlib_sqliteTest
    PUBLIC
    ${dlibInclude}
    PRIVATE
    ${Boost_INCLUDE_DIRS})

  target_compile_features(dlib_sqlite PUBLIC cxx_std_17)

  target_compile_features(dlib_sqliteTest PUBLIC cxx_std_17)

  target_link_libraries(dlib_sqlite
    PUBLIC
    dlib
    PRIVATE
    ${SQLite3_LIBRARIES})

  target_link_libraries(dlib_sqliteTest 
    dlib_sqlite
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
else()
  set(DLIB_SQLITE_FOUND false)
  message("did not find sqlite3")
endif()