#!/usr/bin/env perl

use strict;
use warnings;
use ZMQ::LibZMQ3;
use ZMQ::Constants ':all';

my ($addr, @topics) = @ARGV;
die "Usage: zeromq_subscriber.pl address [topic]*\n" unless $addr;

my $ctx = zmq_init();
my $sub = zmq_socket($ctx, ZMQ_SUB);

push @topics, '' unless @topics;
zmq_setsockopt($sub, ZMQ_SUBSCRIBE, $_) for @topics;

zmq_connect($sub, $addr);

while (1) {
  my $topic   = zmq_recvmsg($sub);
  my $payload = zmq_recvmsg($sub);

  $topic   = zmq_msg_data($topic);
  $payload = zmq_msg_data($payload);

  print "[$topic]: $payload\n";
}
