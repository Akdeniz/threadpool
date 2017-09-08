#include <gtest/gtest.h>

#include <vector>

#include "threadpool.hpp"
#include "utils.hpp"

TEST( ThreadPoolTest, PushJobs )
{
  util::CThreadPool pool;

  std::vector<std::shared_future<int>> pending_tasks;

  for ( int index = 0; index < 10; ++index )
  {
    auto future = pool.PutTask( [index]() -> int {
      std::this_thread::sleep_for( std::chrono::seconds( index ) );
      return index;
    } );
    pending_tasks.push_back( future );
  }

  int total = 0;
  for ( auto it = pending_tasks.begin(); it != pending_tasks.end(); ++it )
  {
    it->wait();
    total += it->get();
  }

  ASSERT_EQ( 45, total );
}