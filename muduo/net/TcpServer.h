// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include <muduo/base/Types.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{
namespace net
{

class Acceptor;
class EventLoop;
class InetAddress;
class EventLoopPool;

///
/// TCP server, supports single-threaded and thread-pool models.
///
/// This is an interface class, so don't expose too much details.
class TcpServer : boost::noncopyable
{
 public:

  TcpServer(EventLoop* loop, const InetAddress& listenAddr);
  TcpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& name);
  ~TcpServer();  // force out-line dtor, for scoped_ptr members.

  const string& name() const { return name_; }

  /// Set the number of threads for handling input.
  ///
  /// Always accepts new connection in loop's thread.
  /// Must be called before @c start
  /// @param numThreads
  /// - 0 means all I/O in loop's thread, no thread will created.
  ///   this is the default value.
  /// - 1 means all I/O in another thread.
  /// - N means a thread pool with N threads, new connections
  ///   are assigned on a round-robin basis.
  void setThreadNum(int numThreads);

  /// Starts the server if it's not listenning.
  ///
  /// It's harmless to call it multiple times.
  /// Not thread safe.
  void start();

  /// Set connection callback.
  /// Not thread safe.
  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  /// Set message callback.
  /// Not thread safe.
  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }

 private:
  /// Not thread safe.
  void newConnection(int sockfd, const InetAddress& peerAddr);
  /// Thread safe.
  void removeConnection(const TcpConnectionPtr& conn);
  /// Not thread safe.
  void removeConnectionInLoop(const TcpConnectionPtr& conn);

  typedef std::map<string, TcpConnectionPtr> ConnectionMap;

  EventLoop* loop_;  // the acceptor loop
  string name_;
  boost::scoped_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
  boost::scoped_ptr<EventLoopPool> threadPool_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  const string serverName_;
  bool started_;
  // always in loop thread
  int nextConnId_;
  ConnectionMap connections_;
};

}
}

#endif  // MUDUO_NET_TCPSERVER_H
