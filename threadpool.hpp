#pragma once

#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

#include <boost/asio/io_service.hpp>

namespace util
{
class CThreadPool
{
public:
  explicit CThreadPool( size_t worker_count = std::thread::hardware_concurrency() + 1 )
      : io_service_(), placeholder_work_( io_service_ ), threads_(), semaphore_( worker_count )
  {
    for ( size_t i = 0; i < worker_count; ++i )
      threads_.emplace_back( std::bind<size_t ( boost::asio::io_service::* )(), boost::asio::io_service*>(
          &boost::asio::io_service::run, &io_service_ ) );
  }

  template <class F>
  std::shared_future<typename std::result_of<F()>::type> PutTask( F&& f )
  {
    using result_t = typename std::result_of<F()>::type;
    semaphore_.wait();
    auto task = std::make_shared<std::packaged_task<result_t()>>( f );
    std::shared_future<result_t> fut( task->get_future() );

    io_service_.post( [task, this] {
      ( *task )();
      semaphore_.notify();
    } );

    return fut;
  }

  virtual ~CThreadPool()
  {
    io_service_.stop();
    for ( auto& thread : threads_ )
    {
      if ( thread.joinable() )
        thread.join();
    }
  }

private:
  class CSemaphore
  {
  public:
    explicit CSemaphore( int count = 0 ) : count_( count )
    {
    }

    inline void notify()
    {
      std::unique_lock<std::mutex> lock( mtx );
      count_++;
      condition_variable_.notify_one();
    }

    inline void wait()
    {
      std::unique_lock<std::mutex> lock( mtx );

      while ( count_ == 0 )
      {
        condition_variable_.wait( lock );
      }
      count_--;
    }

  private:
    std::mutex mtx;
    std::condition_variable condition_variable_;
    int count_;
  };

  boost::asio::io_service io_service_;
  boost::asio::io_service::work placeholder_work_;
  std::vector<std::thread> threads_;
  CSemaphore semaphore_;
};

}  // ns util
